#pragma once

#include <stddef.h>
#include <stdint.h>

enum class Script : uint8_t { Hiragana, Katakana };

struct Kana {
    const char* utf8;
    const char* romaji;
    Script script;
    const uint8_t* glyph;
};

extern const int16_t GLYPH_SIZE;
extern const Kana KANA_TABLE[];
extern const size_t KANA_COUNT;
