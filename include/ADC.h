#ifndef ADC_H 
#define ADC_H

// Підключення основного заголовкового файлу проекту
#include <main.h>


#define ADC_battery_pin 36

#define ADC_battery_min 2400 //21V
#define ADC_battery_max 3180 //26.9V

uint8_t calculateBatteryPercentage(void);

#endif // ADC_H
