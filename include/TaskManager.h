#pragma once
#include "Task.h"
#include "StorageService.h"
#include <ArduinoJson.h>
#include <vector>

class TaskManager {
private:
    std::vector<Task> tasks;
    uint32_t nextId = 1;
    const char *filePath = "/tasks.json";

public:
    uint32_t addTask(const String &title, Priority priority = Priority::PRIORITY_MEDIUM) {
        tasks.emplace_back(nextId, title, priority);
        uint32_t createdId = nextId++;
        saveToFile();
        return createdId;
    }

    bool toggleTask(uint32_t id) {
        for (auto &task : tasks) {
            if (task.id == id) {
                task.completed = !task.completed;
                saveToFile();
                return true;
            }
        }
        return false;
    }

    bool saveToFile() {
        JsonDocument doc;
        JsonArray array = doc.to<JsonArray>();

        for (const auto &task : tasks) {
            JsonObject obj = array.add<JsonObject>();
            obj["id"] = task.id;
            obj["title"] = task.title;
            obj["completed"] = task.completed;
            obj["priority"] = static_cast<int>(task.priority);
        }

        String jsonOutput;
        serializeJson(doc, jsonOutput);
        return StorageService::writeString(filePath, jsonOutput);
    }

    bool loadFromFile() {
        String jsonInput = StorageService::readString(filePath);
        if (jsonInput.isEmpty()) {
            Serial.println("[TASK] Nenhum dado salvo encontrado. Lista inicializada vazia.");
            return false;
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonInput);
        if (error) {
            Serial.printf("[TASK] Falha ao ler JSON: %s\n", error.c_str());
            return false;
        }

        tasks.clear();
        uint32_t highestId = 0;

        for (JsonObject obj : doc.as<JsonArray>()) {
            uint32_t id = obj["id"];
            String title = obj["title"];
            Priority priority = static_cast<Priority>(obj["priority"] | 1);

            Task task(id, title, priority);
            task.completed = obj["completed"] | false;
            tasks.push_back(task);

            if (id > highestId) {
                highestId = id;
            }
        }

        nextId = highestId + 1;
        Serial.printf("[TASK] %u tarefas carregadas da Flash com sucesso.\n", tasks.size());
        return true;
    }

    const std::vector<Task>& getTasks() const {
        return tasks;
    }

    size_t count() const {
        return tasks.size();
    }
};