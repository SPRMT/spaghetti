#include <ADC.h>

uint8_t calculateBatteryPercentage(void) {
    uint16_t adcValue = analogRead(ADC_battery_pin);
    // Обмежуємо значення ADC в межах adcMin та adcMax
    adcValue = constrain(adcValue, ADC_battery_min, ADC_battery_max);
    // Розраховуємо відсоток заряду
    float percentage = (float)(adcValue - ADC_battery_min) / (ADC_battery_max - ADC_battery_min) * 100;
    // Заокруглення до найближчого кратного 5
    uint8_t roundedPercentage = round(percentage / 5) * 5;
    
    return roundedPercentage;
}