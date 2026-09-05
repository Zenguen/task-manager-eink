#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

// ==================== DOMÍNIO DE TAREFAS ====================
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
        for (auto &t : tasks) {
            if (t.id == id) {
                t.completed = !t.completed;
                return true;
            }
        }
        return false;
    }

    void printTasks() const {
        std::cout << "\n--- LISTA DE TAREFAS ---\n";
        std::cout << "ID  | Status | Titulo\n";
        std::cout << "----+--------+----------------------------\n";
        if (tasks.empty()) {
            std::cout << "(Nenhuma tarefa cadastrada)\n";
        } else {
            for (const auto &t : tasks) {
                std::cout << "#" << t.id << "  |  [" 
                          << (t.completed ? 'X' : ' ') << "]   | " 
                          << t.title << "\n";
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

    Note(uint32_t id, const std::string &title, const std::string &content = "")
        : id(id), title(title), content(content) {}
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
        for (auto it = notes.begin(); it != notes.end(); ++it) {
            if (it->id == id) {
                notes.erase(it);
                return true;
            }
        }
        return false;
    }

    void printNotes() const {
        std::cout << "\n--- BLOCO DE NOTAS ---\n";
        if (notes.empty()) {
            std::cout << "(Nenhuma nota cadastrada)\n";
        } else {
            for (const auto &n : notes) {
                std::cout << "[Nota #" << n.id << "] " << n.title << "\n";
                if (!n.content.empty()) {
                    std::cout << "  > " << n.content << "\n";
                }
            }
        }
        std::cout << "----------------------\n\n";
    }
};

// ==================== PROGRAMA PRINCIPAL ====================
int main() {
    TaskManagerDesktop taskMgr;
    NoteManagerDesktop noteMgr;
    std::string cmd;

    // Dados iniciais de teste
    taskMgr.addTask("Validar firmware no ESP32");
    noteMgr.addNote("Ideia de Layout", "Fonte 16px para itens de lista");

    std::cout << "=== SIMULADOR DE MÓDULOS (DESKTOP) ===\n";
    std::cout << "Comandos disponiveis:\n";
    std::cout << "  tasks                   -> Lista tarefas\n";
    std::cout << "  add-task <titulo>       -> Cria tarefa\n";
    std::cout << "  toggle <id>             -> Alterna status da tarefa\n";
    std::cout << "  notes                   -> Lista notas\n";
    std::cout << "  add-note <tit> | <conteudo> -> Cria nota (use | para separar)\n";
    std::cout << "  del-note <id>           -> Apaga nota\n";
    std::cout << "  exit                    -> Sair\n\n";

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, cmd) || cmd == "exit") break;

        if (cmd == "tasks") {
            taskMgr.printTasks();
        } 
        else if (cmd.rfind("add-task ", 0) == 0) {
            std::string title = cmd.substr(9);
            uint32_t id = taskMgr.addTask(title);
            std::cout << "[OK] Tarefa #" << id << " criada!\n";
            taskMgr.printTasks();
        } 
        else if (cmd.rfind("toggle ", 0) == 0) {
            uint32_t id = std::stoi(cmd.substr(7));
            if (taskMgr.toggleTask(id)) {
                std::cout << "[OK] Status da tarefa atualizado!\n";
                taskMgr.printTasks();
            } else {
                std::cout << "[ERRO] Tarefa nao encontrada.\n";
            }
        } 
        else if (cmd == "notes") {
            noteMgr.printNotes();
        } 
        else if (cmd.rfind("add-note ", 0) == 0) {
            std::string full = cmd.substr(9);
            size_t pipePos = full.find('|');
            std::string title, content;

            if (pipePos != std::string::npos) {
                title = full.substr(0, pipePos);
                content = full.substr(pipePos + 1);
            } else {
                title = full;
            }

            uint32_t id = noteMgr.addNote(title, content);
            std::cout << "[OK] Nota #" << id << " criada!\n";
            noteMgr.printNotes();
        } 
        else if (cmd.rfind("del-note ", 0) == 0) {
            uint32_t id = std::stoi(cmd.substr(9));
            if (noteMgr.deleteNote(id)) {
                std::cout << "[OK] Nota #" << id << " removida!\n";
                noteMgr.printNotes();
            } else {
                std::cout << "[ERRO] Nota nao encontrada.\n";
            }
        } 
        else if (!cmd.empty()) {
            std::cout << "[ERRO] Comando nao reconhecido.\n";
        }
    }

    return 0;
}