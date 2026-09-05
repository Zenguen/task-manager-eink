#pragma once
#include <Arduino.h>
#include <vector>

enum class Priority {
    PRIORITY_LOW,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH
};

struct Task {
    uint32_t id;
    String title;
    bool completed;
    Priority priority;
    std::vector<uint32_t> dependencyIds;

    Task(uint32_t id, const String &title, Priority priority = Priority::PRIORITY_MEDIUM)
        : id(id), title(title), completed(false), priority(priority) {}
};