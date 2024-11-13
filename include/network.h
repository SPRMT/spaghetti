#ifndef NETWORK_H
#define NETWORK_H

#include <main.h>

// Спільні стилі для всіх сторінок
extern const char* const commonStyles;
// HTML для головної сторінки
extern const char* const indexPageTemplate;
// HTML шаблон для сторінок відповіді
extern const char* const responsePageTemplate;

// Прототипи функцій
void onMqttConnect(bool sessionPresent);
void onMqttMessage( char* topic, char* payload, int retain, int qos, bool dup);

// Обробка подій WiFi
void WiFiEvent(WiFiEvent_t event);

// Налаштування OTA
void setupWebOTA();


#endif // NETWORK_H