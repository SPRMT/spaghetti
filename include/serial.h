#ifndef SERIAL_H 
#define SERIAL_H

// Підключення основного заголовкового файлу проекту
#include <main.h>

// Функція ініціалізації GPS
bool InitGPS(void);
// Функція зчитування даних GPS
void ReadGPS(void); 

// Функція ініціалізації Цукорка
bool InitTsukorok(void);
// Функція зчитування даних Цукорка
bool ReadTsukorok(void); 

#endif // SERIAL_H
