import argparse
import csv
import sys
import time
from collections import deque
from pathlib import Path

try:
    import serial
    from serial.tools import list_ports
except ImportError:
    serial = None
    list_ports = None

try:
    import matplotlib.pyplot as plt
    from matplotlib.animation import FuncAnimation
    from matplotlib.widgets import Button
except ImportError:
    plt = None
    FuncAnimation = None
    Button = None


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
    "relax": "Keep the arm fully relaxed.",
    "weak": "Apply light and steady muscle force.",
    "active": "Apply normal training-level muscle force.",
}


def parse_args():
    parser = argparse.ArgumentParser(
        description="Live raw ADC plot and labeled EMG CSV collection."
    )
    parser.add_argument("--port", help="Serial port, for example COM7.")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument(
        "--out",
        default=str(Path(__file__).with_name("emg_raw_dataset.csv")),
        help="Output CSV path.",
    )
    parser.add_argument("--rounds", type=int, default=5)
    parser.add_argument("--duration", type=float, default=30.0)
    parser.add_argument("--prepare", type=float, default=5.0)
    parser.add_argument("--rest", type=float, default=5.0)
    parser.add_argument("--seconds", type=float, default=20.0)
    parser.add_argument("--sample-rate", type=float, default=100.0)
    parser.add_argument("--interval", type=int, default=80)
    parser.add_argument(
        "--labels",
        nargs="+",
        default=["relax", "weak", "active"],
        choices=["relax", "weak", "active"],
    )
    return parser.parse_args()


def choose_port():
    if list_ports is None:
        raise RuntimeError("pyserial is not installed. Run: pip install pyserial")

    ports = list(list_ports.comports())
    if not ports:
        raise RuntimeError("No serial ports found.")

    print("Available serial ports:")
    for index, port in enumerate(ports, start=1):
        print(f"  {index}. {port.device}  {port.description}")

    while True:
        value = input("Select serial port number: ").strip()
        try:
            index = int(value)
        except ValueError:
            continue
        if 1 <= index <= len(ports):
            return ports[index - 1].device


def parse_emg_line(line):
    if line.startswith("EMGSTAT,"):
        print(line.strip(), flush=True)
        return None

    parts = line.strip().split(",")
    if len(parts) == 7 and parts[0] == "EMGDATA":
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


class EmgCollector:
    def __init__(self, args, ser, writer, output_file):
        self.args = args
        self.ser = ser
        self.writer = writer
        self.output_file = output_file
        self.start_time = time.monotonic()
        self.state = "idle"
        self.phase_start = time.monotonic()
        self.round_index = 0
        self.label_index = 0
        self.total_saved = 0
        self.phase_saved = 0
        self.last_row = None
        max_points = max(200, int(args.seconds * args.sample_rate))
        self.times = deque(maxlen=max_points)
        self.raw_values = deque(maxlen=max_points)
        self.strength_values = deque(maxlen=max_points)

    @property
    def current_label(self):
        if self.label_index >= len(self.args.labels):
            return self.args.labels[-1]
        return self.args.labels[self.label_index]

    def start_collection(self, _event=None):
        if self.state in ("waiting", "idle"):
            self.round_index = 1
            self.label_index = 0
            self.state = "prepare"
            self.phase_start = time.monotonic()
            self.phase_saved = 0

    def stop_collection(self, _event=None):
        self.state = "idle"
        self.phase_start = time.monotonic()
        self.output_file.flush()

    def _advance_after_collect(self):
        self.output_file.flush()
        self.label_index += 1
        if self.label_index >= len(self.args.labels):
            self.label_index = 0
            self.round_index += 1

        if self.round_index > self.args.rounds:
            self.state = "done"
        elif self.args.rest > 0:
            self.state = "rest"
        else:
            self.state = "prepare"

        self.phase_start = time.monotonic()
        self.phase_saved = 0

    def _advance_after_rest(self):
        self.state = "prepare"
        self.phase_start = time.monotonic()
        self.phase_saved = 0

    def _save_row(self, row):
        saved = dict(row)
        saved["pc_time"] = f"{time.time():.3f}"
        saved["label"] = self.current_label
        self.writer.writerow(saved)
        self.total_saved += 1
        self.phase_saved += 1

    def read_serial(self):
        parsed_count = 0
        max_read = max(40, int(self.args.sample_rate * self.args.interval / 1000.0 * 2.0))
        while parsed_count < max_read:
            raw = self.ser.readline()
            if not raw:
                break
            row = parse_emg_line(raw.decode("ascii", errors="ignore"))
            if row is None:
                continue
            parsed_count += 1
            self.last_row = row
            t = time.monotonic() - self.start_time
            self.times.append(t)
            self.raw_values.append(row["raw"])
            self.strength_values.append(row["strength"])
            if self.state == "collect":
                self._save_row(row)

    def update_phase(self):
        now = time.monotonic()
        elapsed = now - self.phase_start

        if self.state == "prepare" and elapsed >= self.args.prepare:
            self.state = "collect"
            self.phase_start = now
            self.phase_saved = 0
        elif self.state == "collect" and elapsed >= self.args.duration:
            self._advance_after_collect()
        elif self.state == "rest" and elapsed >= self.args.rest:
            self._advance_after_rest()

    def status_text(self):
        row = self.last_row
        if row is None:
            signal = "waiting for EMG serial data..."
        else:
            if row["calibrated"] == 0:
                signal = (
                    f"raw={row['raw']}  sample_count={row['sample_count']}  "
                    f"board=CALIBRATING (keep relaxed)"
                )
            else:
                signal = (
                    f"raw={row['raw']}  sample_count={row['sample_count']}  "
                    f"board={'FORCE' if row['active'] else 'RELAX'}  "
                    f"strength={row['strength']:3d}%"
                )

        elapsed = time.monotonic() - self.phase_start
        if self.state == "idle":
            phase = "Ready. Click START to collect labeled data."
        elif self.state == "prepare":
            left = max(0.0, self.args.prepare - elapsed)
            phase = (
                f"Prepare {self.current_label}: {left:4.1f}s  "
                f"{LABEL_PROMPTS[self.current_label]}"
            )
        elif self.state == "collect":
            left = max(0.0, self.args.duration - elapsed)
            phase = (
                f"Collecting {self.current_label}: {left:4.1f}s  "
                f"round={self.round_index}/{self.args.rounds}  "
                f"saved={self.phase_saved}"
            )
        elif self.state == "rest":
            left = max(0.0, self.args.rest - elapsed)
            phase = f"Rest: {left:4.1f}s"
        elif self.state == "done":
            phase = "Done. CSV has been saved."
        else:
            phase = self.state

        return f"{phase}\n{signal}\nTotal saved: {self.total_saved} rows"


def main():
    args = parse_args()

    if serial is None:
        print("Missing pyserial. Run: pip install pyserial", file=sys.stderr)
        return 2
    if plt is None:
        print("Missing matplotlib. Run: pip install matplotlib", file=sys.stderr)
        return 2

    port = args.port or choose_port()
    out_path = Path(args.out)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    append = out_path.exists() and out_path.stat().st_size > 0

    with serial.Serial(port, args.baud, timeout=0.02) as ser, out_path.open(
        "a", newline="", encoding="utf-8"
    ) as output_file:
        writer = csv.DictWriter(output_file, fieldnames=FIELDNAMES)
        if not append:
            writer.writeheader()

        ser.reset_input_buffer()
        collector = EmgCollector(args, ser, writer, output_file)

        fig, (ax_signal, ax_strength) = plt.subplots(
            2, 1, sharex=True, figsize=(12, 7), gridspec_kw={"height_ratios": [3, 1]}
        )
        fig.canvas.manager.set_window_title("Upper-arm EMG Visual Collector")
        fig.subplots_adjust(left=0.08, right=0.97, top=0.88, bottom=0.16, hspace=0.16)

        raw_line, = ax_signal.plot([], [], label="raw", color="#2F80FF", linewidth=1.2)
        ax_signal.set_ylabel("raw ADC")
        strength_line, = ax_strength.plot(
            [], [], label="estimated strength", color="#EF4444", linewidth=1.5
        )
        ax_strength.axhline(20, color="#F59E0B", linestyle="--", linewidth=1.0, label="force on")
        ax_strength.axhline(10, color="#22C55E", linestyle="--", linewidth=1.0, label="force off")
        ax_strength.set_ylabel("strength %")
        ax_strength.set_xlabel("time s")
        ax_signal.grid(True, alpha=0.22)
        ax_strength.grid(True, alpha=0.22)
        ax_signal.legend(loc="upper right")
        ax_signal.set_ylim(0, 4095)
        ax_strength.set_ylim(-5, 105)
        ax_strength.legend(loc="upper right")

        status = fig.text(
            0.08,
            0.965,
            "Waiting...",
            va="top",
            ha="left",
            fontsize=10,
            family="monospace",
        )

        start_ax = fig.add_axes([0.72, 0.045, 0.1, 0.055])
        stop_ax = fig.add_axes([0.84, 0.045, 0.1, 0.055])
        start_button = Button(start_ax, "START")
        stop_button = Button(stop_ax, "STOP")
        start_button.on_clicked(collector.start_collection)
        stop_button.on_clicked(collector.stop_collection)

        def update(_frame):
            collector.read_serial()
            collector.update_phase()

            x = list(collector.times)
            if x:
                raw_line.set_data(x, list(collector.raw_values))
                strength_line.set_data(x, list(collector.strength_values))
                xmin = max(0.0, x[-1] - args.seconds)
                xmax = max(args.seconds, x[-1])
                ax_signal.set_xlim(xmin, xmax)

            status.set_text(collector.status_text())
            return raw_line, strength_line, status

        ani = FuncAnimation(fig, update, interval=args.interval, blit=False)
        try:
            print(f"Serial: {port} @ {args.baud}")
            print(f"Output: {out_path.resolve()}")
            print("Click START in the plot window to begin collection.")
            plt.show()
        finally:
            output_file.flush()
            _ = ani

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
