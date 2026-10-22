#include "buttons.h"

#include <Arduino.h>

#include "pins.h"

// Geste décidé au relâchement. Après BothLong, le relâchement est ignoré.

namespace {

constexpr uint32_t DEBOUNCE_MS = 30;
constexpr uint32_t BOTH_LONG_MS = 1000;

constexpr uint8_t BIT_BTN1 = 0b01;
constexpr uint8_t BIT_BTN2 = 0b10;
constexpr uint8_t BITS_BOTH = BIT_BTN1 | BIT_BTN2;

struct Debounced {
    int pin;
    bool pressed = false;
    bool lastRaw = false;
    uint32_t lastChangeMs = 0;

    explicit Debounced(int p) : pin(p) {}

    void update(uint32_t now) {
        bool raw = digitalRead(pin) == LOW;
        if (raw != lastRaw) {
            lastRaw = raw;
            lastChangeMs = now;
        } else if (raw != pressed && now - lastChangeMs >= DEBOUNCE_MS) {
            pressed = raw;
        }
    }
};

Debounced btn1(PIN_BTN1);
Debounced btn2(PIN_BTN2);

uint8_t gestureMask = 0;
bool bothDown = false;
uint32_t bothSinceMs = 0;
bool longFired = false;

}

namespace buttons {

void begin() {
    pinMode(PIN_BTN1, INPUT_PULLUP);
    pinMode(PIN_BTN2, INPUT_PULLUP);
}

Event poll() {
    uint32_t now = millis();
    btn1.update(now);
    btn2.update(now);
    uint8_t pressed = (btn1.pressed ? BIT_BTN1 : 0) | (btn2.pressed ? BIT_BTN2 : 0);

    if (pressed) {
        if (!gestureMask) longFired = false;
        gestureMask |= pressed;

        if (pressed != BITS_BOTH) {
            bothDown = false;
        } else if (!bothDown) {
            bothDown = true;
            bothSinceMs = now;
        } else if (!longFired && now - bothSinceMs >= BOTH_LONG_MS) {
            longFired = true;
            return Event::BothLong;
        }
        return Event::None;
    }

    uint8_t mask = gestureMask;
    gestureMask = 0;
    bothDown = false;
    if (longFired) return Event::None;

    switch (mask) {
        case BIT_BTN1:  return Event::Btn1;
        case BIT_BTN2:  return Event::Btn2;
        case BITS_BOTH: return Event::Both;
        default:        return Event::None;
    }
}

const char* name(Event event) {
    switch (event) {
        case Event::Btn1:     return "BTN1";
        case Event::Btn2:     return "BTN2";
        case Event::Both:     return "BTN1+BTN2";
        case Event::BothLong: return "BTN1+BTN2 maintenus";
        default:              return "aucun";
    }
}

}
