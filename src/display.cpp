#include "display.h"

#include <Fonts/FreeSansBold24pt7b.h>
#include <GxEPD2_BW.h>
#include <SPI.h>

#include "pins.h"

namespace {

using EpdDriver = GxEPD2_213_B74;
GxEPD2_BW<EpdDriver, EpdDriver::HEIGHT> epd(
    EpdDriver(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY));

constexpr uint8_t ROTATION = 0;  // 2 = à l'envers

// Mise en page (122 x 250)
constexpr int16_t KANA_Y = 20;
constexpr int16_t ROMAJI_TOP = 140;
constexpr int16_t ROMAJI_BOTTOM = 196;
constexpr int16_t ROMAJI_BASELINE = 182;

// Anti image fantôme
constexpr uint8_t FULL_REFRESH_EVERY = 20;
uint8_t partialCount = 0;

void drawCentered(const char* text, int16_t baseline) {
    int16_t x1, y1;
    uint16_t w, h;
    epd.getTextBounds(text, 0, baseline, &x1, &y1, &w, &h);
    epd.setCursor((epd.width() - w) / 2 - x1, baseline);
    epd.print(text);
}

}

namespace display {

void begin() {
    SPI.begin(PIN_EPD_SCK, -1, PIN_EPD_MOSI, PIN_EPD_CS);
    epd.init(115200, true, 2, false);
    epd.setRotation(ROTATION);
    epd.setFont(&FreeSansBold24pt7b);
    epd.setTextColor(GxEPD_BLACK);
}

void showQuestion(const Kana& kana) {
    epd.fillScreen(GxEPD_WHITE);
    epd.drawBitmap((epd.width() - GLYPH_SIZE) / 2, KANA_Y, kana.glyph,
                   GLYPH_SIZE, GLYPH_SIZE, GxEPD_BLACK);

    bool partial = partialCount < FULL_REFRESH_EVERY;
    epd.display(partial);
    partialCount = partial ? partialCount + 1 : 0;
}

void showAnswer(const Kana& kana) {
    constexpr int16_t height = ROMAJI_BOTTOM - ROMAJI_TOP;
    epd.fillRect(0, ROMAJI_TOP, epd.width(), height, GxEPD_WHITE);
    drawCentered(kana.romaji, ROMAJI_BASELINE);
    epd.displayWindow(0, ROMAJI_TOP, epd.width(), height);
    partialCount++;
}

}
