#pragma once
#include <Arduino.h>
#include <LittleFS.h>

class StorageService {
public:
    static bool init() {
        // Inicializa o sistema de arquivos; formata automaticamente se for a primeira execução
        if (!LittleFS.begin(true)) {
            Serial.println("[STORAGE] Erro ao montar LittleFS!");
            return false;
        }
        Serial.println("[STORAGE] LittleFS montado com sucesso.");
        return true;
    }

    static bool writeString(const char *path, const String &data) {
        File file = LittleFS.open(path, "w");
        if (!file) {
            Serial.printf("[STORAGE] Falha ao abrir %s para escrita.\n", path);
            return false;
        }
        file.print(data);
        file.close();
        return true;
    }

    static String readString(const char *path) {
        if (!LittleFS.exists(path)) {
            return "";
        }
        File file = LittleFS.open(path, "r");
        if (!file) {
            Serial.printf("[STORAGE] Falha ao abrir %s para leitura.\n", path);
            return "";
        }
        String data = file.readString();
        file.close();
        return data;
    }
};