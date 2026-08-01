import os, re
from PIL import Image

SRC = "a27ds/Assets/_Game/Sprites"
OUTC = "out/Parachute_AKA/main/assets_hd.cpp"
OUTH = "out/Parachute_AKA/main/assets_hd.h"

SCREEN_W, SCREEN_H = 320, 240
SRC_W, SRC_H = 890, 530
SCALE = SCREEN_W / SRC_W                     # fit width -> letterbox top/bottom
CANVAS_H = round(SRC_H * SCALE)              # ~191
OFFY = (SCREEN_H - CANVAS_H) // 2            # ~24
HD_TRANS = 0xF81F                            # magenta color key (absent from art)

def pack(r, g, b):                           # native BGR565 (red in low bits)
    return (r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11)

def cname(f):
    return "img_" + re.sub(r'[^0-9A-Za-z]', '_', os.path.splitext(f)[0])

def convert(path, crop=True):
    im = Image.open(path).convert("RGBA")
    if crop:
        bbox = im.split()[3].getbbox() or (0, 0, im.width, im.height)
    else:
        bbox = (0, 0, im.width, im.height)
    im = im.crop(bbox)
    w = max(1, round(im.width  * SCALE))
    h = max(1, round(im.height * SCALE))
    im = im.resize((w, h), Image.LANCZOS)
    px = im.load()
    data = []
    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            data.append(HD_TRANS if a < 128 else pack(r, g, b))
    x0 = round(bbox[0] * SCALE)
    y0 = round(bbox[1] * SCALE) + OFFY
    return x0, y0, w, h, data

files = sorted(f for f in os.listdir(SRC) if f.endswith(".png"))
entries = {}   # cname -> (x,y,w,h,data,opaque)
for f in files:
    opaque = (f == "parachute_background.png")
    entries[cname(f)] = convert(os.path.join(SRC, f), crop=not opaque) + (opaque,)

# ---- emit .cpp ----
with open(OUTC, "w") as c:
    c.write('#include "assets_hd.h"\n\n')
    for name, (x, y, w, h, data, opq) in entries.items():
        c.write(f"static const uint16_t {name}_data[{len(data)}] = {{\n")
        for i in range(0, len(data), 16):
            c.write("  " + ",".join(f"0x{v:04X}" for v in data[i:i+16]) + ",\n")
        c.write("};\n")
        c.write(f"const HdImage {name} = {{ {x}, {y}, {w}, {h}, {name}_data }};\n\n")
    # convenience groups
    def arr(nm, names):
        c.write(f"const HdImage* const {nm}[{len(names)}] = {{ " +
                ", ".join("&"+n for n in names) + " };\n")
    arr("hd_paraA", [f"img_A_{i}" for i in range(1,8)])
    arr("hd_paraB", [f"img_B_{i}" for i in range(1,7)])
    arr("hd_paraC", [f"img_C_{i}" for i in range(1,6)])
    arr("hd_water", ["img_A_Water","img_B_Water","img_C_Water"])
    arr("hd_dead",  [f"img_Dead_{i}" for i in range(1,4)])
    arr("hd_shark", [f"img_Shark_{i}" for i in range(1,6)])
    arr("hd_life",  [f"img_Life_{i}" for i in range(1,4)])
    arr("hd_boat",  ["img_P_A","img_P_B","img_P_C"])

# ---- emit .h ----
with open(OUTH, "w") as h:
    h.write("#pragma once\n#include <stdint.h>\n\n")
    h.write("// Generated from a27ds/Parachute sprites (890x530 RGBA frames),\n")
    h.write("// cropped to their used area, scaled to the 320x240 screen, converted\n")
    h.write("// to native BGR565 with a magenta transparency key. (x,y) is the\n")
    h.write("// top-left on screen, letterboxed vertically.\n\n")
    h.write("struct HdImage { int16_t x, y; uint16_t w, h; const uint16_t* data; };\n\n")
    h.write(f"static const uint16_t HD_TRANS = 0x{HD_TRANS:04X};\n")
    h.write(f"static const int HD_OFFY = {OFFY};\n\n")
    for name in entries:
        h.write(f"extern const HdImage {name};\n")
    h.write("\n")
    for nm, n in [("hd_paraA",7),("hd_paraB",6),("hd_paraC",5),("hd_water",3),
                  ("hd_dead",3),("hd_shark",5),("hd_life",3),("hd_boat",3)]:
        h.write(f"extern const HdImage* const {nm}[{n}];\n")

# ---- summary ----
print(f"SCALE={SCALE:.4f}  canvas={SCREEN_W}x{CANVAS_H}  offsetY={OFFY}  key=0x{HD_TRANS:04X}")
for k in ["img_parachute_background","img_A_1","img_A_7","img_B_1","img_C_1",
          "img_P_A","img_Helicopter","img_Shark_1","img_Dead_1","img_Life_1",
          "img_Miss","img_Clock","img_GameA"]:
    x,y,w,h,d,o = entries[k]
    print(f"  {k:26} pos=({x:3},{y:3}) size={w:3}x{h:<3} {'opaque' if o else 'key'}")
tot = sum(len(v[4]) for v in entries.values())
print(f"total pixels stored = {tot} ({tot*2/1024:.0f} KB flash)")
