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
    // Agora aceita a flag isAnnual (padrão é false)
    uint32_t addEvent(const std::string &title, SimpleDate date, bool isAnnual = false) {
        if (date.year == 0 && !isAnnual) return 0;
        events.emplace_back(nextId, title, date, isAnnual);
        return nextId++;
    }

    long getDaysUntil(uint32_t id) const {
        for (const auto &e : events) {
            if (e.id == id) {
                if (!e.annual) {
                    // Evento normal (Pontual): Calcula a diferença direta
                    return e.targetDate.toTotalDays() - simulatedNow.toTotalDays();
                } else {
                    // Evento Circular (Aniversário)
                    // Passo 1: Monta a data do evento com o ano ATUAL
                    SimpleDate thisYearDate(simulatedNow.year, e.targetDate.month, e.targetDate.day);
                    long diff = thisYearDate.toTotalDays() - simulatedNow.toTotalDays();
                    
                    // Passo 2: Se deu negativo, o aniversário já passou este ano. 
                    // Recalcula somando +1 no ano!
                    if (diff < 0) {
                        SimpleDate nextYearDate(simulatedNow.year + 1, e.targetDate.month, e.targetDate.day);
                        return nextYearDate.toTotalDays() - simulatedNow.toTotalDays();
                    }
                    return diff; // Se não deu negativo, o aniversário ainda vai chegar este ano
                }
            }
        }
        return -9999;
    }

    void printEvents() const {
        std::cout << "\n--- CALENDARIO DE EVENTOS E ANIVERSARIOS ---\n";
        std::cout << "ID  | Tipo | Data        | Faltam   | Titulo\n";
        std::cout << "----+------+-------------+----------+----------------------------\n";
        if (events.empty()) { std::cout << "(Vazio)\n"; } else {
            for (const auto &e : events) {
                long days = getDaysUntil(e.id);
                std::string daysStr = (days == -9999) ? "???" : (days < 0 ? "PASSOU" : std::to_string(days) + " dias");
                std::string tipo = e.annual ? "Aniv" : "Pont"; 
                
                // --- MÁGICA VISUAL DO ANO AQUI ---
                int displayYear = e.targetDate.year;
                if (e.annual) {
                    SimpleDate thisYearDate(simulatedNow.year, e.targetDate.month, e.targetDate.day);
                    if (thisYearDate.toTotalDays() < simulatedNow.toTotalDays()) {
                        displayYear = simulatedNow.year + 1; // Já passou, exibe o ano que vem!
                    } else {
                        displayYear = simulatedNow.year; // Ainda vai acontecer este ano.
                    }
                }
                
                std::cout << "#" << e.id << "  | " << tipo << " | "
                          << displayYear << "-" << (e.targetDate.month < 10 ? "0" : "") << e.targetDate.month << "-" << (e.targetDate.day < 10 ? "0" : "") << e.targetDate.day << " | "
                          << (daysStr.length() < 8 ? daysStr + "  " : daysStr) << " | " 
                          << e.title << "\n";
            }
        }
        std::cout << "-------------------------------------------------------------\n\n";
    }
// ==================== PROGRAMA PRINCIPAL ====================
int main() {
    TaskManagerDesktop taskMgr;
    NoteManagerDesktop noteMgr;
    EventManagerDesktop eventMgr;
    std::string cmd;

    std::cout << "=== SIMULADOR DE ASSISTENTE (DESKTOP) ===\n";
    std::cout << "Comandos principais:\n";
    std::cout << "  tasks | notes | events\n";
    std::cout << "  add-task <tit>\n";
    std::cout << "  add-event <tit> | <YY-MM-DD>\n";
    std::cout << "  add-bday <tit> | <MM-DD>       <-- Opção de Aniversário aqui!\n";
    std::cout << "  set-now <YY-MM-DD>\n";
    std::cout << "  exit\n\n";

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
        else if (cmd.rfind("add-bday ", 0) == 0) {
            std::string full = cmd.substr(9);
            size_t pipePos = full.find('|');
            if (pipePos != std::string::npos) {
                std::string title = full.substr(0, pipePos);
                std::string dateStr = full.substr(pipePos + 1);
                while(!title.empty() && title.back() == ' ') title.pop_back();
                while(!dateStr.empty() && dateStr.front() == ' ') dateStr.erase(0, 1);
                
                // Extrai apenas MM-DD. O ano que a pessoa nasceu (ex: 2000) não importa
                // para o cálculo circular, pois sempre usaremos o ano atual.
                try {
                    int m = std::stoi(dateStr.substr(0, 2));
                    int d = std::stoi(dateStr.substr(3, 2));
                    
                    uint32_t id = eventMgr.addEvent(title, SimpleDate(2000, m, d), true);
                    if (id > 0) {
                        std::cout << "[OK] Aniversario #" << id << " criado!\n";
                        eventMgr.printEvents();
                    }
                } catch (...) { std::cout << "[ERRO] Use formato: MM-DD\n"; }
            } else { std::cout << "[ERRO] Formato: add-bday <titulo> | MM-DD\n"; }
        // ... (código existente de comando não reconhecido) ...
        }
        else if (!cmd.empty()) { std::cout << "[ERRO] Comando nao reconhecido.\n"; }
    }
    return 0;
}