#pragma once
#include "Task.h"
#include "StorageService.h"
#include <ArduinoJson.h>
#include <vector>
#include <string>

class TaskManager {
private:
    std::vector<Task> tasks;
    uint32_t nextId = 1;
    const char *filePath = "/tasks.json";

public:
    uint32_t addTask(const std::string &title) {
        tasks.emplace_back(nextId, title);
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

    bool deleteTask(uint32_t id) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if (it->id == id) {
                tasks.erase(it);
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
            obj["title"] = task.title; // ArduinoJson converte std::string sozinho aqui
            obj["completed"] = task.completed;
            obj["priority"] = static_cast<int>(task.priority);
        }

        String jsonOutput; // Mantemos o String do Arduino aqui pro StorageService
        serializeJson(doc, jsonOutput);
        return StorageService::writeString(filePath, jsonOutput);
    }

    bool loadFromFile() {
        String jsonInput = StorageService::readString(filePath);
        if (jsonInput.isEmpty()) return false;

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonInput);
        if (error) {
            Serial.printf("[TASK] Falha ao desserializar JSON: %s\n", error.c_str());
            return false;
        }

        tasks.clear();
        uint32_t highestId = 0;

        for (JsonObject obj : doc.as<JsonArray>()) {
            uint32_t id = obj["id"];
            // Aqui forçamos a leitura como std::string nativa do C++
            std::string title = obj["title"].as<std::string>(); 
            bool completed = obj["completed"] | false;
            int prioInt = obj["priority"] | static_cast<int>(Priority::PRIORITY_MEDIUM);

            Task t(id, title, static_cast<Priority>(prioInt));
            t.completed = completed;
            tasks.push_back(t);

            if (id > highestId) highestId = id;
        }

        nextId = highestId + 1;
        Serial.printf("[TASK] %u tarefas carregadas.\n", tasks.size());
        return true;
    }

    const std::vector<Task>& getTasks() const { return tasks; }
    size_t count() const { return tasks.size(); }
};