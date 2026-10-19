from datetime import datetime, timedelta
import json
import os
import random
import threading
import time
import urllib.error
import urllib.request

from flask import Flask, request, render_template_string, jsonify, Response

try:
    import paho.mqtt.client as mqtt
except ImportError:
    mqtt = None

app = Flask(__name__)
BASE_DIR = os.path.dirname(__file__)


def load_local_env():
    env_path = os.path.join(BASE_DIR, ".env")
    if not os.path.exists(env_path):
        return
    try:
        with open(env_path, "r", encoding="utf-8") as env_file:
            for line in env_file:
                line = line.strip()
                if not line or line.startswith("#") or "=" not in line:
                    continue
                key, value = line.split("=", 1)
                key = key.strip()
                value = value.strip().strip('"').strip("'")
                if key and key not in os.environ:
                    os.environ[key] = value
    except OSError as exc:
        print(f"读取本地环境配置失败: {exc}")


load_local_env()
DATA_DIR = os.path.join(BASE_DIR, "data")
HEALTH_HISTORY_PATH = os.path.join(DATA_DIR, "health_history.json")
TRAINING_HISTORY_PATH = os.path.join(DATA_DIR, "training_history.json")
MQTT_ENABLED = os.environ.get("MQTT_ENABLED", "1").strip().lower() not in ("0", "false", "no", "off")
MQTT_HOST = os.environ.get("MQTT_HOST", "127.0.0.1")
MQTT_PORT = int(os.environ.get("MQTT_PORT", "1883") or 1883)
MQTT_TOPIC = os.environ.get("MQTT_TOPIC", "rehab/upper/#")
MQTT_CLIENT_ID = os.environ.get("MQTT_CLIENT_ID", "rehab-web-dashboard")

system_state = {
    "is_training": False,
    "training_started_at": None,
    "last_page": "home",
}

latest_data = {
    "data": "暂无数据",
    "raw": {},
    "parsed": {},
    "time": "",
    "count": 0,
}

latest_health = {
    "parsed": {},
    "time": "",
}

history_records = []
health_history_records = []
last_health_history_key = None
last_training_history_key = None
state_condition = threading.Condition()
state_version = 0


def build_latest_payload():
    realtime = normalize_realtime(latest_data.get("parsed", {}))
    devices = normalize_devices(latest_data.get("parsed", {}))
    health = normalize_health(latest_health.get("parsed", {}), latest_health.get("time", ""))
    return {
        **latest_data,
        "system_state": system_state,
        "realtime": realtime,
        "devices": devices,
        "health": health,
    }


def notify_realtime_update():
    global state_version
    with state_condition:
        state_version += 1
        state_condition.notify_all()


def parse_kv_payload(data):
    """Parse MCU payload like tick=1,battery=86,page=home into typed fields."""
    parsed = {}
    if not isinstance(data, str):
        return parsed

    for item in data.replace("&", ",").split(","):
        if "=" not in item:
            continue
        key, value = item.split("=", 1)
        key = key.strip()
        value = value.strip()
        if not key:
            continue
        try:
            parsed[key] = int(value)
        except ValueError:
            try:
                parsed[key] = float(value)
            except ValueError:
                parsed[key] = value
    return parsed


PAGE_ID_NAMES = {
    0: "home",
    1: "prep",
    2: "action",
    3: "training",
    4: "result",
    5: "settings",
    6: "history",
    7: "analytics",
    8: "link_lost",
    9: "free_training",
    10: "free_result",
}


def payload_page_name(parsed):
    page = parsed.get("page")
    if isinstance(page, str) and page:
        return page
    try:
        return PAGE_ID_NAMES.get(int(parsed.get("pg")), "")
    except (TypeError, ValueError):
        return ""


def normalize_motion_angle(value, action_index=None):
    try:
        raw_angle = abs(float(value))
        angle = int(raw_angle)
    except (TypeError, ValueError):
        raw_angle = 0.0
        angle = 0
    angle = max(0, min(180, angle))
    return angle


def normalize_realtime(parsed):
    battery = int(parsed.get("battery", parsed.get("ubat", parsed.get("uba", 0))) or 0)
    reps = int(parsed.get("reps", parsed.get("n", 0)) or 0)
    angle = int(parsed.get("angle", parsed.get("ang", parsed.get("elbow", parsed.get("e", 0)))) or 0)
    elbow_raw = int(parsed.get("elbow", parsed.get("e", 0)) or 0)
    forearm_raw = int(parsed.get("forearm", parsed.get("f", 0)) or 0)
    arm_abd_raw = int(parsed.get("abd", parsed.get("ab", 0)) or 0)
    shoulder_raw = int(parsed.get("shoulder", parsed.get("sh", 0)) or 0)
    score = int(parsed.get("score", parsed.get("s", min(100, reps * 8))) or 0)
    emg = int(parsed.get("emg", parsed.get("em", 0)) or 0)
    emg_strength = int(parsed.get("emg_strength", emg) or 0)
    emg_mode = int(parsed.get("emg_mode", 0) or 0)
    emg_label_raw = str(parsed.get("emg_label", "") or "").strip().lower()
    bt = int(parsed.get("bt", parsed.get("ubt", parsed.get("ub", 0))) or 0)
    gyro = float(parsed.get("gyro", 0) or 0)

    action_map = {
        0: "肘关节屈伸",
        1: "肩部上举",
        2: "前臂旋转",
        3: "手臂外展",
        4: "待识别",
    }
    action_raw = parsed.get("action", parsed.get("a", parsed.get("action_index", parsed.get("ai", 4))))
    try:
        action_index = int(action_raw)
    except (TypeError, ValueError):
        action_index = 4
    action = action_map.get(action_index, str(action_raw))
    target_reps_raw = parsed.get("target_reps", parsed.get("tr", parsed.get("g", None)))
    if target_reps_raw is None and "target" in parsed and not any(
        key in parsed for key in ("action", "a", "action_index", "ai")
    ):
        target_reps_raw = parsed.get("target")
    target_reps = int(target_reps_raw or 5)
    if "cpl" in parsed or "progress" in parsed or "pv" in parsed:
        completion = max(0, min(100, int(parsed.get("cpl", parsed.get("progress", parsed.get("pv", 0))) or 0)))
    else:
        completion = max(0, min(100, int((reps * 100) / max(1, target_reps))))
    emg_label_map = {
        0: "放松",
        1: "较弱",
        2: "用力",
    }
    angle_by_action = {
        0: ("肘关节角度", elbow_raw if elbow_raw else angle),
        1: ("上举角度", shoulder_raw if shoulder_raw else angle),
        2: ("旋转角度", forearm_raw if forearm_raw else angle),
        3: ("外展角度", arm_abd_raw if arm_abd_raw else angle),
    }
    angle_name, display_angle_raw = angle_by_action.get(action_index, ("动作角度", angle))
    display_angle = normalize_motion_angle(display_angle_raw, action_index)
    elbow_angle = normalize_motion_angle(elbow_raw)
    forearm_angle = normalize_motion_angle(forearm_raw, 2)
    arm_abd_angle = normalize_motion_angle(arm_abd_raw)
    shoulder_angle = normalize_motion_angle(shoulder_raw)
    max_angle = normalize_motion_angle(parsed.get("max", parsed.get("m", 0)), action_index)
    avg_angle = normalize_motion_angle(parsed.get("avg", parsed.get("av", 0)), action_index)
    # EMG is sampled by the upper-arm sensor for every training action.  The
    # action index selects the angle algorithm; it must not disable real EMG.
    emg_enabled = any(
        key in parsed
        for key in ("emg", "em", "emg_strength", "emg_mode", "emg_label")
    )
    if emg_enabled:
        if emg_label_raw in ("relax", "weak", "strong"):
            emg_label = {"relax": "放松", "weak": "较弱", "strong": "用力"}[emg_label_raw]
        else:
            emg_label = emg_label_map.get(emg_mode, "放松")
        emg_strength = max(0, min(100, emg_strength))
        emg_mode = max(0, min(2, emg_mode))
    else:
        emg = 0
        emg_strength = 0
        emg_mode = 0
        emg_label = "未启用"

    return {
        "battery": max(0, min(100, battery)),
        "bluetooth_connected": bool(bt),
        "reps": reps,
        "elbow_angle": elbow_angle,
        "forearm_angle": forearm_angle,
        "arm_abd_angle": arm_abd_angle,
        "shoulder_angle": shoulder_angle,
        "display_angle": display_angle,
        "angle_name": angle_name,
        "action_index": action_index,
        "score": max(0, min(100, score)),
        "amplitude": max(0, min(100, int(parsed.get("amp", 0) or 0))),
        "max_angle": max_angle,
        "avg_angle": avg_angle,
        "target_reps": target_reps,
        "paused": bool(int(parsed.get("paused", 0) or 0)),
        "emg": emg,
        "emg_strength": emg_strength,
        "emg_mode": emg_mode,
        "emg_label": emg_label,
        "emg_enabled": emg_enabled,
        "action": action,
        "completion": completion,
        "gyro": gyro,
    }

def normalize_devices(parsed):
    upper_battery = int(parsed.get("ubat", parsed.get("uba", parsed.get("battery", 0))) or 0)
    wrist_battery = int(parsed.get("wbat", parsed.get("wba", 0)) or 0)
    return {
        "upper": {
            "bluetooth_connected": bool(int(parsed.get("ubt", parsed.get("ub", parsed.get("bt", 0))) or 0)),
            "battery": max(0, min(100, upper_battery)),
            "battery_valid": bool(int(parsed.get("ubatv", parsed.get("bv", 0)) or 0)),
            "sleeping": bool(int(parsed.get("usleep", 0) or 0)),
        },
        "wrist": {
            "bluetooth_connected": bool(int(parsed.get("wbt", parsed.get("wb", 0)) or 0)),
            "battery": max(0, min(100, wrist_battery)),
            "battery_valid": bool(int(parsed.get("wbatv", 0) or 0)),
            "sleeping": bool(int(parsed.get("wsleep", 0) or 0)),
            "frame_valid": bool(int(parsed.get("wv", 0) or 0)),
            "health_active": bool(int(parsed.get("wh", 0) or 0)),
            "health_enter": bool(int(parsed.get("whe", 0) or 0)),
            "health_exit": bool(int(parsed.get("whexit", 0) or 0)),
            "health_request": bool(int(parsed.get("hreq", 0) or 0)),
        },
    }


def normalize_health(parsed, updated_at=""):
    return {
        "valid": bool(int(parsed.get("hv", 0) or 0)),
        "heart_rate": int(parsed.get("hr", 0) or 0),
        "spo2": int(parsed.get("spo2", 0) or 0),
        "micro_circ": int(parsed.get("micro", 0) or 0),
        "fatigue": int(parsed.get("fatigue", 0) or 0),
        "sbp": int(parsed.get("sbp", 0) or 0),
        "dbp": int(parsed.get("dbp", 0) or 0),
        "sdnn": int(parsed.get("sdnn", 0) or 0),
        "rmssd": int(parsed.get("rmssd", 0) or 0),
        "seq": int(parsed.get("hseq", 0) or 0),
        "age_ms": int(parsed.get("hage", 0) or 0),
        "updated_at": updated_at,
    }


def load_health_history():
    global health_history_records
    if not os.path.exists(HEALTH_HISTORY_PATH):
        health_history_records = []
        return
    try:
        with open(HEALTH_HISTORY_PATH, "r", encoding="utf-8") as fp:
            records = json.load(fp)
        health_history_records = records if isinstance(records, list) else []
    except (OSError, json.JSONDecodeError):
        health_history_records = []


def save_health_history():
    os.makedirs(DATA_DIR, exist_ok=True)
    temp_path = HEALTH_HISTORY_PATH + ".tmp"
    with open(temp_path, "w", encoding="utf-8") as fp:
        json.dump(health_history_records[-500:], fp, ensure_ascii=False, indent=2)
    os.replace(temp_path, HEALTH_HISTORY_PATH)


def load_training_history():
    global history_records
    if not os.path.exists(TRAINING_HISTORY_PATH):
        history_records = []
        return
    try:
        with open(TRAINING_HISTORY_PATH, "r", encoding="utf-8") as fp:
            records = json.load(fp)
        history_records = records if isinstance(records, list) else []
    except (OSError, json.JSONDecodeError):
        history_records = []


def save_training_history():
    os.makedirs(DATA_DIR, exist_ok=True)
    temp_path = TRAINING_HISTORY_PATH + ".tmp"
    with open(temp_path, "w", encoding="utf-8") as fp:
        json.dump(history_records[:500], fp, ensure_ascii=False, indent=2)
    os.replace(temp_path, TRAINING_HISTORY_PATH)


def add_health_history_record(parsed, updated_at):
    global last_health_history_key
    health = normalize_health(parsed, updated_at)
    if not health["valid"] or health["heart_rate"] <= 0:
        return

    if health["seq"] > 0:
        record_key = f"seq:{health['seq']}"
    else:
        record_key = "value:%s:%s:%s:%s:%s" % (
            updated_at,
            health["heart_rate"],
            health["spo2"],
            health["sdnn"] or health["rmssd"],
            health["fatigue"],
        )
    if record_key == last_health_history_key:
        return
    if health["seq"] > 0 and any(item.get("seq") == health["seq"] for item in health_history_records[-12:]):
        last_health_history_key = record_key
        return

    record = {
        "id": (health_history_records[-1]["id"] + 1) if health_history_records else 1,
        "label": updated_at.split(" ")[-1] if updated_at else datetime.now().strftime("%H:%M:%S"),
        "updated_at": updated_at,
        "heart_rate": health["heart_rate"],
        "spo2": health["spo2"],
        "micro": health["micro_circ"],
        "micro_circ": health["micro_circ"],
        "fatigue": health["fatigue"],
        "hrv": health["sdnn"] or health["rmssd"],
        "sdnn": health["sdnn"],
        "rmssd": health["rmssd"],
        "sbp": health["sbp"],
        "dbp": health["dbp"],
        "seq": health["seq"],
    }
    health_history_records.append(record)
    if len(health_history_records) > 500:
        del health_history_records[:-500]
    last_health_history_key = record_key
    save_health_history()


def add_history_record(parsed=None, source="manual"):
    global last_training_history_key
    parsed = parsed if parsed is not None else latest_data.get("parsed", {})
    realtime = normalize_realtime(parsed)
    reps = int(realtime["reps"] or 0)
    target_reps = int(realtime["target_reps"] or 0)
    score = int(realtime["score"] or 0)
    max_angle = int(realtime["max_angle"] or realtime["display_angle"] or 0)
    avg_angle = int(realtime["avg_angle"] or realtime["display_angle"] or 0)
    action = realtime["action"] if realtime["action"] != "待识别" else "肘关节屈伸"
    record_key = (
        action,
        reps,
        target_reps,
        score,
        int(realtime["completion"] or 0),
        int(realtime["amplitude"] or 0),
        max_angle,
        avg_angle,
    )
    if source != "manual":
        if reps <= 0 and score <= 0 and max_angle <= 0:
            return False
        if record_key == last_training_history_key:
            return False

    record = {
        "id": len(history_records) + 1,
        "date": datetime.now().strftime("%Y-%m-%d %H:%M"),
        "duration": "00:00:00",
        "score": score,
        "reps": reps,
        "best_angle": max_angle or realtime["display_angle"],
        "action": action,
        "completion": int(realtime["completion"] or 0),
        "amplitude": int(realtime["amplitude"] or 0),
        "max_angle": max_angle,
        "avg_angle": avg_angle,
        "target_reps": target_reps,
    }
    history_records.insert(0, record)
    last_training_history_key = record_key
    save_training_history()
    return True


def maybe_add_training_history_from_payload(parsed):
    page = str(payload_page_name(parsed)).lower()
    page_event = int(parsed.get("uevt", 0) or 0)
    has_training_fields = any(
        key in parsed
        for key in ("action", "a", "reps", "n", "score", "s", "cpl", "amp", "max", "m", "avg", "av", "target", "g")
    )
    is_result_packet = page in ("result", "free_result") or page_event == 1
    if is_result_packet and has_training_fields:
        return add_history_record(parsed, "device")
    return False


def seed_history():
    if history_records:
        return
    today = datetime.now()
    demo_actions = ["肘关节屈伸", "肩部上举", "前臂旋转", "手臂外展", "肘关节屈伸", "前臂旋转", "肩部上举"]
    for i in range(7):
        day = today - timedelta(days=6 - i)
        history_records.append(
            {
                "id": i + 1,
                "date": day.strftime("%Y-%m-%d %H:%M"),
                "duration": "00:%02d:%02d" % (random.randint(10, 22), random.randint(0, 59)),
                "score": random.randint(76, 96),
                "reps": random.randint(12, 34),
                "best_angle": random.randint(95, 165),
                "action": demo_actions[i % len(demo_actions)],
                "completion": random.randint(68, 100),
                "amplitude": random.randint(70, 100),
                "max_angle": random.randint(95, 165),
                "avg_angle": random.randint(70, 135),
                "target_reps": random.choice([15, 20, 25, 30]),
            }
        )


ACTION_CONFIG = {
    "elbow": {
        "name": "肘关节弯曲",
        "aliases": ("肘关节弯曲", "肘关节屈伸", "肘关节屈曲"),
        "suggestions": ["弹力带屈肘", "毛巾抓握", "辅助屈伸训练", "桌面滑动屈肘", "等长屈肘保持"],
    },
    "abduction": {
        "name": "肩上举",
        "aliases": ("肩上举", "肩部上举"),
        "suggestions": ["墙面爬手", "肩部钟摆训练", "桌面前推训练", "肩胛稳定练习", "辅助上举保持"],
    },
    "forearm": {
        "name": "前臂转动",
        "aliases": ("前臂转动", "前臂旋转"),
        "suggestions": ["毛巾拧干", "矿泉水瓶旋转", "门把手旋转练习", "掌心翻转控制", "轻阻力旋前旋后"],
    },
    "shoulder": {
        "name": "手臂外展",
        "aliases": ("手臂外展", "上臂外展"),
        "suggestions": ["墙面爬手", "桌面滑动", "肩部外展辅助训练", "侧向滑轮外展", "等长外展保持"],
    },
}


def clamp_number(value, low=0, high=100):
    try:
        number = float(value)
    except (TypeError, ValueError):
        number = 0
    return max(low, min(high, number))


def parse_record_datetime(record):
    text = str(record.get("date", ""))
    for fmt in ("%Y-%m-%d %H:%M", "%Y-%m-%d %H:%M:%S", "%Y/%m/%d %H:%M"):
        try:
            return datetime.strptime(text, fmt)
        except ValueError:
            continue
    return datetime.min


def action_key_for(record):
    name = str(record.get("action", ""))
    for key, cfg in ACTION_CONFIG.items():
        if any(alias in name for alias in cfg["aliases"]):
            return key
    return "elbow"


def record_metric(record, key, fallback_key=None, default=0):
    value = record.get(key)
    if value in (None, "") and fallback_key:
        value = record.get(fallback_key)
    return clamp_number(value if value not in (None, "") else default)


def completion_metric(record):
    if record.get("completion") not in (None, ""):
        return record_metric(record, "completion")
    reps = record_metric(record, "reps")
    target = record_metric(record, "target_reps")
    if target > 0:
        return clamp_number(reps / target * 100)
    return clamp_number(reps * 4)


def amplitude_metric(record):
    if record.get("amplitude") not in (None, ""):
        return record_metric(record, "amplitude")
    angle = record_metric(record, "best_angle", "max_angle")
    return clamp_number(angle / 160 * 100)


def average(values):
    return sum(values) / len(values) if values else 0


def star_level(score):
    if score >= 90:
        return 5
    if score >= 80:
        return 4
    if score >= 70:
        return 3
    if score >= 60:
        return 2
    return 1


def trend_label(records):
    if len(records) < 2:
        return "数据不足"
    ordered = sorted(records, key=parse_record_datetime)
    split = max(1, len(ordered) // 2)
    early = average([record_metric(r, "score") for r in ordered[:split]])
    late = average([record_metric(r, "score") for r in ordered[split:]])
    diff = late - early
    if diff > 5:
        return "上升"
    if diff < -5:
        return "下降"
    return "稳定"


def build_training_suggestions(action_analysis):
    if not action_analysis:
        return [
            {
                "action": cfg["name"],
                "reason": "建立基础训练覆盖",
                "items": cfg["suggestions"][:3],
            }
            for cfg in ACTION_CONFIG.values()
        ]

    ranked = sorted(
        action_analysis,
        key=lambda item: (
            1 if item["problems"] == ["训练表现稳定"] else 0,
            item["avg_score"] or 0,
            item["avg_completion"] or 0,
            item["avg_amplitude"] or 0,
            item["records"],
        ),
    )
    selected = ranked[:3]
    if len(selected) < 4:
        selected_keys = {item["key"] for item in selected}
        for item in sorted(action_analysis, key=lambda x: x["records"]):
            if item["key"] not in selected_keys:
                selected.append(item)
                selected_keys.add(item["key"])
            if len(selected) >= 4:
                break

    groups = []
    for item in selected[:4]:
        cfg = ACTION_CONFIG[item["key"]]
        reason = next((p for p in item["problems"] if p != "训练表现稳定"), "")
        if not reason:
            reason = "维持动作控制和活动范围"
        items = list(cfg["suggestions"][:3])
        if item["avg_completion"] and item["avg_completion"] < 75:
            items.append("降低速度，优先保证完整动作")
        if item["avg_amplitude"] and item["avg_amplitude"] < 75:
            items.append("每次末端保持2秒，逐步扩大活动范围")
        if item["trend"] == "下降":
            items.append("减少单组次数，增加组间休息")
        groups.append({
            "action": item["action"],
            "reason": reason,
            "items": items[:5],
        })
    return groups


def recent_analysis_records():
    seed_history()
    ordered = sorted(history_records, key=parse_record_datetime, reverse=True)
    cutoff = datetime.now() - timedelta(days=7)
    recent = [r for r in ordered if parse_record_datetime(r) >= cutoff]
    if len(recent) < min(10, len(ordered)):
        recent = ordered[:10]
    return recent[:10]


def build_analysis_payload():
    records = recent_analysis_records()
    record_count = len(records)
    avg_score = average([record_metric(r, "score") for r in records])
    avg_completion = average([completion_metric(r) for r in records])
    avg_amplitude = average([amplitude_metric(r) for r in records])
    active_days = len({parse_record_datetime(r).date() for r in records if parse_record_datetime(r) != datetime.min})
    frequency_score = min(100, active_days / 7 * 100) if records else 0
    recovery_index = round(clamp_number(
        avg_score * 0.50 + avg_completion * 0.25 + avg_amplitude * 0.15 + frequency_score * 0.10
    ))

    action_analysis = []
    for key, cfg in ACTION_CONFIG.items():
        action_records = [r for r in records if action_key_for(r) == key]
        scores = [record_metric(r, "score") for r in action_records]
        completions = [completion_metric(r) for r in action_records]
        amplitudes = [amplitude_metric(r) for r in action_records]
        angles = [record_metric(r, "avg_angle", "best_angle") for r in action_records]
        avg_action_score = average(scores)
        avg_action_completion = average(completions)
        avg_action_amplitude = average(amplitudes)
        avg_action_angle = average(angles)
        trend = trend_label(action_records)
        problems = []
        if not action_records:
            problems.append("近期训练数据不足")
        if avg_action_score and avg_action_score < 80:
            problems.append("综合评分偏低")
        if avg_action_completion and avg_action_completion < 70:
            problems.append("动作完成度不足")
        if avg_action_amplitude and avg_action_amplitude < 75:
            problems.append("动作幅度不足")
        if trend == "下降":
            problems.append("近期表现下降")
        if not problems:
            problems.append("训练表现稳定")
        advice = "建议继续保持当前训练节奏。"
        if problems != ["训练表现稳定"]:
            advice = "训练建议：优先提高动作完成度和动作幅度，单组训练中保持慢速、稳定、可控。"
        action_analysis.append({
            "key": key,
            "action": cfg["name"],
            "records": len(action_records),
            "avg_score": round(avg_action_score),
            "avg_completion": round(avg_action_completion),
            "avg_amplitude": round(avg_action_amplitude),
            "avg_angle": round(avg_action_angle),
            "trend": trend,
            "stars": star_level(avg_action_score),
            "problems": problems,
            "advice": advice,
        })

    weak = min(
        action_analysis,
        key=lambda item: (item["avg_score"] or 0, item["avg_completion"] or 0, item["records"]),
    ) if action_analysis else None
    weak_key = weak["key"] if weak else "elbow"
    weak_action = weak["action"] if weak else ACTION_CONFIG["elbow"]["name"]
    main_problem = "训练表现稳定"
    if weak:
        main_problem = next((p for p in weak["problems"] if p != "训练表现稳定"), weak["problems"][0])

    return {
        "range": "最近7天或最近10条训练记录",
        "record_count": record_count,
        "recovery_index": recovery_index,
        "weak_action": weak_action,
        "main_problem": main_problem,
        "action_analysis": action_analysis,
        "suggestions": build_training_suggestions(action_analysis),
        "generated_at": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
    }


def build_ai_context():
    analysis = build_analysis_payload()
    recent = []
    for item in recent_analysis_records()[:6]:
        recent.append({
            "date": item.get("date"),
            "action": item.get("action"),
            "score": item.get("score"),
            "reps": item.get("reps"),
            "completion": item.get("completion"),
            "amplitude": item.get("amplitude"),
            "best_angle": item.get("best_angle"),
            "duration": item.get("duration"),
        })
    realtime = normalize_realtime(latest_data.get("parsed", {}))
    return {
        "analysis": {
            "recovery_index": analysis.get("recovery_index"),
            "weak_action": analysis.get("weak_action"),
            "main_problem": analysis.get("main_problem"),
            "action_analysis": analysis.get("action_analysis"),
            "suggestions": analysis.get("suggestions"),
        },
        "recent_records": recent,
        "latest_realtime": {
            "action": realtime.get("action"),
            "display_angle": realtime.get("display_angle"),
            "reps": realtime.get("reps"),
            "score": realtime.get("score"),
            "completion": realtime.get("completion"),
            "amplitude": realtime.get("amplitude"),
        },
    }


def call_dashscope_chat(messages):
    api_key = os.environ.get("DASHSCOPE_API_KEY", "").strip()
    base_url = os.environ.get("DASHSCOPE_BASE_URL", "https://dashscope.aliyuncs.com/compatible-mode/v1").rstrip("/")
    model = os.environ.get("DASHSCOPE_MODEL", "qwen3.7-plus").strip()
    if not api_key:
        raise RuntimeError("DASHSCOPE_API_KEY 未配置")

    payload = {
        "model": model,
        "messages": messages,
        "temperature": 0.35,
        "max_tokens": int(os.environ.get("DASHSCOPE_MAX_TOKENS", "360") or 360),
    }
    req = urllib.request.Request(
        f"{base_url}/chat/completions",
        data=json.dumps(payload, ensure_ascii=False).encode("utf-8"),
        headers={
            "Authorization": f"Bearer {api_key}",
            "Content-Type": "application/json",
        },
        method="POST",
    )
    try:
        timeout = float(os.environ.get("DASHSCOPE_TIMEOUT", "35") or 35)
        # The desktop environment may define a dead HTTP(S) proxy (for example
        # 127.0.0.1:9).  DashScope is reachable directly, so do not inherit
        # proxy environment variables for the model request.
        direct_opener = urllib.request.build_opener(urllib.request.ProxyHandler({}))
        with direct_opener.open(req, timeout=timeout) as resp:
            data = json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as exc:
        detail = exc.read().decode("utf-8", errors="ignore")
        raise RuntimeError(f"模型接口 HTTP {exc.code}: {detail[:240]}") from exc
    except urllib.error.URLError as exc:
        raise RuntimeError(f"模型接口连接失败: {exc.reason}") from exc

    choices = data.get("choices") or []
    if not choices:
        raise RuntimeError("模型接口未返回有效内容")
    return choices[0].get("message", {}).get("content", "").strip()


def ingest_device_payload(data, raw=None, source="http"):
    if isinstance(data, dict):
        raw = raw if raw is not None else data
        data = data.get("data", data)

    if data in (None, ""):
        data = "空"

    parsed = parse_kv_payload(data)
    page = payload_page_name(parsed)
    is_health_data_packet = (
        str(page).lower() == "health" and
        any(key in parsed for key in ("hv", "hr", "hseq", "spo2", "sdnn", "rmssd"))
    )
    if is_health_data_packet:
        latest_health["parsed"] = parsed
        latest_health["time"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        latest_data["time"] = latest_health["time"]
        latest_data["count"] += 1
        add_health_history_record(parsed, latest_health["time"])
        notify_realtime_update()
        return "health"

    latest_data["data"] = data
    latest_data["raw"] = raw if raw is not None else {}
    latest_data["parsed"] = parsed
    latest_data["time"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    latest_data["count"] += 1

    if isinstance(page, str) and page:
        system_state["last_page"] = page
    if str(page).lower() in ("training", "realtime", "train"):
        system_state["is_training"] = True
    elif str(page).lower() in ("result", "free_result", "home"):
        system_state["is_training"] = False

    maybe_add_training_history_from_payload(parsed)
    print(f"收到{source}上传数据：{latest_data}")
    notify_realtime_update()
    return "realtime"


@app.route("/upload", methods=["POST"])
def upload_data():
    payload = request.get_json(silent=True)

    if isinstance(payload, dict):
        data = payload.get("data", payload)
        raw = payload
    else:
        data = request.form.get("data")
        if data is None:
            data = request.get_data(as_text=True).strip()
        raw = dict(request.form) if request.form else {"body": data}

    ingest_device_payload(data, raw, "HTTP")
    now = datetime.now()
    return (
        f"ok,epoch={int(time.time())},hh={now.hour},mm={now.minute},ss={now.second}\n",
        200,
    )


@app.route("/api/latest")
def api_latest():
    return jsonify(build_latest_payload())


@app.route("/api/device-time")
def api_device_time():
    return Response(f"epoch={int(time.time())}\n", mimetype="text/plain")


@app.route("/events")
def realtime_events():
    def stream():
        last_seen = -1
        while True:
            with state_condition:
                state_condition.wait_for(lambda: state_version != last_seen, timeout=3)
                last_seen = state_version
            payload = json.dumps(build_latest_payload(), ensure_ascii=False)
            yield f"event: realtime_data\ndata: {payload}\n\n"
            time.sleep(0.01)

    return Response(stream(), mimetype="text/event-stream")


@app.route("/api/start", methods=["POST"])
def api_start():
    system_state["is_training"] = True
    system_state["training_started_at"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    system_state["last_page"] = "realtime"
    notify_realtime_update()
    return jsonify({"ok": True, "system_state": system_state})


@app.route("/api/stop", methods=["POST"])
def api_stop():
    if system_state["is_training"]:
        add_history_record()
    system_state["is_training"] = False
    system_state["last_page"] = "result"
    notify_realtime_update()
    return jsonify({"ok": True, "system_state": system_state})


@app.route("/api/history")
def api_history():
    seed_history()
    return jsonify({"records": history_records})


@app.route("/api/health_history")
def api_health_history():
    return jsonify({"records": health_history_records[-500:]})


@app.route("/api/analysis")
def api_analysis():
    return jsonify(build_analysis_payload())


@app.route("/api/ai_chat", methods=["POST"])
def api_ai_chat():
    payload = request.get_json(silent=True) or {}
    user_message = str(payload.get("message", "")).strip()
    client_messages = payload.get("messages", [])
    if not user_message and isinstance(client_messages, list):
        user_message = str((client_messages[-1] or {}).get("content", "")).strip() if client_messages else ""
    if not user_message:
        return jsonify({"ok": False, "error": "请输入要咨询的问题"}), 400

    context = build_ai_context()
    system_prompt = (
        "你是智能康复训练辅助助手，服务对象是上肢功能训练和日常康复人群。"
        "请先直接回答用户问题的核心结论，第一句不要以免责声明开头。"
        "如果用户询问某个动作是否对某类人群有帮助，先明确回答通常有帮助、可能有帮助或目前不能判断，"
        "再解释它可能改善的功能、适用前提和不适用情况。"
        "回答应优先结合当前训练数据中的动作完成度、活动角度、肌电状态、训练次数和评分；"
        "没有相关数据时要明确说明‘当前数据不足’，不要编造数值。"
        "给出可执行方案时尽量包含：动作要领、每组次数或持续时间、组间休息、进阶方式，以及如何判断效果。"
        "对脑卒中等疾病，只能提供一般训练信息，不能替代医生或治疗师诊断；"
        "出现疼痛加重、明显疲劳、头晕、麻木或动作异常时，应立即停止并咨询专业人员。"
        "中文回答，结构清晰，可使用‘结论/为什么/怎么做/注意事项’小标题，控制在6条以内；"
        "避免重复长篇免责声明，安全提示放在最后。"
    )
    messages = [
        {"role": "system", "content": system_prompt},
        {
            "role": "user",
            "content": "以下是康复训练系统当前数据，请作为上下文：\n"
                       + json.dumps(context, ensure_ascii=False, default=str),
        },
    ]
    if isinstance(client_messages, list):
        for item in client_messages[-2:]:
            role = item.get("role")
            content = str(item.get("content", "")).strip()
            if role in ("user", "assistant") and content:
                messages.append({"role": role, "content": content[:420]})
    if not messages or messages[-1].get("content") != user_message:
        messages.append({"role": "user", "content": user_message[:420]})

    try:
        answer = call_dashscope_chat(messages)
        return jsonify({"ok": True, "answer": answer, "model": os.environ.get("DASHSCOPE_MODEL", "qwen3.7-plus")})
    except RuntimeError as exc:
        return jsonify({"ok": False, "error": str(exc)}), 502
    except Exception as exc:
        return jsonify({"ok": False, "error": f"模型调用异常: {exc}"}), 502


def start_mqtt_subscriber():
    if not MQTT_ENABLED:
        print("MQTT订阅已关闭：MQTT_ENABLED=0")
        return None
    if mqtt is None:
        print("MQTT订阅未启动：缺少 paho-mqtt，请先安装 paho-mqtt")
        return None

    if hasattr(mqtt, "CallbackAPIVersion"):
        client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=MQTT_CLIENT_ID, clean_session=True)
    else:
        client = mqtt.Client(client_id=MQTT_CLIENT_ID, clean_session=True)

    def on_connect(client, userdata, flags, reason_code, properties=None):
        try:
            rc = int(reason_code)
        except (TypeError, ValueError):
            rc = 0 if str(reason_code).lower() == "success" else 1
        if rc == 0:
            client.subscribe(MQTT_TOPIC, qos=0)
            print(f"MQTT已连接：{MQTT_HOST}:{MQTT_PORT}，订阅 {MQTT_TOPIC}")
        else:
            print(f"MQTT连接失败，rc={rc}")

    def on_message(client, userdata, msg):
        try:
            text = msg.payload.decode("utf-8", errors="ignore").strip()
            raw = {"mqtt_topic": msg.topic, "payload": text}
            if text.startswith("{"):
                try:
                    obj = json.loads(text)
                    data = obj.get("data", obj)
                    raw["json"] = obj
                except json.JSONDecodeError:
                    data = text
            else:
                data = text
            ingest_device_payload(data, raw, f"MQTT:{msg.topic}")
        except Exception as exc:
            print(f"MQTT消息处理失败：{exc}")

    def on_disconnect(client, userdata, *args):
        reason_code = args[-2] if len(args) >= 2 else (args[0] if args else 0)
        try:
            rc = int(reason_code)
        except (TypeError, ValueError):
            rc = 0 if str(reason_code).lower() == "normal disconnection" else 1
        if rc != 0:
            print(f"MQTT异常断开，rc={rc}，客户端会自动重连")

    client.on_connect = on_connect
    client.on_message = on_message
    client.on_disconnect = on_disconnect
    client.reconnect_delay_set(min_delay=1, max_delay=30)
    try:
        client.connect_async(MQTT_HOST, MQTT_PORT, keepalive=60)
        client.loop_start()
    except Exception as exc:
        print(f"MQTT订阅启动失败：{exc}")
        return None
    return client


@app.route("/")
def index():
    dashboard_path = os.path.join(os.path.dirname(__file__), "dashboard.html")
    if os.path.exists(dashboard_path):
        with open(dashboard_path, "r", encoding="utf-8") as fp:
            return render_template_string(fp.read())

    html_template = r"""
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>STM32智能康复臂带仪表盘</title>
    <script src="https://cdn.jsdelivr.net/npm/echarts@5/dist/echarts.min.js"></script>
    <style>
        :root {
            --bg: #0B1220;
            --panel: rgba(16, 30, 54, 0.66);
            --panel-strong: rgba(18, 42, 77, 0.78);
            --line: rgba(91, 182, 255, 0.22);
            --blue: #2F80FF;
            --blue-soft: #5BB6FF;
            --text: #FFFFFF;
            --muted: #C9D4E3;
            --dim: #7F8FA6;
            --green: #35E6A8;
            --warn: #FFD166;
            --danger: #FF5C7A;
        }

        * { box-sizing: border-box; }
        body {
            margin: 0;
            min-height: 100vh;
            font-family: Inter, "Microsoft YaHei", "PingFang SC", Arial, sans-serif;
            color: var(--text);
            background:
                radial-gradient(circle at 18% 10%, rgba(47, 128, 255, 0.22), transparent 30%),
                radial-gradient(circle at 88% 18%, rgba(91, 182, 255, 0.16), transparent 34%),
                linear-gradient(135deg, #07101D 0%, var(--bg) 48%, #071827 100%);
            overflow-x: hidden;
        }

        body::before {
            content: "";
            position: fixed;
            inset: 0;
            pointer-events: none;
            background-image:
                linear-gradient(rgba(91,182,255,0.035) 1px, transparent 1px),
                linear-gradient(90deg, rgba(91,182,255,0.035) 1px, transparent 1px);
            background-size: 38px 38px;
            mask-image: linear-gradient(to bottom, rgba(0,0,0,0.75), transparent 78%);
        }

        .app-shell {
            width: min(1440px, calc(100vw - 48px));
            margin: 0 auto;
            padding: 28px 0 36px;
        }

        .topbar {
            display: flex;
            align-items: center;
            justify-content: space-between;
            gap: 20px;
            margin-bottom: 24px;
        }

        .brand {
            display: flex;
            align-items: center;
            gap: 14px;
        }

        .brand-mark {
            width: 46px;
            height: 46px;
            border-radius: 14px;
            display: grid;
            place-items: center;
            background: linear-gradient(135deg, var(--blue), var(--blue-soft));
            box-shadow: 0 0 34px rgba(47,128,255,0.5);
            font-size: 24px;
            font-weight: 800;
        }

        .brand h1 {
            margin: 0;
            font-size: 24px;
            letter-spacing: 0;
        }

        .brand p {
            margin: 4px 0 0;
            color: var(--muted);
            font-size: 13px;
        }

        .status-strip {
            display: flex;
            align-items: center;
            gap: 12px;
            flex-wrap: wrap;
        }

        .pill {
            display: inline-flex;
            align-items: center;
            gap: 8px;
            min-height: 36px;
            padding: 0 14px;
            border: 1px solid var(--line);
            border-radius: 999px;
            background: rgba(255,255,255,0.06);
            color: var(--muted);
            backdrop-filter: blur(16px);
            box-shadow: inset 0 1px 0 rgba(255,255,255,0.08);
            font-size: 13px;
        }

        .dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background: var(--dim);
            box-shadow: 0 0 12px currentColor;
        }
        .dot.on { background: var(--green); color: var(--green); }
        .dot.train { background: var(--blue-soft); color: var(--blue-soft); }

        .page {
            display: none;
            opacity: 0;
            transform: translateY(10px);
            transition: opacity 300ms ease, transform 300ms ease;
        }
        .page.active {
            display: block;
            opacity: 1;
            transform: translateY(0);
        }

        .glass {
            border: 1px solid var(--line);
            border-radius: 22px;
            background: linear-gradient(145deg, rgba(18, 38, 68, 0.76), rgba(12, 22, 39, 0.58));
            backdrop-filter: blur(22px);
            box-shadow: 0 18px 60px rgba(0,0,0,0.28), 0 0 36px rgba(47,128,255,0.12), inset 0 1px 0 rgba(255,255,255,0.08);
            transition: transform 300ms ease, box-shadow 300ms ease, border-color 300ms ease;
        }
        .glass:hover {
            transform: translateY(-2px);
            border-color: rgba(91,182,255,0.42);
            box-shadow: 0 22px 70px rgba(0,0,0,0.34), 0 0 48px rgba(47,128,255,0.2), inset 0 1px 0 rgba(255,255,255,0.12);
        }

        .hero {
            display: grid;
            grid-template-columns: 1.15fr 0.85fr;
            gap: 22px;
            align-items: stretch;
        }

        .hero-main {
            padding: 34px;
            min-height: 420px;
            position: relative;
            overflow: hidden;
        }
        .hero-main::after {
            content: "";
            position: absolute;
            right: -80px;
            bottom: -110px;
            width: 360px;
            height: 360px;
            border-radius: 50%;
            border: 1px solid rgba(91,182,255,0.18);
            box-shadow: inset 0 0 80px rgba(47,128,255,0.16), 0 0 60px rgba(47,128,255,0.12);
        }

        .eyebrow {
            margin: 0 0 12px;
            color: var(--blue-soft);
            font-weight: 700;
            font-size: 13px;
            letter-spacing: 0.08em;
            text-transform: uppercase;
        }

        .hero-title {
            margin: 0;
            max-width: 760px;
            font-size: clamp(34px, 4.2vw, 62px);
            line-height: 1.06;
            letter-spacing: 0;
        }

        .hero-copy {
            margin: 18px 0 0;
            max-width: 680px;
            color: var(--muted);
            font-size: 17px;
            line-height: 1.75;
        }

        .actions {
            display: flex;
            flex-wrap: wrap;
            gap: 14px;
            margin-top: 34px;
        }

        button {
            border: 0;
            color: var(--text);
            cursor: pointer;
            font: inherit;
            transition: transform 220ms ease, box-shadow 220ms ease, border-color 220ms ease, background 220ms ease;
        }
        button:active { transform: scale(0.98); }

        .btn {
            min-height: 48px;
            padding: 0 20px;
            border-radius: 14px;
            border: 1px solid rgba(91,182,255,0.22);
            background: rgba(255,255,255,0.07);
            color: var(--muted);
        }
        .btn:hover {
            color: var(--text);
            border-color: rgba(91,182,255,0.48);
            box-shadow: 0 0 26px rgba(47,128,255,0.18);
        }
        .btn.primary {
            background: linear-gradient(135deg, var(--blue), var(--blue-soft));
            color: #fff;
            font-weight: 800;
            box-shadow: 0 16px 34px rgba(47,128,255,0.38);
        }

        .home-side {
            display: grid;
            gap: 16px;
        }

        .metric-card {
            padding: 24px;
            min-height: 126px;
        }
        .label {
            color: var(--muted);
            font-size: 13px;
        }
        .value {
            margin-top: 10px;
            color: var(--blue-soft);
            font-size: 42px;
            line-height: 1;
            font-weight: 850;
            text-shadow: 0 0 28px rgba(47,128,255,0.45);
        }
        .unit {
            margin-left: 6px;
            color: var(--muted);
            font-size: 16px;
            font-weight: 600;
        }

        .grid {
            display: grid;
            gap: 18px;
        }
        .realtime-grid {
            grid-template-columns: 1.2fr 0.8fr;
        }
        .stats-grid {
            grid-template-columns: repeat(4, minmax(0, 1fr));
        }
        .panel {
            padding: 22px;
        }
        .panel-title {
            display: flex;
            justify-content: space-between;
            align-items: center;
            gap: 12px;
            margin-bottom: 16px;
        }
        .panel-title h2, .panel-title h3 {
            margin: 0;
            font-size: 18px;
            letter-spacing: 0;
        }
        .chart {
            width: 100%;
            height: 320px;
        }

        .angle-wrap {
            display: grid;
            grid-template-columns: 180px 1fr;
            gap: 20px;
            align-items: center;
        }
        .angle-ring {
            width: 180px;
            height: 180px;
            border-radius: 50%;
            display: grid;
            place-items: center;
            background: conic-gradient(var(--blue) var(--angle-deg, 0deg), rgba(255,255,255,0.08) 0deg);
            box-shadow: 0 0 36px rgba(47,128,255,0.2);
            position: relative;
        }
        .angle-ring::before {
            content: "";
            position: absolute;
            inset: 14px;
            border-radius: 50%;
            background: #0D1728;
            border: 1px solid rgba(91,182,255,0.2);
        }
        .angle-ring span {
            position: relative;
            color: var(--blue-soft);
            font-size: 34px;
            font-weight: 850;
        }

        .imu-table {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
            margin-top: 14px;
        }
        .imu-cell {
            padding: 14px;
            border: 1px solid rgba(91,182,255,0.16);
            border-radius: 16px;
            background: rgba(255,255,255,0.045);
        }
        .imu-cell strong {
            display: block;
            margin-top: 6px;
            color: #fff;
            font-size: 20px;
        }

        .progress-track {
            height: 12px;
            border-radius: 999px;
            background: rgba(255,255,255,0.08);
            overflow: hidden;
            margin-top: 12px;
        }
        .progress-fill {
            height: 100%;
            width: 0%;
            border-radius: inherit;
            background: linear-gradient(90deg, var(--blue), var(--blue-soft));
            box-shadow: 0 0 22px rgba(47,128,255,0.55);
            transition: width 300ms ease;
        }

        .history-layout {
            display: grid;
            grid-template-columns: 0.95fr 1.05fr;
            gap: 18px;
        }
        .record-list {
            display: grid;
            gap: 12px;
            max-height: 540px;
            overflow: auto;
            padding-right: 4px;
        }
        .record {
            display: grid;
            grid-template-columns: 1fr auto;
            gap: 10px;
            align-items: center;
            padding: 16px;
            border: 1px solid rgba(91,182,255,0.16);
            border-radius: 18px;
            background: rgba(255,255,255,0.045);
            cursor: pointer;
        }
        .record.active {
            border-color: rgba(91,182,255,0.56);
            background: rgba(47,128,255,0.14);
        }
        .record h3 {
            margin: 0 0 6px;
            font-size: 16px;
        }
        .record p {
            margin: 0;
            color: var(--muted);
            font-size: 13px;
        }
        .score {
            color: var(--blue-soft);
            font-size: 28px;
            font-weight: 850;
        }

        .detail-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 12px;
            margin-top: 18px;
        }

        @media (max-width: 1080px) {
            .hero, .realtime-grid, .history-layout { grid-template-columns: 1fr; }
            .stats-grid { grid-template-columns: repeat(2, 1fr); }
        }
        @media (max-width: 680px) {
            .app-shell { width: min(100vw - 24px, 1440px); padding-top: 18px; }
            .topbar { align-items: flex-start; flex-direction: column; }
            .hero-main { padding: 24px; min-height: 360px; }
            .stats-grid, .detail-grid, .angle-wrap { grid-template-columns: 1fr; }
            .angle-ring { margin: 0 auto; }
            .chart { height: 260px; }
        }
    </style>
</head>
<body>
    <main class="app-shell">
        <header class="topbar">
            <div class="brand">
                <div class="brand-mark">R</div>
                <div>
                    <h1>STM32智能康复臂带</h1>
                    <p>上肢康复训练实时监测与历史分析系统</p>
                </div>
            </div>
            <div class="status-strip">
                <span class="pill"><i id="btDot" class="dot"></i><span id="btText">蓝牙未连接</span></span>
                <span class="pill"><i id="trainDot" class="dot"></i><span id="trainText">待机</span></span>
                <span class="pill">电量 <strong id="topBattery">--%</strong></span>
            </div>
        </header>

        <section id="homePage" class="page active">
            <div class="hero">
                <div class="glass hero-main">
                    <p class="eyebrow">Medical Rehabilitation Dashboard</p>
                    <h2 class="hero-title">高精度上肢康复训练可视化中心</h2>
                    <p class="hero-copy">实时展示动作角度、动作识别和训练评分，为竞赛演示提供稳定、清晰、专业的医疗仪器级界面。</p>
                    <div class="actions">
                        <button class="btn primary" onclick="startTraining()">开始训练</button>
                        <button class="btn" onclick="showPage('history')">历史记录</button>
                        <button class="btn" onclick="showPage('history')">AI分析</button>
                    </div>
                </div>
                <div class="home-side">
                    <div class="glass metric-card">
                        <div class="label">上臂端状态</div>
                        <div class="value" id="upperStatus">--</div>
                        <div class="label" id="upperMeta" style="margin-top:10px;">电量 -- · 未息屏</div>
                    </div>
                    <div class="glass metric-card">
                        <div class="label">手腕端状态</div>
                        <div class="value" id="wristStatus">--</div>
                        <div class="label" id="wristMeta" style="margin-top:10px;">电量 -- · 未息屏</div>
                    </div>
                    <div class="glass metric-card">
                        <div class="label">上传次数</div>
                        <div class="value"><span id="homeCount">0</span><span class="unit">次</span></div>
                    </div>
                </div>
            </div>
        </section>

        <section id="realtimePage" class="page">
            <div class="grid stats-grid" style="margin-bottom:18px;">
                <div class="glass metric-card"><div class="label">肘关节角度</div><div class="value"><span id="rtAngle">0</span><span class="unit">deg</span></div></div>
                <div class="glass metric-card"><div class="label">动作识别</div><div class="value" id="rtAction">待识别</div></div>
                <div class="glass metric-card"><div class="label">完成度</div><div class="value"><span id="rtCompletion">0</span><span class="unit">%</span></div></div>
                <div class="glass metric-card"><div class="label">幅度</div><div class="value"><span id="rtAmplitude">0</span><span class="unit">%</span></div></div>
                <div class="glass metric-card"><div class="label">训练评分</div><div class="value"><span id="rtScore">0</span><span class="unit">分</span></div></div>
            </div>

            <div class="grid realtime-grid">
                <div class="glass panel">
                    <div class="panel-title">
                        <h2>实时动作曲线</h2>
                        <button class="btn" onclick="stopTraining()">停止训练</button>
                    </div>
                    <div id="realtimeChart" class="chart"></div>
                </div>
                <div class="glass panel">
                    <div class="panel-title"><h2>肘关节活动范围</h2><span class="pill">0-180 deg</span></div>
                    <div class="angle-wrap">
                        <div id="angleRing" class="angle-ring"><span id="ringAngle">0 deg</span></div>
                        <div>
                            <div class="label">目标完成进度</div>
                            <div class="progress-track"><div id="progressFill" class="progress-fill"></div></div>
                        </div>
                    </div>
                </div>
            </div>
        </section>

        <section id="resultPage" class="page">
            <div class="hero">
                <div class="glass hero-main">
                    <p class="eyebrow">Training Summary</p>
                    <h2 class="hero-title">训练结算</h2>
                    <p class="hero-copy">本次训练已结束，系统汇总动作完成情况、幅度表现与综合评分。</p>
                    <div class="actions">
                        <button class="btn primary" onclick="showPage('home')">返回主页</button>
                        <button class="btn" onclick="startTraining()">再次训练</button>
                        <button class="btn" onclick="showPage('history')">历史记录</button>
                    </div>
                </div>
                <div class="home-side">
                    <div class="glass metric-card">
                        <div class="label">综合评分</div>
                        <div class="value"><span id="resultScore">0</span><span class="unit">分</span></div>
                    </div>
                    <div class="glass metric-card">
                        <div class="label">完成次数</div>
                        <div class="value"><span id="resultReps">0/0</span><span class="unit">次</span></div>
                    </div>
                    <div class="glass metric-card">
                        <div class="label">训练动作</div>
                        <div class="value" id="resultAction">--</div>
                    </div>
                </div>
            </div>
            <div class="grid stats-grid" style="margin-top:18px;">
                <div class="glass metric-card"><div class="label">完成度</div><div class="value"><span id="resultCompletion">0</span><span class="unit">%</span></div></div>
                <div class="glass metric-card"><div class="label">幅度</div><div class="value"><span id="resultAmplitude">0</span><span class="unit">%</span></div></div>
                <div class="glass metric-card"><div class="label">最大角度</div><div class="value"><span id="resultMaxAngle">0</span><span class="unit">deg</span></div></div>
                <div class="glass metric-card"><div class="label">平均峰值角度</div><div class="value"><span id="resultAvgAngle">0</span><span class="unit">deg</span></div></div>
            </div>
        </section>

        <section id="historyPage" class="page">
            <div class="history-layout">
                <div class="glass panel">
                    <div class="panel-title">
                        <h2>训练记录</h2>
                        <button class="btn" onclick="showPage(systemState.is_training ? 'realtime' : 'home')">返回</button>
                    </div>
                    <div id="recordList" class="record-list"></div>
                </div>
                <div class="glass panel">
                    <div class="panel-title"><h2>7天评分趋势</h2><span class="pill">History Analysis</span></div>
                    <div id="historyChart" class="chart"></div>
                    <div class="detail-grid">
                        <div class="imu-cell">训练动作<strong id="detailAction">--</strong></div>
                        <div class="imu-cell">完成次数<strong id="detailReps">--</strong></div>
                        <div class="imu-cell">最佳角度<strong id="detailAngle">--</strong></div>
                    </div>
                </div>
            </div>
        </section>
    </main>

    <script>
        const state = {
            page: 'home',
            points: [],
            history: [],
            selectedRecord: null,
        };
        let systemState = { is_training: false };
        let realtimeChart;
        let historyChart;

        function makeLineChart(el, names) {
            const chart = echarts.init(el);
            chart.setOption({
                backgroundColor: 'transparent',
                animationDurationUpdate: 280,
                grid: { left: 42, right: 20, top: 28, bottom: 32 },
                tooltip: { trigger: 'axis', backgroundColor: 'rgba(11,18,32,0.92)', borderColor: 'rgba(91,182,255,0.35)', textStyle: { color: '#fff' } },
                xAxis: { type: 'category', boundaryGap: false, axisLine: { lineStyle: { color: 'rgba(201,212,227,0.25)' } }, axisLabel: { color: '#7F8FA6' }, data: [] },
                yAxis: { type: 'value', splitLine: { lineStyle: { color: 'rgba(201,212,227,0.08)' } }, axisLabel: { color: '#7F8FA6' } },
                series: names.map((name, i) => ({
                    name,
                    type: 'line',
                    smooth: true,
                    showSymbol: false,
                    lineStyle: { width: i === 0 ? 3 : 2, color: i === 0 ? '#2F80FF' : '#5BB6FF' },
                    areaStyle: i === 0 ? {
                        color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
                            { offset: 0, color: 'rgba(47,128,255,0.32)' },
                            { offset: 1, color: 'rgba(47,128,255,0.02)' },
                        ])
                    } : undefined,
                    data: [],
                })),
            });
            return chart;
        }

        function initCharts() {
            realtimeChart = makeLineChart(document.getElementById('realtimeChart'), ['肘关节角度', '陀螺仪强度']);
            historyChart = makeLineChart(document.getElementById('historyChart'), ['评分']);
            window.addEventListener('resize', () => {
                realtimeChart.resize();
                historyChart.resize();
            });
        }

        function showPage(page) {
            state.page = page;
            document.querySelectorAll('.page').forEach(el => el.classList.remove('active'));
            document.getElementById(page + 'Page').classList.add('active');
            setTimeout(() => {
                realtimeChart && realtimeChart.resize();
                historyChart && historyChart.resize();
            }, 50);
            if (page === 'history') loadHistory();
        }

        async function startTraining() {
            await fetch('/api/start', { method: 'POST' });
            systemState.is_training = true;
            showPage('realtime');
            updateStatus();
        }

        async function stopTraining() {
            await fetch('/api/stop', { method: 'POST' });
            systemState.is_training = false;
            showPage('result');
            updateStatus();
        }

        function updateStatus(data) {
            const realtime = data?.realtime || {};
            const devices = data?.devices || {};
            const upper = devices.upper || {};
            const btOn = !!realtime.bluetooth_connected;
            document.getElementById('btDot').className = 'dot' + (btOn ? ' on' : '');
            document.getElementById('btText').textContent = btOn ? '蓝牙已连接' : '蓝牙未连接';
            document.getElementById('trainDot').className = 'dot' + (systemState.is_training ? ' train' : '');
            document.getElementById('trainText').textContent = systemState.is_training ? '训练中' : '待机';
            document.getElementById('topBattery').textContent = ((upper.battery_valid ? upper.battery : realtime.battery) ?? '--') + '%';
        }

        function renderDeviceStatus(obj) {
            const devices = obj.devices || {};
            const upper = devices.upper || {};
            const wrist = devices.wrist || {};
            const upperBattery = upper.battery_valid ? `${upper.battery}%` : '--';
            const wristBattery = wrist.battery_valid ? `${wrist.battery}%` : '--';
            setText('upperStatus', upper.bluetooth_connected ? '已连接' : '未连接');
            setText('wristStatus', wrist.bluetooth_connected ? '已连接' : '未连接');
            setText('upperMeta', `电量 ${upperBattery} · ${upper.sleeping ? '息屏中' : '未息屏'}`);
            setText('wristMeta', `电量 ${wristBattery} · ${wrist.sleeping ? '息屏中' : '未息屏'}`);
        }

        function renderResult(rt) {
            setText('resultScore', rt.score ?? 0);
            setText('resultReps', `${rt.reps ?? 0}/${rt.target_reps ?? 0}`);
            setText('resultAction', rt.action || '--');
            setText('resultCompletion', rt.completion ?? 0);
            setText('resultAmplitude', rt.amplitude ?? 0);
            setText('resultMaxAngle', rt.max_angle ?? 0);
            setText('resultAvgAngle', rt.avg_angle ?? 0);
        }

        function setText(id, value) {
            document.getElementById(id).textContent = value;
        }

        function applyRealtime(obj) {
            systemState = obj.system_state || systemState;
            const rt = obj.realtime || {};
            updateStatus(obj);
            renderDeviceStatus(obj);
            renderResult(rt);

            const displayAngle = rt.display_angle ?? rt.elbow_angle ?? 0;
            setText('homeCount', obj.count ?? 0);
            setText('rtAngle', displayAngle);
            setText('ringAngle', displayAngle + ' deg');
            setText('rtAction', rt.action || '待识别');
            setText('rtCompletion', rt.completion ?? 0);
            setText('rtAmplitude', rt.amplitude ?? 0);
            setText('rtScore', rt.score ?? 0);
            const angleDeg = Math.max(0, Math.min(180, displayAngle || 0)) * 2;
            document.getElementById('angleRing').style.setProperty('--angle-deg', angleDeg + 'deg');
            document.getElementById('progressFill').style.width = Math.max(0, Math.min(100, rt.completion || 0)) + '%';

            const label = obj.time ? obj.time.split(' ').pop() : new Date().toLocaleTimeString();
            state.points.push({ label, angle: displayAngle || 0, gyro: rt.gyro || 0 });
            if (state.points.length > 50) state.points.shift();
            realtimeChart.setOption({
                xAxis: { data: state.points.map(p => p.label) },
                series: [
                    { data: state.points.map(p => p.angle) },
                    { data: state.points.map(p => p.gyro) },
                ],
            });

            const pageName = String(obj.parsed?.page || obj.system_state?.last_page || '').toLowerCase();
            if (pageName === 'result' || pageName === 'free_result') {
                showPage('result');
            } else if (pageName === 'training' || pageName === 'realtime' || pageName === 'train') {
                if (state.page === 'home' || state.page === 'result') showPage('realtime');
            } else if (pageName === 'home' && state.page === 'realtime') {
                showPage('home');
            }
        }

        async function pollLatest() {
            try {
                const resp = await fetch('/api/latest', { cache: 'no-store' });
                applyRealtime(await resp.json());
            } catch (err) {
                console.warn(err);
            }
        }

        async function loadHistory() {
            const resp = await fetch('/api/history', { cache: 'no-store' });
            const obj = await resp.json();
            state.history = obj.records || [];
            renderHistory();
        }

        function renderHistory() {
            const list = document.getElementById('recordList');
            list.innerHTML = '';
            state.history.forEach((record, index) => {
                const item = document.createElement('div');
                item.className = 'record' + (index === 0 ? ' active' : '');
                item.innerHTML = `<div><h3>${record.action}</h3><p>${record.date} · ${record.duration}</p></div><div class="score">${record.score}</div>`;
                item.onclick = () => selectRecord(record, item);
                list.appendChild(item);
                if (index === 0) selectRecord(record, item, false);
            });
            const labels = state.history.slice().reverse().map(r => r.date.slice(5, 10));
            const scores = state.history.slice().reverse().map(r => r.score);
            historyChart.setOption({
                xAxis: { data: labels },
                series: [{ data: scores }],
            });
        }

        function selectRecord(record, el, mark = true) {
            state.selectedRecord = record;
            if (mark) {
                document.querySelectorAll('.record').forEach(x => x.classList.remove('active'));
                el.classList.add('active');
            }
            setText('detailAction', record.action);
            setText('detailReps', record.reps + ' 次');
            setText('detailAngle', record.best_angle + ' deg');
        }

        function connectRealtimeStream() {
            if (!window.EventSource) {
                setInterval(pollLatest, 1000);
                return;
            }
            const events = new EventSource('/events');
            events.addEventListener('realtime_data', (event) => {
                try {
                    applyRealtime(JSON.parse(event.data));
                } catch (err) {
                    console.warn(err);
                }
            });
            events.onerror = () => {
                events.close();
                setInterval(pollLatest, 1000);
            };
        }

        initCharts();
        pollLatest();
        try { connectRealtimeStream(); } catch (err) { setInterval(pollLatest, 1000); }
    </script>
</body>
</html>
    """
    return render_template_string(html_template)


load_training_history()
load_health_history()
mqtt_client = start_mqtt_subscriber()


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=False, use_reloader=False)

