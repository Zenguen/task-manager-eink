#pragma once
#include <string>
#include <cstdint>

struct Note {
    uint32_t id;
    std::string title;
    std::string content;

    Note(uint32_t id, const std::string &title, const std::string &content = "")
        : id(id), title(title), content(content) {}
};