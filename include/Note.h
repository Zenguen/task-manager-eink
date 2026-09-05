#pragma once
#include <Arduino.h>

struct Note {
    uint32_t id;
    String title;
    String content;

    Note(uint32_t id, const String &title, const String &content = "")
        : id(id), title(title), content(content) {}
};