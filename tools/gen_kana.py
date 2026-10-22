"""Génère src/kana_data.cpp. Usage : python tools/gen_kana.py"""

import sys
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont, ImageOps

ROOT = Path(__file__).resolve().parent.parent
FONT_PATH = ROOT / "tools" / "fonts" / "KleeOne-SemiBold.ttf"
OUT_CPP = ROOT / "src" / "kana_data.cpp"
OUT_PREVIEW = ROOT / "tools" / "preview.png"

GLYPH_SIZE = 104  # multiple de 8
MARGIN = 2

# (hiragana, katakana, romaji)
GOJUON = [
    ("あ", "ア", "a"), ("い", "イ", "i"), ("う", "ウ", "u"), ("え", "エ", "e"), ("お", "オ", "o"),
    ("か", "カ", "ka"), ("き", "キ", "ki"), ("く", "ク", "ku"), ("け", "ケ", "ke"), ("こ", "コ", "ko"),
    ("さ", "サ", "sa"), ("し", "シ", "shi"), ("す", "ス", "su"), ("せ", "セ", "se"), ("そ", "ソ", "so"),
    ("た", "タ", "ta"), ("ち", "チ", "chi"), ("つ", "ツ", "tsu"), ("て", "テ", "te"), ("と", "ト", "to"),
    ("な", "ナ", "na"), ("に", "ニ", "ni"), ("ぬ", "ヌ", "nu"), ("ね", "ネ", "ne"), ("の", "ノ", "no"),
    ("は", "ハ", "ha"), ("ひ", "ヒ", "hi"), ("ふ", "フ", "fu"), ("へ", "ヘ", "he"), ("ほ", "ホ", "ho"),
    ("ま", "マ", "ma"), ("み", "ミ", "mi"), ("む", "ム", "mu"), ("め", "メ", "me"), ("も", "モ", "mo"),
    ("や", "ヤ", "ya"), ("ゆ", "ユ", "yu"), ("よ", "ヨ", "yo"),
    ("ら", "ラ", "ra"), ("り", "リ", "ri"), ("る", "ル", "ru"), ("れ", "レ", "re"), ("ろ", "ロ", "ro"),
    ("わ", "ワ", "wa"), ("を", "ヲ", "wo"),
    ("ん", "ン", "n"),
]

KANA = [(h, r, "Hiragana") for h, _, r in GOJUON] + \
       [(k, r, "Katakana") for _, k, r in GOJUON]


def pick_font():
    half = GLYPH_SIZE / 2 - MARGIN
    for size in range(GLYPH_SIZE + 20, 10, -1):
        font = ImageFont.truetype(str(FONT_PATH), size)
        if all(_fits(font.getbbox(c, anchor="mm"), half) for c, _, _ in KANA):
            return font
    raise RuntimeError("Aucune taille de police ne convient")


def _fits(bbox, half):
    left, top, right, bottom = bbox
    return -half <= left and right <= half and -half <= top and bottom <= half


def render(font, char):
    img = Image.new("L", (GLYPH_SIZE, GLYPH_SIZE), 255)
    ImageDraw.Draw(img).text((GLYPH_SIZE / 2, GLYPH_SIZE / 2), char, font=font, fill=0, anchor="mm")
    return img.point(lambda p: 0 if p < 128 else 255)


def to_bytes(img):
    return ImageOps.invert(img).convert("1", dither=Image.Dither.NONE).tobytes()


def cpp_source(font, images):
    row = GLYPH_SIZE // 8
    lines = [
        "// FICHIER GÉNÉRÉ par tools/gen_kana.py — ne pas modifier à la main.",
        f"// Police : {FONT_PATH.name} (SIL Open Font License), {font.size} px.",
        "",
        '#include "kana_data.h"',
        "",
        f"const int16_t GLYPH_SIZE = {GLYPH_SIZE};",
        "",
        "namespace {",
        "",
    ]
    for i, ((char, romaji, _), img) in enumerate(zip(KANA, images)):
        data = to_bytes(img)
        lines.append(f"// {char} ({romaji})")
        lines.append(f"const uint8_t GLYPH_{i}[] = {{")
        for y in range(0, len(data), row):
            lines.append("    " + ", ".join(f"0x{b:02X}" for b in data[y:y + row]) + ",")
        lines += ["};", ""]
    lines += ["}", "", "const Kana KANA_TABLE[] = {"]
    for i, (char, romaji, script) in enumerate(KANA):
        lines.append(f'    {{"{char}", "{romaji}", Script::{script}, GLYPH_{i}}},')
    lines += ["};", "", "const size_t KANA_COUNT = sizeof(KANA_TABLE) / sizeof(KANA_TABLE[0]);", ""]
    return "\n".join(lines)


def save_preview(images, cols=18):
    rows = -(-len(images) // cols)
    sheet = Image.new("L", (cols * GLYPH_SIZE, rows * GLYPH_SIZE), 255)
    for i, img in enumerate(images):
        sheet.paste(img, ((i % cols) * GLYPH_SIZE, (i // cols) * GLYPH_SIZE))
    sheet.convert("1", dither=Image.Dither.NONE).save(OUT_PREVIEW)


def main():
    sys.stdout.reconfigure(encoding="utf-8")
    font = pick_font()
    images = [render(font, c) for c, _, _ in KANA]

    OUT_CPP.write_text(cpp_source(font, images), encoding="utf-8")
    save_preview(images)

    print(f"Police {FONT_PATH.name} à {font.size} px")
    print(f"Écrit : {OUT_CPP.relative_to(ROOT)} ({len(KANA)} kana)")
    print(f"Aperçu : {OUT_PREVIEW.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
