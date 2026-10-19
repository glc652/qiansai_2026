import argparse
import time
from collections import deque

try:
    import serial
    from serial.tools import list_ports
except ImportError:
    serial = None
    list_ports = None

try:
    import matplotlib.pyplot as plt
    from matplotlib.animation import FuncAnimation
except ImportError:
    plt = None
    FuncAnimation = None


def parse_args():
    parser = argparse.ArgumentParser(description="Live plot upper-arm EMG values from debug UART.")
    parser.add_argument("--port", help="Serial port, for example COM7.")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--seconds", type=float, default=20.0, help="Visible time window.")
    parser.add_argument("--sample-rate", type=float, default=100.0, help="Expected EMG sample rate in Hz.")
    parser.add_argument("--interval", type=int, default=100, help="Plot refresh interval in ms.")
    return parser.parse_args()


def choose_port():
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


def parse_line(line):
    if line.startswith("EMGSTAT,"):
        print(line.strip(), flush=True)
        return None

    parts = line.strip().split(",")
    if len(parts) in (7, 9) and parts[0] == "EMG":
        try:
            mode = int(parts[7]) if len(parts) == 9 else None
            mode_label = parts[8] if len(parts) == 9 else ""
            return {
                "raw": int(parts[1]),
                "baseline": int(parts[2]) / 10.0,
                "rectified": int(parts[3]) / 10.0,
                "rms": int(parts[4]) / 10.0,
                "calibrated": int(parts[5]),
                "strength": int(parts[6]),
                "mode": mode,
                "mode_label": mode_label,
            }
        except ValueError:
            return None

    if len(parts) == 13 and parts[0] == "EMGCSV":
        try:
            return {
                "raw": int(parts[2]),
                "baseline": int(parts[3]) / 10.0,
                "rectified": int(parts[5]) / 10.0,
                "rms": int(parts[7]) / 10.0,
                "active": int(parts[8]),
                "calibrated": int(parts[10]),
                "strength": int(parts[9]),
                "mode": None,
                "mode_label": "",
            }
        except ValueError:
            return None
    return None


def main():
    args = parse_args()
    if serial is None:
        raise SystemExit("缺少 pyserial，请先运行：pip install pyserial")
    if plt is None:
        raise SystemExit("缺少 matplotlib，请先运行：pip install matplotlib")

    port = args.port or choose_port()
    max_points = max(200, int(args.seconds * args.sample_rate))
    times = deque(maxlen=max_points)
    raw_values = deque(maxlen=max_points)
    baseline_values = deque(maxlen=max_points)
    rectified_values = deque(maxlen=max_points)
    rms_values = deque(maxlen=max_points)
    strength_values = deque(maxlen=max_points)
    start = time.monotonic()

    ser = serial.Serial(port, args.baud, timeout=0.02)
    ser.reset_input_buffer()

    fig, (ax_signal, ax_strength) = plt.subplots(2, 1, sharex=True, figsize=(10, 6))
    fig.canvas.manager.set_window_title("Upper-arm EMG Live Plot")

    raw_line, = ax_signal.plot([], [], label="raw", color="#4C78A8", linewidth=1.2)
    base_line, = ax_signal.plot([], [], label="baseline", color="#888888", linewidth=1.0)
    rect_line, = ax_signal.plot([], [], label="rectified", color="#F58518", linewidth=1.2)
    rms_line, = ax_signal.plot([], [], label="rms", color="#54A24B", linewidth=1.4)
    strength_line, = ax_strength.plot([], [], label="strength", color="#E45756", linewidth=1.5)

    ax_signal.set_ylabel("ADC / amplitude")
    ax_strength.set_ylabel("strength %")
    ax_strength.set_xlabel("time s")
    ax_signal.grid(True, alpha=0.25)
    ax_strength.grid(True, alpha=0.25)
    ax_signal.legend(loc="upper right")
    ax_strength.legend(loc="upper right")
    ax_strength.set_ylim(-5, 105)

    status = ax_signal.text(
        0.01,
        0.95,
        "waiting EMG...",
        transform=ax_signal.transAxes,
        va="top",
        ha="left",
    )

    def update(_frame):
        parsed_count = 0
        max_read = max(20, int(args.sample_rate * args.interval / 1000.0 * 2.0))
        while parsed_count < max_read:
            raw = ser.readline()
            if not raw:
                break
            row = parse_line(raw.decode("ascii", errors="ignore"))
            if row is None:
                continue
            t = time.monotonic() - start
            times.append(t)
            raw_values.append(row["raw"])
            baseline_values.append(row["baseline"])
            rectified_values.append(row["rectified"])
            rms_values.append(row["rms"])
            strength_values.append(row["strength"])
            status.set_text(
                f"calibrated={row['calibrated']}  "
                f"raw={row['raw']}  rect={row['rectified']:.1f}  "
                f"rms={row['rms']:.1f}  strength={row['strength']}  "
                f"active={row.get('active', '-')}  "
                f"mode={row.get('mode_label') or row.get('mode') or '-'}"
            )
            parsed_count += 1

        x = list(times)
        if not x:
            return raw_line, base_line, rect_line, rms_line, strength_line, status

        raw_line.set_data(x, list(raw_values))
        base_line.set_data(x, list(baseline_values))
        rect_line.set_data(x, list(rectified_values))
        rms_line.set_data(x, list(rms_values))
        strength_line.set_data(x, list(strength_values))

        xmin = max(0.0, x[-1] - args.seconds)
        xmax = max(args.seconds, x[-1])
        ax_signal.set_xlim(xmin, xmax)
        signal_max = max(
            max(raw_values, default=100),
            max(baseline_values, default=100),
            max(rectified_values, default=100),
            max(rms_values, default=100),
        )
        ax_signal.set_ylim(0, max(100.0, signal_max * 1.15))
        return raw_line, base_line, rect_line, rms_line, strength_line, status

    ani = FuncAnimation(fig, update, interval=args.interval, blit=False)
    try:
        plt.tight_layout()
        plt.show()
    finally:
        ser.close()
        _ = ani


if __name__ == "__main__":
    main()
