import argparse
import csv
import sys
import time
from pathlib import Path

try:
    import serial
    from serial.tools import list_ports
except ImportError:
    serial = None
    list_ports = None


FIELDNAMES = [
    "pc_time",
    "tick",
    "raw",
    "sample_count",
    "active",
    "strength",
    "calibrated",
    "label",
]

LABEL_PROMPTS = {
    "relax": "保持手臂完全放松，不要主动用力。",
    "weak": "开始轻微发力，保持稳定，不要抖动，也不要用到正常训练强度。",
    "active": "开始正常训练发力，保持稳定，不需要最大用力。",
}


def parse_args():
    parser = argparse.ArgumentParser(description="Collect upper-arm EMG CSV data from UART.")
    parser.add_argument("--port", help="Serial port, for example COM7.")
    parser.add_argument("--baud", type=int, default=115200, help="Serial baudrate.")
    parser.add_argument("--out", default="emg_raw_dataset.csv", help="Output CSV path.")
    parser.add_argument("--rounds", type=int, default=5, help="Rounds per label.")
    parser.add_argument("--duration", type=float, default=30.0, help="Collect seconds per round.")
    parser.add_argument("--prepare", type=float, default=5.0, help="Prepare countdown seconds.")
    parser.add_argument("--rest", type=float, default=5.0, help="Rest seconds between rounds.")
    parser.add_argument(
        "--labels",
        nargs="+",
        default=["relax", "weak", "active"],
        choices=["relax", "weak", "active"],
        help="Labels to collect.",
    )
    return parser.parse_args()


def choose_port():
    if list_ports is None:
        raise RuntimeError("pyserial is not installed. Run: pip install pyserial")

    ports = list(list_ports.comports())
    if not ports:
        raise RuntimeError("No serial ports found.")

    print("可用串口：")
    for index, port in enumerate(ports, start=1):
        print(f"  {index}. {port.device}  {port.description}")

    while True:
        value = input("请选择串口编号：").strip()
        try:
            index = int(value)
        except ValueError:
            continue
        if 1 <= index <= len(ports):
            return ports[index - 1].device


def countdown(seconds, message):
    seconds = int(max(seconds, 0))
    if seconds <= 0:
        return
    print(message)
    for left in range(seconds, 0, -1):
        print(f"  {left}...", end="\r", flush=True)
        time.sleep(1)
    print(" " * 16, end="\r")


def parse_emgcsv(line):
    parts = line.split(",")
    if line.startswith("EMGDATA,") and len(parts) == 7:
        try:
            return {
                "tick": int(parts[1]),
                "raw": int(parts[2]),
                "sample_count": int(parts[3]),
                "active": int(parts[4]),
                "strength": int(parts[5]),
                "calibrated": int(parts[6]),
            }
        except ValueError:
            return None
    return None


def wait_for_calibrated(ser):
    print("等待板端肌电静息标定完成，请保持放松...")
    start = time.monotonic()
    while True:
        line = ser.readline().decode("ascii", errors="ignore").strip()
        row = parse_emgcsv(line)
        if row is None:
            continue
        if row["calibrated"] != 0:
            print(
                f"标定完成，baseline_x10={row['baseline_x10']} "
                f"rms_x10={row['rms_x10']} sample_count={row['sample_count']}"
            )
            return
        if time.monotonic() - start > 15.0:
            print("仍未标定完成，继续等待。确认电极已贴稳。")
            start = time.monotonic()


def collect_round(ser, writer, label, duration):
    count = 0
    skipped = 0
    start = time.monotonic()
    end = start + duration

    while time.monotonic() < end:
        line = ser.readline().decode("ascii", errors="ignore").strip()
        row = parse_emgcsv(line)
        if row is None:
            continue
        row["pc_time"] = f"{time.time():.3f}"
        row["label"] = label
        writer.writerow(row)
        count += 1

        elapsed = time.monotonic() - start
        if count % 25 == 0:
            print(f"  {label}: {elapsed:5.1f}/{duration:.1f}s  saved={count}", end="\r", flush=True)

    print(f"  {label}: {duration:.1f}/{duration:.1f}s  saved={count} skipped={skipped}")
    return count


def main():
    args = parse_args()

    if serial is None:
        print("缺少 pyserial，请先运行：pip install pyserial", file=sys.stderr)
        return 2

    port = args.port or choose_port()
    out_path = Path(args.out)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    append = out_path.exists() and out_path.stat().st_size > 0

    print(f"打开串口 {port} @ {args.baud}")
    print(f"数据保存到：{out_path.resolve()}")
    print("采集期间请不要进入息屏；若息屏了，触摸屏幕或喊“小康”唤醒后重新开始这一组。")

    with serial.Serial(port, args.baud, timeout=1.0) as ser, out_path.open(
        "a", newline="", encoding="utf-8"
    ) as f:
        writer = csv.DictWriter(f, fieldnames=FIELDNAMES)
        if not append:
            writer.writeheader()

        ser.reset_input_buffer()

        input("准备开始采集，按回车继续。")
        total = 0
        for round_index in range(1, args.rounds + 1):
            print(f"\n========== 第 {round_index}/{args.rounds} 轮 ==========")
            for label in args.labels:
                print(f"\n标签：{label}")
                print(LABEL_PROMPTS[label])
                countdown(args.prepare, "准备倒计时")
                print("开始采集。")
                total += collect_round(ser, writer, label, args.duration)
                f.flush()
                if args.rest > 0:
                    countdown(args.rest, "休息并恢复放松")

        print(f"\n采集完成，共保存 {total} 行。")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
