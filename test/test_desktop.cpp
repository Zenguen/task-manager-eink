#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <sstream> // Para parsear datas

// ==================== UTILITÁRIOS DE DATA (SIMULADOR) ====================
// Mock simplificado de tempo para o teste de contagem regressiva
struct SimDate {
    int y, m, d;
    SimDate(int year=2026, int month=1, int day=1) : y(year), m(month), d(day) {}
    
    // Função simples para converter data em "dias totais" (MOCK: mês = 30 dias)
    // Usada apenas para testar a lógica do EventManager no simulador Desktop.
    long toTotalDays() const {
        return (y * 360) + (m * 30) + d; 
    }
};

// Variável global que mantém o "HOJE" simulado
SimDate simulatedNow(2026, 1, 1); // Começamos em 01/01/2026

// Helper para parsear data YYYY-MM-DD
SimDate parseDateStr(const std::string& dateStr) {
    if (dateStr.length() != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
        return SimDate(0,0,0); // Formato inválido
    }
    try {
        int y = std::stoi(dateStr.substr(0, 4));
        int m = std::stoi(dateStr.substr(5, 2));
        int d = std::stoi(dateStr.substr(8, 2));
        return SimDate(y, m, d);
    } catch (...) {
        return SimDate(0,0,0);
    }
}

// ==================== DOMÍNIO DE TAREFAS ====================
struct Task {
    uint32_t id;
    std::string title;
    bool completed;
    Task(uint32_t id, const std::string &title) : id(id), title(title), completed(false) {}
};

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

// ==================== DOMÍNIO DE NOTAS ====================
struct Note {
    uint32_t id;
    std::string title;
    std::string content;
    Note(uint32_t id, const std::string &title, const std::string &content = "") : id(id), title(title), content(content) {}
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

// ==================== DOMÍNIO DE EVENTOS ====================
struct Event {
    uint32_t id;
    std::string title;
    SimDate targetDate;
    Event(uint32_t id, const std::string &title, SimDate date) : id(id), title(title), targetDate(date) {}
};

class EventManagerDesktop {
private:
    std::vector<Event> events;
    uint32_t nextId = 1;
public:
    uint32_t addEvent(const std::string &title, SimDate date) {
        if (date.y == 0) return 0; // Data inválida
        events.emplace_back(nextId, title, date);
        return nextId++;
    }

    // Calcula dias até o evento com base no MOCK de tempo
    long getDaysUntil(uint32_t id) const {
        for (const auto &e : events) {
            if (e.id == id) {
                long eventDays = e.targetDate.toTotalDays();
                long nowDays = simulatedNow.toTotalDays();
                return eventDays - nowDays;
            }
        }
        return -9999; // Evento não encontrado
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
                          << e.targetDate.y << "-" << (e.targetDate.m < 10 ? "0" : "") << e.targetDate.m << "-" << (e.targetDate.d < 10 ? "0" : "") << e.targetDate.d << " | "
                          << (daysStr.length() < 8 ? daysStr + "  " : daysStr) << " | " 
                          << e.title << "\n";
            }
        }
        std::cout << "---------------------------------------------------------\n\n";
    }
};

// ==================== PROGRAMA PRINCIPAL (SIMULADOR) ====================
int main() {
    TaskManagerDesktop taskMgr;
    NoteManagerDesktop noteMgr;
    EventManagerDesktop eventMgr;
    std::string cmd;

    // Dados iniciais de teste (Imaginando o Dashboard do design)
    taskMgr.addTask("Preparar mise en place do bolo");
    taskMgr.addTask("Pagar boleto da Vivo");
    noteMgr.addNote("Ideia de Layout", "Tags dinamicas {{tasks:1}} e {{evento:1}}");
    eventMgr.addEvent("Viagem p/ Ubatuba", SimDate(2026, 5, 22)); // Viagem em Maio de 2026

    std::cout << "=== SIMULADOR DE ASSISTENTE (DESKTOP) ===\n";
    std::cout << "Hoje simulado: " << simulatedNow.y << "-" << simulatedNow.m << "-" << simulatedNow.d << "\n";
    std::cout << "Comandos principais:\n";
    std::cout << "  set-now <YYYY-MM-DD>       -> Simula o tempo passando (MOCK)\n";
    std::cout << "  tasks | notes | events     -> Lista os módulos\n";
    std::cout << "  add-task <tit>             -> Cria tarefa\n";
    std::cout << "  toggle <id>                 -> Check na tarefa\n";
    std::cout << "  add-event <tit> | <YY-M-D> -> Cria evento (formato fixo: 2026-05-22)\n";
    std::cout << "  exit                        -> Sair\n\n";

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, cmd) || cmd == "exit") break;

        if (cmd == "tasks") { taskMgr.printTasks(); } 
        else if (cmd == "notes") { noteMgr.printNotes(); }
        else if (cmd == "events") { eventMgr.printEvents(); }
        
        // Comando Especial do Simulador: Mudar o "HOJE"
        else if (cmd.rfind("set-now ", 0) == 0) {
            std::string dateStr = cmd.substr(8);
            SimDate newNow = parseDateStr(dateStr);
            if (newNow.y > 0) {
                simulatedNow = newNow;
                std::cout << "[OK] Hoje simulado definido para: " << simulatedNow.y << "-" << simulatedNow.m << "-" << simulatedNow.d << "\n";
                eventMgr.printEvents(); // Mostra contagens atualizadas
            } else {
                std::cout << "[ERRO] Formato de data invalido (use YYYY-MM-DD).\n";
            }
        }
        
        // Comandos de Escrita
        else if (cmd.rfind("add-task ", 0) == 0) {
            std::string title = cmd.substr(9);
            uint32_t id = taskMgr.addTask(title);
            std::cout << "[OK] Tarefa #" << id << " criada!\n";
            taskMgr.printTasks();
        } 
        else if (cmd.rfind("toggle ", 0) == 0) {
            uint32_t id = std::stoi(cmd.substr(7));
            if (taskMgr.toggleTask(id)) { taskMgr.printTasks(); } else { std::cout << "[ERRO] Nao encontrada.\n"; }
        } 
        else if (cmd.rfind("add-event ", 0) == 0) {
            std::string full = cmd.substr(10);
            size_t pipePos = full.find('|');
            if (pipePos != std::string::npos) {
                std::string title = full.substr(0, pipePos);
                std::string dateStr = full.substr(pipePos + 1);
                // Remove espaços extras ao redor do pipe
                while(!title.empty() && title.back() == ' ') title.pop_back();
                while(!dateStr.empty() && dateStr.front() == ' ') dateStr.erase(0, 1);
                
                SimDate date = parseDateStr(dateStr);
                uint32_t id = eventMgr.addEvent(title, date);
                if (id > 0) {
                    std::cout << "[OK] Evento #" << id << " criado!\n";
                    eventMgr.printEvents();
                } else {
                    std::cout << "[ERRO] Falha ao criar evento. Verifique a data.\n";
                }
            } else {
                std::cout << "[ERRO] Use o formato: add-event <titulo> | YYYY-MM-DD\n";
            }
        }
        
        else if (!cmd.empty()) { std::cout << "[ERRO] Comando nao reconhecido.\n"; }
    }
    return 0;
}