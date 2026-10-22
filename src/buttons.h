#pragma once

#include <stdint.h>

namespace buttons {

enum class Event : uint8_t {
    None,
    Btn1,
    Btn2,
    Both,      // appui court
    BothLong,  // maintenus 1 s
};

void begin();
Event poll();
const char* name(Event event);

}
