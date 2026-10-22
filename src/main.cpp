#include <Arduino.h>

#include "buttons.h"
#include "display.h"
#include "kana_data.h"

// Défilement auto sans boutons
constexpr bool AUTO_DEMO = true;
constexpr uint32_t AUTO_DEMO_STEP_MS = 2500;

size_t current = 0;
bool answerShown = false;

buttons::Event nextEvent() {
    if (!AUTO_DEMO) return buttons::poll();

    static uint32_t lastStepMs = 0;
    if (millis() - lastStepMs < AUTO_DEMO_STEP_MS) return buttons::Event::None;
    lastStepMs = millis();
    return answerShown ? buttons::Event::Btn2 : buttons::Event::Btn1;
}

void showQuestion() {
    const Kana& kana = KANA_TABLE[current];
    Serial.printf("[%u/%u] %s\n", unsigned(current + 1), unsigned(KANA_COUNT), kana.utf8);
    display::showQuestion(kana);
    answerShown = false;
}

void showAnswer() {
    if (answerShown) return;
    const Kana& kana = KANA_TABLE[current];
    Serial.printf("  = %s\n", kana.romaji);
    display::showAnswer(kana);
    answerShown = true;
}

void setup() {
    Serial.begin(115200);
    buttons::begin();
    display::begin();
    showQuestion();
}

void loop() {
    buttons::Event event = nextEvent();
    if (event == buttons::Event::None) return;

    Serial.printf("Geste : %s\n", buttons::name(event));
    switch (event) {
        case buttons::Event::Btn1:
            showAnswer();
            break;
        case buttons::Event::Btn2:
            current = (current + 1) % KANA_COUNT;
            showQuestion();
            break;
        default:
            break;
    }
}
