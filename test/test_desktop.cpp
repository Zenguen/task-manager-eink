#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <sstream>

// Importando as Entidades REAIS do projeto!
#include "../include/Task.h"
#include "../include/Note.h"
#include "../include/Event.h"

// ==================== UTILITÁRIOS DE DATA (SIMULADOR) ====================
// Variável global que mantém o "HOJE" simulado usando a estrutura oficial
SimpleDate simulatedNow(2026, 1, 1); 

// Helper para converter texto (YYYY-MM-DD) na estrutura SimpleDate oficial
SimpleDate parseDateStr(const std::string& dateStr) {
    if (dateStr.length() != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
        return SimpleDate(0,0,0); 
    }
    try {
        int y = std::stoi(dateStr.substr(0, 4));
        int m = std::stoi(dateStr.substr(5, 2));
        int d = std::stoi(dateStr.substr(8, 2));
        return SimpleDate(y, m, d);
    } catch (...) {
        return SimpleDate(0,0,0);
    }
}

// ==================== MANAGERS (SIMULADOR) ====================
class TaskManagerDesktop {
private:
    std::vector<Task> tasks;
    uint32_t nextId = 1;
public:
    uint32_t addTask(const std::string &title) {
        tasks.emplace_back(nextId, title);
        return nextId++;
    }
    bool toggleTask(uint32_t id) {
        for (auto &t : tasks) { if (t.id == id) { t.completed = !t.completed; return true; } }
        return false;
    }
    void printTasks() const {
        std::cout << "\n--- LISTA DE TAREFAS ---\nID  | Status | Titulo\n----+--------+----------------------------\n";
        if (tasks.empty()) { std::cout << "(Vazia)\n"; } else {
            for (const auto &t : tasks) {
                std::cout << "#" << t.id << "  |  [" << (t.completed ? 'X' : ' ') << "]   | " << t.title << "\n";
            }
        }
        std::cout << "-----------------------------------------\n\n";
    }
};

class NoteManagerDesktop {
private:
    std::vector<Note> notes;
    uint32_t nextId = 1;
public:
    uint32_t addNote(const std::string &title, const std::string &content = "") {
        notes.emplace_back(nextId, title, content);
        return nextId++;
    }
    bool deleteNote(uint32_t id) {
        for (auto it = notes.begin(); it != notes.end(); ++it) { if (it->id == id) { notes.erase(it); return true; } }
        return false;
    }
    void printNotes() const {
        std::cout << "\n--- BLOCO DE NOTAS ---\n";
        if (notes.empty()) { std::cout << "(Vazia)\n"; } else {
            for (const auto &n : notes) {
                std::cout << "[Nota #" << n.id << "] " << n.title << "\n";
                if (!n.content.empty()) { std::cout << "  > Content: \"" << n.content << "\"\n"; }
            }
        }
        std::cout << "----------------------\n\n";
    }
};

class EventManagerDesktop {
private:
    std::vector<Event> events;
    uint32_t nextId = 1;
public:
    uint32_t addEvent(const std::string &title, SimpleDate date) {
        if (date.year == 0) return 0;
        events.emplace_back(nextId, title, date);
        return nextId++;
    }

    long getDaysUntil(uint32_t id) const {
        for (const auto &e : events) {
            if (e.id == id) {
                return e.targetDate.toTotalDays() - simulatedNow.toTotalDays();
            }
        }
        return -9999;
    }

    void printEvents() const {
        std::cout << "\n--- CALENDARIO DE EVENTOS (Tempo: Mock) ---\n";
        std::cout << "ID  | Data        | Faltam   | Titulo\n";
        std::cout << "----+-------------+----------+----------------------------\n";
        if (events.empty()) { std::cout << "(Vazio)\n"; } else {
            for (const auto &e : events) {
                long days = getDaysUntil(e.id);
                std::string daysStr = (days == -9999) ? "???" : (days < 0 ? "PASSOU" : std::to_string(days) + " dias");
                std::cout << "#" << e.id << "  | " 
                          << e.targetDate.year << "-" << (e.targetDate.month < 10 ? "0" : "") << e.targetDate.month << "-" << (e.targetDate.day < 10 ? "0" : "") << e.targetDate.day << " | "
                          << (daysStr.length() < 8 ? daysStr + "  " : daysStr) << " | " 
                          << e.title << "\n";
            }
        }
        std::cout << "---------------------------------------------------------\n\n";
    }
};

// ==================== PROGRAMA PRINCIPAL ====================
int main() {
    TaskManagerDesktop taskMgr;
    NoteManagerDesktop noteMgr;
    EventManagerDesktop eventMgr;
    std::string cmd;

    std::cout << "=== SIMULADOR DE ASSISTENTE (DESKTOP) ===\n";
    std::cout << "Comandos: tasks | notes | events | add-task <tit> | add-event <tit> | <YY-MM-DD> | set-now <YY-MM-DD> | exit\n\n";

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, cmd) || cmd == "exit") break;

        if (cmd == "tasks") { taskMgr.printTasks(); } 
        else if (cmd == "notes") { noteMgr.printNotes(); }
        else if (cmd == "events") { eventMgr.printEvents(); }
        
        else if (cmd.rfind("set-now ", 0) == 0) {
            std::string dateStr = cmd.substr(8);
            SimpleDate newNow = parseDateStr(dateStr);
            if (newNow.year > 0) {
                simulatedNow = newNow;
                std::cout << "[OK] Hoje simulado definido para: " << simulatedNow.year << "-" << simulatedNow.month << "-" << simulatedNow.day << "\n";
                eventMgr.printEvents();
            } else {
                std::cout << "[ERRO] Formato de data invalido.\n";
            }
        }
        
        else if (cmd.rfind("add-task ", 0) == 0) {
            std::string title = cmd.substr(9);
            uint32_t id = taskMgr.addTask(title);
            std::cout << "[OK] Tarefa #" << id << " criada!\n";
            taskMgr.printTasks();
        } 
        else if (cmd.rfind("add-event ", 0) == 0) {
            std::string full = cmd.substr(10);
            size_t pipePos = full.find('|');
            if (pipePos != std::string::npos) {
                std::string title = full.substr(0, pipePos);
                std::string dateStr = full.substr(pipePos + 1);
                while(!title.empty() && title.back() == ' ') title.pop_back();
                while(!dateStr.empty() && dateStr.front() == ' ') dateStr.erase(0, 1);
                
                uint32_t id = eventMgr.addEvent(title, parseDateStr(dateStr));
                if (id > 0) {
                    std::cout << "[OK] Evento #" << id << " criado!\n";
                    eventMgr.printEvents();
                } else {
                    std::cout << "[ERRO] Falha ao criar evento.\n";
                }
            } else { std::cout << "[ERRO] Formato: add-event <titulo> | YYYY-MM-DD\n"; }
        }
        else if (!cmd.empty()) { std::cout << "[ERRO] Comando nao reconhecido.\n"; }
    }
    return 0;
}