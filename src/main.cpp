#include <Arduino.h>
#include "StorageService.h"
#include "TaskManager.h"
#include "NoteManager.h"

TaskManager taskMgr;
NoteManager noteMgr;

void setup() {
    Serial.begin(115200);
    delay(1500);

    StorageService::init();

    // Carrega os dois módulos de arquivos separados na Flash
    taskMgr.loadFromFile();
    noteMgr.loadFromFile();

    if (noteMgr.count() == 0) {
        noteMgr.addNote("Ideias de Layout", "Usar fonte 16px para listas e 24px para o relogio.");
        noteMgr.addNote("Atalhos do Jog Dial", "Girar: rolar itens. Clicar: selecionar.");
    }

    Serial.println("\n=== STATUS DO SISTEMA ===");
    Serial.printf("Tarefas ativas: %u\n", taskMgr.count());
    Serial.printf("Notas salvas:   %u\n", noteMgr.count());
}

void loop() {
    delay(5000);
}