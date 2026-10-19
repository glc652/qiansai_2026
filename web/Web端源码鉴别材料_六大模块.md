# Web端源码鉴别材料（六大模块）

- 软件名称：智能康复臂带训练评估与健康管理系统
- 软件简称：智康臂带
- 软件版本：V1.0
- 子模块：Web服务端与训练数据可视化端
- 源代码目录：F:\qiansai\web

Web端工程由Python Flask服务端和dashboard.html前端页面组成。该工程不包含GPIO、ADC、SPI等传统嵌入式硬件驱动，因此硬件初始化/驱动模块按服务运行环境、MQTT接入和外部设备数据接收初始化如实整理。代码以完整函数为单位保留，每个函数均标注真实文件路径、模块名称、函数名称和用途。

## 一、主程序入口

---
文件路径：F:\qiansai\web\server.py
模块名称：一、主程序入口
函数名称：load_local_env
主要用途：保留该函数的完整实现，体现Web服务初始化或外部设备接入逻辑。

```python
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：一、主程序入口
函数名称：index
主要用途：保留该函数的完整实现，体现Web服务初始化或外部设备接入逻辑。

```python
def index():
    dashboard_path = os.path.join(os.path.dirname(__file__), "dashboard.html")
    if os.path.exists(dashboard_path):
        with open(dashboard_path, "r", encoding="utf-8") as fp:
            return render_template_string(fp.read())

    html_template = r"""
```

---
文件路径：F:\qiansai\web\server.py
模块名称：一、主程序入口
函数名称：start_mqtt_subscriber
主要用途：保留该函数的完整实现，体现Web服务初始化或外部设备接入逻辑。

```python
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


```

## 二、硬件初始化/驱动模块

---
文件路径：F:\qiansai\web\server.py
模块名称：二、硬件初始化/驱动模块
函数名称：load_local_env
主要用途：保留该函数的完整实现，体现Web服务初始化或外部设备接入逻辑。

```python
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：二、硬件初始化/驱动模块
函数名称：start_mqtt_subscriber
主要用途：保留该函数的完整实现，体现Web服务初始化或外部设备接入逻辑。

```python
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


```

## 三、核心业务逻辑

---
文件路径：F:\qiansai\web\server.py
模块名称：训练数据接收与业务处理
函数名称：ingest_device_payload
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：训练数据接收与业务处理
函数名称：upload_data
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：训练数据接收与业务处理
函数名称：api_start
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def api_start():
    system_state["is_training"] = True
    system_state["training_started_at"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    system_state["last_page"] = "realtime"
    notify_realtime_update()
    return jsonify({"ok": True, "system_state": system_state})


```

---
文件路径：F:\qiansai\web\server.py
模块名称：训练数据接收与业务处理
函数名称：api_stop
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def api_stop():
    if system_state["is_training"]:
        add_history_record()
    system_state["is_training"] = False
    system_state["last_page"] = "result"
    notify_realtime_update()
    return jsonify({"ok": True, "system_state": system_state})


```

---
文件路径：F:\qiansai\web\server.py
模块名称：训练数据接收与业务处理
函数名称：maybe_add_training_history_from_payload
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：训练数据接收与业务处理
函数名称：seed_history
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：训练状态与页面业务逻辑
函数名称：startTraining
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    async function startTraining() { await fetch('/api/start', { method: 'POST' }); systemState.is_training = true; showPage('realtime'); updateTopStatus(); }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：训练状态与页面业务逻辑
函数名称：stopTraining
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    async function stopTraining() { await fetch('/api/stop', { method: 'POST' }); systemState.is_training = false; showPage('result'); updateTopStatus(); }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：训练状态与页面业务逻辑
函数名称：applyRealtime
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    function applyRealtime(obj) {
      systemState = obj.system_state || systemState;
      const rt = obj.realtime || {};
      const wristDevice = (obj.devices || {}).wrist || {};
      const angle = clamp(rt.display_angle ?? rt.elbow_angle, 0, 180);
      const now = performance.now();
      const hadSample = state.lastSampleAt > 0;
      const previousSampleAt = state.lastSampleAt || now;
      const sampleGap = Math.max(80, now - previousSampleAt);
      const countChanged = state.lastCount === null || obj.count !== state.lastCount;
      state.prevAngle = state.lastAngle;
      state.lastAngle = angle;
      state.lastSampleAt = now;
      state.lastCount = obj.count;
      if (!hadSample) {
        state.angleVelocity = 0;
        state.smoothChartAngle = angle;
        state.smoothDisplayAngle = angle;
        state.targetAngle = angle;
        state.displayAngleReady = true;
      } else if (countChanged) {
        const rawVelocity = (state.lastAngle - state.prevAngle) / sampleGap;
        const delta = Math.abs(state.lastAngle - state.prevAngle);
        const damping = clamp(1 - delta / 140, 0.24, 0.86);
        state.angleVelocity = clamp(rawVelocity * damping, -0.045, 0.045);
      }
      updateTopStatus(obj); renderDeviceStatus(obj); renderResult(rt); updateMotion(rt, obj.parsed || {});
      const healthFresh = renderHealth(obj.health || {});
      setText('homeCount', obj.count ?? 0);
      setText('rtAction', rt.action || '待识别');
      setText('heroAction', rt.action || '待识别');
      const emgEnabled = rt.emg_enabled !== false;
      const emgStrength = emgEnabled ? clamp(rt.emg_strength ?? 0, 0, 100) : 0;
      const emgMode = emgEnabled ? clamp(rt.emg_mode ?? 0, 0, 2) : 0;
      const emgLabel = emgEnabled ? (rt.emg_label || ['放松', '较弱', '用力'][emgMode] || '放松') : '未启用';
      const emgClass = emgEnabled ? (emgMode === 2 ? 'strong' : (emgMode === 1 ? 'weak' : 'relax')) : 'disabled';
      setText('rtEmgLabel', emgLabel);
      setText('rtEmgStrength', emgStrength);
      ids('rtEmgLabel').className = 'value emg-value ' + emgClass;
      ids('rtEmgFill').className = 'emg-fill ' + emgClass;
      ids('rtEmgFill').style.setProperty('--emg', emgStrength + '%');
      setText('rtCompletion', rt.completion ?? 0);
      setText('rtScore', rt.score ?? 0);
      setText('ratingText', rating(rt.score || 0));
      ids('progressRing').style.setProperty('--pct', clamp(rt.completion, 0, 100) * 3.6 + 'deg');
      const label = obj.time ? obj.time.split(' ').pop() : new Date().toLocaleTimeString('zh-CN', { hour12: false });
      state.points.push({ label, angle, score: rt.score || 0, emg: rt.emg || 0 });
      if (state.points.length > 72) state.points.shift();
      drawLiveChart();
      const pageName = String(obj.parsed?.page || obj.system_state?.last_page || '').toLowerCase();
      const trainingPage = pageName === 'training' || pageName === 'realtime' || pageName === 'train';
      const wristHealthActive = !!wristDevice.health_active;
      const wristHealthEnter = !!wristDevice.health_enter;
      const wristHealthExit = !!wristDevice.health_exit;
      const healthRequest = !!wristDevice.health_request;
      const upperPageEvent = Number(obj.parsed?.uevt || 0);
      const manualLocked = Date.now() < state.manualNavUntil;
      const healthAutoSuppressed = Date.now() < state.healthSuppressUntil;
      if (wristHealthActive || healthFresh) state.wristHealthActiveSeen = true;
      if ((state.page === 'health') && wristHealthExit && isFreshRealtime(obj)) {
        state.wristHealthActiveSeen = false;
        state.healthSuppressUntil = Date.now() + 3000;
        showPage('home', 'mcu');
      }
      if (!manualLocked && (trainingPage || obj.system_state?.is_training) && isFreshRealtime(obj) && (state.page === 'home' || state.page === 'result')) showPage('realtime', 'mcu');
      if (!manualLocked && upperPageEvent === 1 && !wristHealthActive && !wristHealthEnter && !healthRequest) showPage('result', 'mcu');
      if (!manualLocked && upperPageEvent === 2 && !wristHealthActive && !wristHealthEnter && !healthRequest) showPage('home', 'mcu');
      if (!manualLocked && !healthAutoSuppressed && !wristHealthExit && (wristHealthEnter || healthRequest || wristHealthActive || healthFresh) && !obj.system_state?.is_training && (state.page === 'home' || state.page === 'result')) showPage('health', 'mcu');
      if (!manualLocked && (state.page === 'health') && !wristHealthExit && (state.wristHealthActiveSeen && !wristHealthActive && !healthFresh) && isFreshRealtime(obj)) {
        state.wristHealthActiveSeen = false;
        showPage('home', 'mcu');
      }
      if (!manualLocked && !healthFresh && !healthRequest && !wristHealthActive && !wristHealthEnter && !wristHealthExit && (pageName === 'result' || pageName === 'free_result')) showPage('result', 'mcu');
      else if (!manualLocked && !healthFresh && !healthRequest && !wristHealthActive && !wristHealthEnter && !wristHealthExit && pageName === 'home' && isFreshRealtime(obj) && state.page !== 'home') showPage('home', 'mcu');
    }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：训练状态与页面业务逻辑
函数名称：renderResult
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    function renderResult(rt) {
      setText('resultScore', rt.score ?? 0);
      setText('resultReps', `${rt.reps ?? 0}/${rt.target_reps ?? 0}`);
      setText('resultAction', rt.action || '--');
    }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：训练状态与页面业务逻辑
函数名称：renderHistory
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
      state.history = obj.records || []; renderHistory(); drawHistoryChart();
    }
    async function loadHealthHistory() {
      try {
        const obj = await (await fetch('/api/health_history', { cache: 'no-store' })).json();
        const records = Array.isArray(obj.records) ? obj.records : [];
        state.healthTrend = records.map(item => ({
          label: item.label || (item.updated_at ? String(item.updated_at).split(' ').pop() : '--'),
          updated_at: item.updated_at || '',
          heart_rate: Number(item.heart_rate || 0),
          spo2: Number(item.spo2 || 0),
          hrv: Number(item.hrv || item.sdnn || item.rmssd || 0),
          fatigue: Number(item.fatigue || 0),
          micro: Number(item.micro || item.micro_circ || 0),
          seq: Number(item.seq || 0),
        }));
        state.healthPoints = state.healthTrend.map(item => ({ label: item.label, rate: item.heart_rate })).filter(item => item.rate > 0);
        const last = state.healthTrend[state.healthTrend.length - 1];
        if (last && last.seq) state.lastHealthSeq = last.seq;
        renderHealthHistory();
        renderHealthTrendCharts();
        drawHeartChart();
      } catch (err) {
        console.warn(err);
      }
    }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：训练状态与页面业务逻辑
函数名称：renderAnalysis
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
        renderAnalysis(obj);
      } catch (err) {
```

## 四、算法处理模块

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：normalize_motion_angle
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def normalize_motion_angle(value, action_index=None):
    try:
        raw_angle = abs(float(value))
        angle = int(raw_angle)
    except (TypeError, ValueError):
        raw_angle = 0.0
        angle = 0
    angle = max(0, min(180, angle))
    return angle


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：normalize_realtime
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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

```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：normalize_devices
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：normalize_health
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：clamp_number
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def clamp_number(value, low=0, high=100):
    try:
        number = float(value)
    except (TypeError, ValueError):
        number = 0
    return max(low, min(high, number))


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：record_metric
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def record_metric(record, key, fallback_key=None, default=0):
    value = record.get(key)
    if value in (None, "") and fallback_key:
        value = record.get(fallback_key)
    return clamp_number(value if value not in (None, "") else default)


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：completion_metric
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def completion_metric(record):
    if record.get("completion") not in (None, ""):
        return record_metric(record, "completion")
    reps = record_metric(record, "reps")
    target = record_metric(record, "target_reps")
    if target > 0:
        return clamp_number(reps / target * 100)
    return clamp_number(reps * 4)


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：amplitude_metric
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def amplitude_metric(record):
    if record.get("amplitude") not in (None, ""):
        return record_metric(record, "amplitude")
    angle = record_metric(record, "best_angle", "max_angle")
    return clamp_number(angle / 160 * 100)


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：average
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def average(values):
    return sum(values) / len(values) if values else 0


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：star_level
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：trend_label
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：build_training_suggestions
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：数据归一化、指标计算和趋势分析
函数名称：build_analysis_payload
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端健康指标分析和曲线计算
函数名称：evaluateHealthIndicator
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    function evaluateHealthIndicator(indicator, health) {
      const value = indicator.value(health || {});
      if (!value || value === '0/0') {
        return { value: '--', level: '暂无数据', reason: indicator.fallback || '等待健康模块上传该指标。', advice: '请保持手腕端在心率检测界面并等待数据稳定。' };
      }
      const matched = (indicator.rules || []).find(rule => rule.test(health || {}));
      if (!matched) {
        return { value, level: '待观察', reason: indicator.fallback || '当前数值暂未命中规则范围。', advice: '建议重新测量或结合连续数据观察。' };
      }
      return { value, level: matched.level, reason: matched.reason || indicator.definition, advice: matched.advice };
    }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端健康指标分析和曲线计算
函数名称：healthScore
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    function healthScore(evalItems) {
      const valid = evalItems.filter(x => x.result.level !== '暂无数据');
      if (!valid.length) return 0;
      const normal = valid.filter(x => x.result.level === '正常').length;
      const watch = valid.filter(x => x.result.level === '待观察').length;
      return Math.round(((normal + watch * 0.5) / valid.length) * 100);
    }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端健康指标分析和曲线计算
函数名称：drawCurve
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    function drawCurve(canvas, series, opts = {}) {
      if (!canvas) return;
      const rect = canvas.getBoundingClientRect();
      const dpr = window.devicePixelRatio || 1;
      canvas.width = Math.max(1, Math.floor(rect.width * dpr));
      canvas.height = Math.max(1, Math.floor(rect.height * dpr));
      const ctx = canvas.getContext('2d'); ctx.scale(dpr, dpr);
      const w = rect.width, h = rect.height, pad = 22;
      ctx.clearRect(0, 0, w, h);
      const light = isLightTheme();
      ctx.strokeStyle = light ? 'rgba(122,140,165,.24)' : 'rgba(157,176,201,.12)'; ctx.lineWidth = 1;
      for (let i = 1; i < 5; i++) { const y = pad + (h - pad * 2) * i / 5; ctx.beginPath(); ctx.moveTo(pad, y); ctx.lineTo(w - pad, y); ctx.stroke(); }
      if (!series.length) return;
      const max = opts.max || Math.max(100, ...series.map(p => p.v));
      const pts = series.map((p, i) => ({ x: pad + (w - pad * 2) * (series.length === 1 ? 1 : i / (series.length - 1)), y: h - pad - clamp(p.v, 0, max) / max * (h - pad * 2) }));
      const grad = ctx.createLinearGradient(0, pad, 0, h - pad); grad.addColorStop(0, light ? 'rgba(47,128,255,.18)' : 'rgba(91,182,255,.38)'); grad.addColorStop(1, 'rgba(47,128,255,0)');
      ctx.beginPath(); ctx.moveTo(pts[0].x, h - pad); pts.forEach((p, i) => i ? ctx.lineTo(p.x, p.y) : ctx.lineTo(p.x, p.y)); ctx.lineTo(pts[pts.length-1].x, h - pad); ctx.closePath(); ctx.fillStyle = grad; ctx.fill();
      ctx.beginPath(); pts.forEach((p, i) => { if (!i) ctx.moveTo(p.x, p.y); else { const prev = pts[i-1]; const cx = (prev.x + p.x) / 2; ctx.bezierCurveTo(cx, prev.y, cx, p.y, p.x, p.y); } });
      ctx.strokeStyle = opts.color || '#5bb6ff'; ctx.lineWidth = 3; ctx.shadowColor = opts.color || '#5bb6ff'; ctx.shadowBlur = 16; ctx.stroke(); ctx.shadowBlur = 0;
    }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端健康指标分析和曲线计算
函数名称：drawTrendCanvas
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    function drawTrendCanvas(canvas, key, max, color = '#58d6ff') {
      const data = state.healthTrend.map(p => ({ v: p[key] || 0 }));
      drawCurve(canvas, data, { max, color });
    }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端健康指标分析和曲线计算
函数名称：avgHealth
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    function avgHealth(key) {
      const values = state.healthTrend.map(item => Number(item[key] || 0)).filter(v => v > 0);
      if (!values.length) return 0;
      return values.reduce((sum, value) => sum + value, 0) / values.length;
    }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端健康指标分析和曲线计算
函数名称：healthHistoryScore
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    function healthHistoryScore() {
      const hr = avgHealth('heart_rate');
      const spo2 = avgHealth('spo2');
      const hrv = avgHealth('hrv');
      const fatigue = avgHealth('fatigue');
      let score = 100;
      if (hr && (hr < 60 || hr > 100)) score -= Math.min(20, Math.abs(hr - 80) * 0.6);
      if (spo2 && spo2 < 95) score -= Math.min(25, (95 - spo2) * 8);
      if (hrv && hrv < 22) score -= Math.min(18, (22 - hrv) * 0.7);
      if (fatigue && fatigue < 25) score -= Math.min(20, (25 - fatigue) * 0.8);
      return Math.round(clamp(score, 0, 100));
    }
```

## 五、通信与数据模块

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：build_latest_payload
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：notify_realtime_update
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def notify_realtime_update():
    global state_version
    with state_condition:
        state_version += 1
        state_condition.notify_all()


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：parse_kv_payload
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：load_health_history
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：save_health_history
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def save_health_history():
    os.makedirs(DATA_DIR, exist_ok=True)
    temp_path = HEALTH_HISTORY_PATH + ".tmp"
    with open(temp_path, "w", encoding="utf-8") as fp:
        json.dump(health_history_records[-500:], fp, ensure_ascii=False, indent=2)
    os.replace(temp_path, HEALTH_HISTORY_PATH)


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：load_training_history
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：save_training_history
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def save_training_history():
    os.makedirs(DATA_DIR, exist_ok=True)
    temp_path = TRAINING_HISTORY_PATH + ".tmp"
    with open(temp_path, "w", encoding="utf-8") as fp:
        json.dump(history_records[:500], fp, ensure_ascii=False, indent=2)
    os.replace(temp_path, TRAINING_HISTORY_PATH)


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：api_latest
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def api_latest():
    return jsonify(build_latest_payload())


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：api_device_time
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def api_device_time():
    return Response(f"epoch={int(time.time())}\n", mimetype="text/plain")


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：realtime_events
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：api_history
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def api_history():
    seed_history()
    return jsonify({"records": history_records})


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：api_health_history
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def api_health_history():
    return jsonify({"records": health_history_records[-500:]})


```

---
文件路径：F:\qiansai\web\server.py
模块名称：HTTP接口、SSE实时推送和数据存储
函数名称：api_analysis
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
def api_analysis():
    return jsonify(build_analysis_payload())


```

---
文件路径：F:\qiansai\web\server.py
模块名称：MQTT订阅与设备消息接收
函数名称：start_mqtt_subscriber
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端HTTP、SSE和AI接口交互
函数名称：pollLatest
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    async function pollLatest() { try { applyRealtime(await (await fetch('/api/latest', { cache: 'no-store' })).json()); } catch (err) { console.warn(err); } }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端HTTP、SSE和AI接口交互
函数名称：loadAnalysis
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
      if (page === 'analysis') loadAnalysis();
      if (page === 'healthAnalysis') renderHealthAnalysis();
      if (page === 'healthHistory') loadHealthHistory();
      setTimeout(() => { drawLiveChart(); drawHeartChart(); drawHistoryChart(); renderHealthTrendCharts(); renderHealthHistory(); }, 60);
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端HTTP、SSE和AI接口交互
函数名称：loadHistory
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
      if (page === 'history') loadHistory();
      if (page === 'analysis') loadAnalysis();
      if (page === 'healthAnalysis') renderHealthAnalysis();
      if (page === 'healthHistory') loadHealthHistory();
      setTimeout(() => { drawLiveChart(); drawHeartChart(); drawHistoryChart(); renderHealthTrendCharts(); renderHealthHistory(); }, 60);
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端HTTP、SSE和AI接口交互
函数名称：loadHealthHistory
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
      if (page === 'healthHistory') loadHealthHistory();
      setTimeout(() => { drawLiveChart(); drawHeartChart(); drawHistoryChart(); renderHealthTrendCharts(); renderHealthHistory(); }, 60);
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端HTTP、SSE和AI接口交互
函数名称：connectRealtimeStream
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    function connectRealtimeStream() {
      if (!window.EventSource) { state.pollTimer = setInterval(pollLatest, 400); return; }
      const events = new EventSource('/events');
      events.addEventListener('realtime_data', event => { try { applyRealtime(JSON.parse(event.data)); } catch (err) { console.warn(err); } });
      events.onerror = () => { events.close(); if (!state.pollTimer) state.pollTimer = setInterval(pollLatest, 400); };
    }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端HTTP、SSE和AI接口交互
函数名称：sendAiChat
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    async function sendAiChat(message) {
      if (state.aiChatBusy) return;
      const input = ids('aiChatInput');
      const text = (message || input?.value || '').trim();
      if (!text) return;
      if (input) input.value = '';
      state.aiChatMessages.push({ role: 'user', content: text });
      state.aiChatMessages.push({ role: 'assistant', content: '正在调用模型生成建议...' });
      state.aiChatBusy = true;
      renderAiChat();
      try {
        const resp = await fetch('/api/ai_chat', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ message: text, messages: state.aiChatMessages.filter(m => m.content !== '正在调用模型生成建议...') })
        });
        const obj = await resp.json();
        state.aiChatMessages.pop();
        if (!resp.ok || !obj.ok) throw new Error(obj.error || '模型调用失败');
        state.aiChatMessages.push({ role: 'assistant', content: obj.answer || '模型未返回内容。' });
        setText('aiModelName', obj.model || 'qwen3.7-plus');
      } catch (err) {
        state.aiChatMessages.pop();
        state.aiChatMessages.push({ role: 'assistant', content: `模型调用失败：${err.message || err}` });
      } finally {
        state.aiChatBusy = false;
        renderAiChat();
      }
    }
```

## 六、异常处理与辅助模块

---
文件路径：F:\qiansai\web\server.py
模块名称：AI建议调用和异常响应
函数名称：build_ai_context
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：AI建议调用和异常响应
函数名称：call_dashscope_chat
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\server.py
模块名称：AI建议调用和异常响应
函数名称：api_ai_chat
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
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


```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端异常处理和显示辅助
函数名称：escapeHtml
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
        <div class="ai-msg ${msg.role === 'user' ? 'user' : 'assistant'}">${escapeHtml(msg.content || '')}</div>
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端异常处理和显示辅助
函数名称：updateTopStatus
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    async function startTraining() { await fetch('/api/start', { method: 'POST' }); systemState.is_training = true; showPage('realtime'); updateTopStatus(); }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端异常处理和显示辅助
函数名称：renderDeviceStatus
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
    function renderDeviceStatus(obj) {
      const upper = (obj.devices || {}).upper || {};
      const wrist = (obj.devices || {}).wrist || {};
      const hasUploaded = Number(obj.count || 0) > 0 && isFreshRealtime(obj);
      setText('upperStatus', hasUploaded && upper.bluetooth_connected ? '已连接' : '未连接');
      setText('wristStatus', hasUploaded && wrist.bluetooth_connected ? '已连接' : '未连接');
      setText('upperMeta', `电量 ${upper.battery_valid ? upper.battery + '%' : '--'} · ${upper.sleeping ? '息屏中' : '未息屏'}`);
      setText('wristMeta', `电量 ${wrist.battery_valid ? wrist.battery + '%' : '--'} · ${wrist.sleeping ? '息屏中' : '未息屏'}`);
      const health = obj.health || {};
      setText('homeHeartRate', health.valid && health.heart_rate ? health.heart_rate : '--');
      setText('homeHealthMeta', `血氧 ${health.valid && health.spo2 ? health.spo2 + '%' : '--'} · 血压 ${health.valid && health.sbp ? health.sbp : '--'}/${health.valid && health.dbp ? health.dbp : '--'}`);
    }
```

---
文件路径：F:\qiansai\web\dashboard.html
模块名称：前端异常处理和显示辅助
函数名称：renderHealthAnalysis
主要用途：保留该函数的完整实现，体现Web端对应模块的核心处理逻辑。

`$lang
      if (page === 'healthAnalysis') renderHealthAnalysis();
      if (page === 'healthHistory') loadHealthHistory();
      setTimeout(() => { drawLiveChart(); drawHeartChart(); drawHistoryChart(); renderHealthTrendCharts(); renderHealthHistory(); }, 60);
```
