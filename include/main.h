
#ifndef MAIN_H
#define MAIN_H

// Підключення необхідних бібліотек Arduino
#include <Arduino.h>
#include <SPIFFS.h>         // Файлова система для зберігання даних у флеш-пам'яті
#include <ETH.h>            // Підтримка Ethernet
#include <Update.h>         // OTA (over-the-air) оновлення
#include <time.h>           // Робота з часом

// Бібліотеки для роботи з JSON, таймерами та асинхронним MQTT та веб-сервером
#include <ArduinoJson.h>    // Для роботи з JSON-структурами
#include <AsyncDelay.h>     // Таймер для відкладених операцій

#include <PsychicMqttClient.h>  // Ваш спеціальний MQTT-клієнт
#include <ESPAsyncWebServer.h>  // Асинхронний веб-сервер

// Бібліотека для роботи з GNSS
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

// Власні заголовки для роботи з JSON, послідовними портами та мережею
#include "json.h"
#include "network.h"
#include "serial.h"
#include "ADC.h"

// Використання послідовного порту для відладки
#define USE_SERIAL_DEBUG

// Визначаємо піни для Serial2 tsukorok
#define RXD2 5
#define TXD2 17

// Визначаємо піни для Serial1 GPS
#define RXD3 4
#define TXD3 2


// Оголошення зовнішніх змінних

//Об'єкт GPS
extern SFE_UBLOX_GNSS myGNSS; 
// MQTT-клієнт для обміну повідомленнями
extern PsychicMqttClient client;
// JSON-документ для обробки даних у форматі JSON
extern JsonDocument json, jsonHeartbeats;

// Структура з налаштуваннями MQTT
extern struct Config ConfigMQTT;
//Структура з даними Цукорка
extern struct tsukorokReadData tsukorokData;

// Таймер отримування даних Цукорка
extern AsyncDelay TSUKOROKreadDelay;
// Таймер відправки MQTT повідомлення
extern AsyncDelay MQTTsendDelay;
// Асинхронний веб-сервер на порту 80
extern AsyncWebServer server;

// Константи

// Шлях до конфігураційного файлу на файловій системі
extern const char* configFile;

// Зовнішні змінні
//Широта GPS
extern double latitude;
//Довгота GPS
extern double longitude;
//Заряд встроєної батареї
extern int8_t battery;

//Структура з даними Цукорка
struct tsukorokReadData {
    //Часова мітка Цукорка
    int timestampTsukorok;
    //Тип задетектченого дрону
    int8_t droneType;
    //Назва задетектченого дрону
    String droneName;
    //Сила сигналу задетектченого дрону
    int8_t RSSI;
    //Частота задетектченого дрону
    int frequency;
    //ID задетектченого дрону
    int droneID;
};

// Час між відправкою повідомлень Heartbeats
extern int timeBetweenHeartbeats;

// Статус підключення GPS
extern bool connectGPS;
// Статус підключення Цукорка
extern bool connectTsukorok;
// Статус виявлення дрона
extern bool detectDrone;


struct Config {
    // Ідентифікатор сенсора, за замовчуванням "TSUKOROK-000"
    String sensor_id = "TSUKOROK-000";
    // IP-адреса MQTT-брокера у форматі рядка
    String mqtt_ip_address = "193.168.22.252";
    // порт MQTT-брокера у форматі рядка
    String mqtt_port = "8083";
    // Налаштування MQTT брокера (вебсокет-з'єднання) 
    char mqtt_broker[100];
    // Топік для відправки повідомлення
    char topic_status[50];
    // Логін для MQTT
    const char *mqtt_username = "";
    // Пароль для MQTT
    const char *mqtt_password = "";
};


#endif // MAIN_H