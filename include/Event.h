#pragma once
#include <iostream> // Para std::string no Desktop
#include <cstdint>  // Para uint32_t

// Estrutura simples para lidar com datas no simulador
struct SimpleDate {
    int year;
    int month;
    int day;

    // Construtor conveniente
    SimpleDate(int y = 0, int m = 0, int d = 0) : year(y), month(m), day(d) {}
};

struct Event {
    uint32_t id;
    std::string title;
    SimpleDate targetDate;
    bool annual; // true = Aniversário/Anual; false = Evento pontual

    // Construtor básico (usando std::string para Desktop)
    Event(uint32_t id, const std::string &title, SimpleDate date, bool isAnnual = false)
        : id(id), title(title), targetDate(date), annual(isAnnual) {}
};