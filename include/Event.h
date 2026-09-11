#pragma once
#include <string>
#include <cstdint>

struct SimpleDate {
    int year, month, day;
    SimpleDate(int y = 0, int m = 0, int d = 0) : year(y), month(m), day(d) {}
    
    long toTotalDays() const { return (year * 360) + (month * 30) + day; }
};

struct Event {
    uint32_t id;
    std::string title;
    SimpleDate targetDate;
    bool annual;

    Event(uint32_t id, const std::string &title, SimpleDate date, bool isAnnual = false)
        : id(id), title(title), targetDate(date), annual(isAnnual) {}
};