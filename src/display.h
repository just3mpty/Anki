#pragma once

#include "kana_data.h"

namespace display {

void begin();
void showQuestion(const Kana& kana);
void showAnswer(const Kana& kana);

}
