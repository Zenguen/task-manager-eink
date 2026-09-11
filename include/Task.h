#pragma once
#include <string>
#include <cstdint>

enum class Priority {
    PRIORITY_LOW,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH
};

struct Task {
    uint32_t id;
    std::string title;
    bool completed;
    Priority priority;

    Task(uint32_t id, const std::string &title, Priority priority = Priority::PRIORITY_MEDIUM)
        : id(id), title(title), completed(false), priority(priority) {}
};