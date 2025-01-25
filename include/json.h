#ifndef JSON_H 
#define JSON_H

// Підключення основного заголовкового файлу проекту
#include <main.h>


// Функція для створення JSON-документу з даними сенсора
void createJsonDocument(JsonDocument &json);

// Оновлення JSON-документу для змінення каналу
void updateJsonDocument(JsonDocument &json);

// Функція для створення Heartbeats JSON-документу з даними сенсора
void createJsonDocumentHeartbeats(JsonDocument &json);

// Оновлення Heartbeats JSON-документу для змінення каналу
void updateJsonDocumentHeartbeats(JsonDocument &json);

// Відправка JSON даних через MQTT
void sendJsonDataMQTT(JsonDocument &json, PsychicMqttClient &client, const char *topic);

// Завантаження конфігурації з файлу
bool loadConfig(const char* filename);

#endif // JSON_H