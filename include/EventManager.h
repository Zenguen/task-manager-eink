#pragma once
#include "Event.h"
#include "StorageService.h"
#include <ArduinoJson.h>
#include <vector>
#include <string>

class EventManager {
private:
    std::vector<Event> events;
    uint32_t nextId = 1;
    const char *filePath = "/events.json";

public:
    uint32_t addEvent(const std::string &title, SimpleDate date, bool isAnnual = false) {
        events.emplace_back(nextId, title, date, isAnnual);
        uint32_t createdId = nextId++;
        saveToFile();
        return createdId;
    }

    bool deleteEvent(uint32_t id) {
        for (auto it = events.begin(); it != events.end(); ++it) {
            if (it->id == id) {
                events.erase(it);
                saveToFile();
                return true;
            }
        }
        return false;
    }

    bool saveToFile() {
        JsonDocument doc;
        JsonArray array = doc.to<JsonArray>();

        for (const auto &e : events) {
            JsonObject obj = array.add<JsonObject>();
            obj["id"] = e.id;
            obj["title"] = e.title;
            
            // Salvando a data de forma estruturada
            obj["year"] = e.targetDate.year;
            obj["month"] = e.targetDate.month;
            obj["day"] = e.targetDate.day;
            obj["annual"] = e.annual;
        }

        String jsonOutput;
        serializeJson(doc, jsonOutput);
        return StorageService::writeString(filePath, jsonOutput);
    }

    bool loadFromFile() {
        String jsonInput = StorageService::readString(filePath);
        if (jsonInput.isEmpty()) {
            return false;
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonInput);
        if (error) {
            Serial.printf("[EVENT] Falha ao desserializar JSON: %s\n", error.c_str());
            return false;
        }

        events.clear();
        uint32_t highestId = 0;

        for (JsonObject obj : doc.as<JsonArray>()) {
            uint32_t id = obj["id"];
            std::string title = obj["title"].as<std::string>();
            int y = obj["year"];
            int m = obj["month"];
            int d = obj["day"];
            bool annual = obj["annual"] | false;

            events.emplace_back(id, title, SimpleDate(y, m, d), annual);
            if (id > highestId) highestId = id;
        }

        nextId = highestId + 1;
        Serial.printf("[EVENT] %u eventos carregados da Flash.\n", events.size());
        return true;
    }

    const std::vector<Event>& getEvents() const {
        return events;
    }

    size_t count() const {
        return events.size();
    }
};