#pragma once
#include "Note.h"
#include "StorageService.h"
#include <ArduinoJson.h>
#include <vector>
#include <string>

class NoteManager {
private:
    std::vector<Note> notes;
    uint32_t nextId = 1;
    const char *filePath = "/notes.json";

public:
    uint32_t addNote(const std::string &title, const std::string &content = "") {
        notes.emplace_back(nextId, title, content);
        uint32_t createdId = nextId++;
        saveToFile();
        return createdId;
    }

    bool updateNote(uint32_t id, const std::string &newTitle, const std::string &newContent) {
        for (auto &note : notes) {
            if (note.id == id) {
                note.title = newTitle;
                note.content = newContent;
                saveToFile();
                return true;
            }
        }
        return false;
    }

    bool deleteNote(uint32_t id) {
        for (auto it = notes.begin(); it != notes.end(); ++it) {
            if (it->id == id) {
                notes.erase(it);
                saveToFile();
                return true;
            }
        }
        return false;
    }

    const Note* getNote(uint32_t id) const {
        for (const auto &note : notes) {
            if (note.id == id) return &note;
        }
        return nullptr;
    }

    bool saveToFile() {
        JsonDocument doc;
        JsonArray array = doc.to<JsonArray>();

        for (const auto &note : notes) {
            JsonObject obj = array.add<JsonObject>();
            obj["id"] = note.id;
            obj["title"] = note.title;
            obj["content"] = note.content;
        }

        String jsonOutput;
        serializeJson(doc, jsonOutput);
        return StorageService::writeString(filePath, jsonOutput);
    }

    bool loadFromFile() {
        String jsonInput = StorageService::readString(filePath);
        if (jsonInput.isEmpty()) return false;

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonInput);
        if (error) {
            Serial.printf("[NOTE] Falha: %s\n", error.c_str());
            return false;
        }

        notes.clear();
        uint32_t highestId = 0;

        for (JsonObject obj : doc.as<JsonArray>()) {
            uint32_t id = obj["id"];
            std::string title = obj["title"].as<std::string>();
            std::string content = obj["content"] | "";

            notes.emplace_back(id, title, content);
            if (id > highestId) highestId = id;
        }

        nextId = highestId + 1;
        Serial.printf("[NOTE] %u notas carregadas.\n", notes.size());
        return true;
    }

    const std::vector<Note>& getNotes() const { return notes; }
    size_t count() const { return notes.size(); }
};