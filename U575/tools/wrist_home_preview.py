"""
Desktop preview for the wrist LVGL UI.

Run:
    python "E:/qiansai/U575/tools/wrist_home_preview.py"
    python "E:/qiansai/U575/tools/wrist_home_preview.py" --screen health
"""

from __future__ import annotations

import argparse
import tkinter as tk
from tkinter import font as tkfont


SCREEN_W = 240
SCREEN_H = 280

TXT_TITLE = "\u667a\u80fd\u5eb7\u590d\u8bc4\u4f30\u81c2\u5e26"
TXT_TRAINING = "\u8bad\u7ec3\u6a21\u5f0f"
TXT_REHAB = "\u5f00\u59cb\u5eb7\u590d\u8bad\u7ec3"
TXT_HEART = "\u5fc3\u7387"
TXT_REALTIME = "\u5f00\u59cb\u5b9e\u65f6\u76d1\u6d4b"
TXT_CONNECTED = "\u5df2\u8fde\u63a5"
TXT_DISCONNECTED = "\u672a\u8fde\u63a5"
TXT_HEALTH = "\u5065\u5eb7\u76d1\u6d4b"
TXT_SPO2 = "\u8840\u6c27"
TXT_WAVE = "\u5fc3\u7387\u6ce2\u5f62"
TXT_MONITORING = "\u5b9e\u65f6\u76d1\u6d4b\u4e2d"


def rgb(hex_color: int) -> str:
    return f"#{hex_color:06X}"


def clamp(value: int, low: int, high: int) -> int:
    return max(low, min(high, value))


class WristHomePreview:
    def __init__(self, scale: int = 3, connected: bool = True, screen: str = "home") -> None:
        self.scale = scale
        self.connected = connected
        self.screen = screen
        self.root = tk.Tk()
        self.root.title("Wrist UI Preview")
        self.canvas = tk.Canvas(
            self.root,
            width=SCREEN_W * scale,
            height=SCREEN_H * scale,
            bg=rgb(0xF4FAFF),
            highlightthickness=0,
        )
        self.canvas.pack()

        self.font_title = self.pick_font(16)
        self.font_status = self.pick_font(16)
        self.font_card_title = self.pick_font(16)
        self.font_card_subtitle = self.pick_font(14)
        self.font_arrow = self.pick_font(14, family="Arial")

    def pick_font(self, size: int, weight: str = "normal", family: str | None = None) -> tkfont.Font:
        families = set(tkfont.families(self.root))
        candidates = [family] if family else ["Microsoft YaHei", "SimHei", "Arial Unicode MS", "Arial"]
        chosen = next((name for name in candidates if name and name in families), "Arial")
        return tkfont.Font(family=chosen, size=size * self.scale, weight=weight)

    def s(self, value: float) -> float:
        return value * self.scale

    def rect(self, x: float, y: float, w: float, h: float, fill: str, outline: str = "", width: int = 1) -> None:
        self.canvas.create_rectangle(
            self.s(x), self.s(y), self.s(x + w), self.s(y + h),
            fill=fill, outline=outline, width=max(1, int(width * self.scale)),
        )

    def round_rect(self, x: float, y: float, w: float, h: float, r: float, fill: str, outline: str = "", width: int = 1) -> None:
        x0, y0, x1, y1, r = self.s(x), self.s(y), self.s(x + w), self.s(y + h), self.s(r)
        line_width = max(1, int(width * self.scale))
        self.canvas.create_arc(x0, y0, x0 + 2 * r, y0 + 2 * r, start=90, extent=90, fill=fill, outline=outline, width=line_width)
        self.canvas.create_arc(x1 - 2 * r, y0, x1, y0 + 2 * r, start=0, extent=90, fill=fill, outline=outline, width=line_width)
        self.canvas.create_arc(x1 - 2 * r, y1 - 2 * r, x1, y1, start=270, extent=90, fill=fill, outline=outline, width=line_width)
        self.canvas.create_arc(x0, y1 - 2 * r, x0 + 2 * r, y1, start=180, extent=90, fill=fill, outline=outline, width=line_width)
        self.canvas.create_rectangle(x0 + r, y0, x1 - r, y1, fill=fill, outline=outline, width=line_width)
        self.canvas.create_rectangle(x0, y0 + r, x1, y1 - r, fill=fill, outline=outline, width=line_width)

    def label(self, x: float, y: float, text: str, fill: str, font: tkfont.Font, anchor: str = "nw") -> None:
        self.canvas.create_text(self.s(x), self.s(y), text=text, fill=fill, font=font, anchor=anchor)

    def draw_training_icon(self, cx: float, cy: float) -> None:
        blue = rgb(0x1877F2)
        self.round_rect(cx - 15, cy - 3.5, 30, 7, 4, blue)
        self.round_rect(cx - 22, cy - 16, 10, 32, 6, blue)
        self.round_rect(cx + 12, cy - 16, 10, 32, 6, blue)
        self.round_rect(cx - 29.5, cy - 10, 5, 20, 3, blue)
        self.round_rect(cx + 24.5, cy - 10, 5, 20, 3, blue)

    def draw_home_heart_icon(self, ox: float, oy: float) -> None:
        red = rgb(0xF24E5E)
        hole = rgb(0xFFECEF)
        heart = [
            (26, 20), (27, 17), (28, 16), (29, 15), (31, 13), (33, 12),
            (35, 12), (37, 12), (40, 13), (42, 14), (44, 16), (45, 18),
            (46, 20), (46, 22), (46, 24), (45, 27), (44, 29), (42, 31),
            (40, 33), (37, 35), (35, 37), (33, 39), (31, 41), (29, 43),
            (28, 44), (27, 45), (26, 48), (25, 45), (24, 44), (23, 43),
            (21, 41), (19, 39), (17, 37), (15, 35), (12, 33), (10, 31),
            (8, 29), (7, 27), (6, 24), (6, 22), (6, 20), (7, 18),
            (8, 16), (10, 14), (12, 13), (15, 12), (17, 12), (19, 12),
            (21, 13), (23, 15), (24, 16), (25, 17),
        ]
        flat = []
        for x, y in heart:
            flat.extend([self.s(ox + x), self.s(oy + y)])
        self.canvas.create_polygon(*flat, fill=red, outline=red)
        ecg = [(7, 28), (16, 28), (21, 21), (27, 37), (33, 28), (45, 28)]
        flat = []
        for x, y in ecg:
            flat.extend([self.s(ox + x), self.s(oy + y)])
        self.canvas.create_line(*flat, fill=hole, width=max(1, int(4 * self.scale)), capstyle=tk.ROUND, joinstyle=tk.ROUND)

    def draw_bluetooth_icon(self, connected: bool) -> None:
        color = rgb(0x1877F2 if connected else 0x9AA8BD)
        ox = SCREEN_W - 80 - 18
        oy = SCREEN_H - 13 - 24
        for points in ([(9, 2), (9, 22)], [(9, 2), (16, 8), (9, 14), (3, 8)], [(9, 14), (16, 20), (9, 22), (3, 20)]):
            flat = []
            for x, y in points:
                flat.extend([self.s(ox + x), self.s(oy + y)])
            self.canvas.create_line(*flat, fill=color, width=max(1, int(3 * self.scale)), capstyle=tk.ROUND, joinstyle=tk.ROUND)

    def draw_card(self, y: float, card_fill: str, border: str, icon_fill: str, title: str, subtitle: str, accent: str, icon: str) -> None:
        x = 12
        self.round_rect(x, y, 216, 78, 18, card_fill, border)
        self.round_rect(x + 13, y + 13, 52, 52, 26, icon_fill, icon_fill)
        if icon == "training":
            self.draw_training_icon(x + 39, y + 39)
        else:
            self.draw_home_heart_icon(x + 13, y + 13)
        self.label(x + 68, y + 13, title, rgb(0x102B6B), self.font_card_title)
        self.label(x + 68, y + 40, subtitle, rgb(0x7890B7), self.font_card_subtitle)
        self.label(x + 205, y + 39, ">", accent, self.font_arrow, anchor="e")

    def draw_home(self) -> None:
        self.rect(0, 0, SCREEN_W, SCREEN_H, rgb(0xF4FAFF), rgb(0xF4FAFF))
        self.label(12, 12, TXT_TITLE, rgb(0x102B6B), self.font_title)
        self.round_rect(192, 13, 25, 14, 4, rgb(0xF4FAFF), rgb(0x102B6B), width=2)
        self.round_rect(194, 16, 17, 8, 2, rgb(0x28D95B), rgb(0x28D95B))
        self.label(228, 11, "85%", rgb(0x102B6B), self.font_title, anchor="ne")
        self.round_rect(14, 36, 42, 4, 3, rgb(0x1877F2), rgb(0x1877F2))
        self.draw_card(60, rgb(0xF8FCFF), rgb(0x95C3FF), rgb(0xE6F2FF), TXT_TRAINING, TXT_REHAB, rgb(0x1877F2), "training")
        self.draw_card(152, rgb(0xFFF8FA), rgb(0xFFC3CC), rgb(0xFFECEF), TXT_HEART, TXT_REALTIME, rgb(0xF24E5E), "heart")
        self.draw_bluetooth_icon(self.connected)
        self.label(14, SCREEN_H - 13, "00:00", rgb(0x4E6799), self.font_status, anchor="sw")
        self.label(SCREEN_W - 14, SCREEN_H - 13, TXT_CONNECTED if self.connected else TXT_DISCONNECTED, rgb(0x102B6B if self.connected else 0x7B8799), self.font_status, anchor="se")

    def draw_health_metric(self, x: float, y: float, title: str, value: str, unit: str, color: str, icon: str) -> None:
        self.round_rect(x, y, 104, 104, 16, rgb(0xFFFFFF), rgb(0xDCEBFF))
        self.round_rect(x + 29, y + 7, 46, 46, 23, rgb(0xFFF0F3 if icon == "heart" else 0xEAF4FF), "")
        if icon == "heart":
            heart = [(52, 18), (55, 12), (61, 10), (67, 14), (68, 21), (63, 28), (57, 34), (52, 39), (47, 34), (41, 28), (36, 21), (37, 14), (43, 10), (49, 12)]
            flat = []
            for px, py in heart:
                flat.extend([self.s(x + px), self.s(y + py)])
            self.canvas.create_polygon(*flat, fill=color, outline=color)
            ecg = [(39, 24), (47, 24), (50, 18), (55, 32), (60, 24), (67, 24)]
            flat = []
            for px, py in ecg:
                flat.extend([self.s(x + px), self.s(y + py)])
            self.canvas.create_line(*flat, fill=rgb(0xFFFFFF), width=max(1, int(3 * self.scale)), capstyle=tk.ROUND, joinstyle=tk.ROUND)
        else:
            self.round_rect(x + 36, y + 14, 32, 32, 16, color, color)
            self.label(x + 52, y + 30, "O2", rgb(0xFFFFFF), self.font_arrow, anchor="center")
        self.label(x + 52, y + 56, title, rgb(0x102B6B), self.font_card_title, anchor="n")
        self.canvas.create_text(self.s(x + 15), self.s(y + 87), text=value, fill=rgb(0x102B6B), font=tkfont.Font(family="Arial", size=28 * self.scale, weight="bold"), anchor="sw")
        self.label(x + 92, y + 88, unit, rgb(0x52617D), self.font_arrow, anchor="se")

    def draw_health(self) -> None:
        self.rect(0, 0, SCREEN_W, SCREEN_H, rgb(0xF4FAFF), rgb(0xF4FAFF))
        self.canvas.create_text(self.s(20), self.s(24), text="<", fill=rgb(0x102B6B), font=tkfont.Font(family="Arial", size=28 * self.scale, weight="bold"), anchor="center")
        self.label(SCREEN_W / 2, 13, TXT_HEALTH, rgb(0x102B6B), self.font_title, anchor="n")
        self.round_rect(99, 40, 42, 4, 3, rgb(0x2F80ED), rgb(0x2F80ED))
        self.draw_health_metric(10, 55, TXT_HEART, "76", "BPM", rgb(0xF24E5E), "heart")
        self.draw_health_metric(126, 55, TXT_SPO2, "98", "%", rgb(0x2F80ED), "o2")
        self.round_rect(10, 184, 220, 84, 16, rgb(0xFFFFFF), rgb(0xDCEBFF))
        wave = [(15, 214), (28, 211), (41, 207), (54, 212), (67, 213), (80, 196), (93, 232), (106, 217), (119, 213), (132, 210), (145, 212), (158, 214), (171, 207), (184, 203), (197, 209), (210, 212)]
        flat = []
        for x, y in wave:
            flat.extend([self.s(x), self.s(y)])
        self.canvas.create_line(*flat, fill=rgb(0xF24E5E), width=max(1, int(3 * self.scale)), capstyle=tk.ROUND, joinstyle=tk.ROUND)
        self.label(22, 246, TXT_WAVE, rgb(0xF24E5E), self.font_card_subtitle, anchor="sw")
        self.round_rect(143, 246, 12, 12, 6, rgb(0x27AE60), rgb(0x27AE60))
        self.label(220, 250, TXT_MONITORING, rgb(0x102B6B), self.font_card_subtitle, anchor="se")

    def draw(self) -> None:
        if self.screen == "health":
            self.draw_health()
        else:
            self.draw_home()

    def run(self) -> None:
        self.draw()
        self.root.mainloop()


def main() -> None:
    parser = argparse.ArgumentParser(description="Preview wrist UI on desktop.")
    parser.add_argument("--scale", type=int, default=3, help="Window scale factor, default: 3")
    parser.add_argument("--screen", choices=["home", "health"], default="home", help="Screen to preview")
    parser.add_argument("--connected", action="store_true", default=True, help="Show connected Bluetooth state")
    parser.add_argument("--disconnected", action="store_true", help="Show disconnected Bluetooth state")
    args = parser.parse_args()
    WristHomePreview(scale=clamp(args.scale, 1, 6), connected=not args.disconnected, screen=args.screen).run()


if __name__ == "__main__":
    main()
