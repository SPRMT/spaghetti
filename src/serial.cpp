#include "serial.h"

bool InitGPS(void)
{
  // Підключаємо GNSS модуль
  if (myGNSS.begin(Serial1) == false)
  {
    Serial.println(F("u-blox GNSS module not detected. Please check wiring."));
    Serial.println("InitGPS: ERROR");
    return false;
  }
  else { 
    myGNSS.setAutoPVT(true); // Налаштовуємо модуль на автоматичне оновлення даних
    Serial.println("InitGPS: SUCCESS");
    return true;
  }
}

void ReadGPS(void)
{
    // Оновлюємо дані з модуля
  myGNSS.checkUblox();

  // Перевіряємо, чи є нові дані про позицію
  if (myGNSS.getPVT())
  {
    // Зчитуємо широту та довготу 
     latitude  = ((myGNSS.getLatitude())/ 10000000.0);  // Перетворюємо в градуси
     longitude = ((myGNSS.getLongitude())/ 10000000.0); // Перетворюємо в градуси
     
  }
}

bool InitTsukorok(void)
{
  String csvCommand = "machine readable 1;";
    
  while(Serial2.available()) { Serial2.read(); }

  Serial2.println(csvCommand);
    
  // Чекаємо на відповідь (timeout 1 секунда)
  unsigned long startTime = millis();
  String response = "";
    
  while (millis() - startTime < 1000) {
      if (Serial2.available()) {
          char c = Serial2.read();
          response += c;
            
          // Перевіряємо, чи містить відповідь потрібний текст
          if (response.indexOf("machine readable csv") != -1) {
              Serial.println("InitTsukorok: SUCCESS");
              return true;
          }
      }
  }
  Serial.println("InitTsukorok: ERROR");
  return false;
}

bool ReadTsukorok(void) {
  String inputString = "";
    
  if (!Serial2.available()) return false;

  inputString = Serial2.readStringUntil('\n');
  Serial.println(inputString);
    
  if (!inputString.startsWith("detect")) return false;
    
  String containers[10];
    
  int containerIndex = 0;
  int startIndex = 0;
  int semicolonIndex;
    
  while ((semicolonIndex = inputString.indexOf(';', startIndex)) != -1 && containerIndex < 10) {
      containers[containerIndex] = inputString.substring(startIndex, semicolonIndex);
      startIndex = semicolonIndex + 1;
      containerIndex++;
  }
    
  if (containerIndex < 10) {
    containers[containerIndex] = inputString.substring(startIndex);
    containerIndex++;
  }
    
  if (containerIndex != 10) return false;
    
  int spaceIndex = containers[6].indexOf(' ');
  if (spaceIndex != -1) {
    tsukorokData.droneName = containers[6].substring(0, spaceIndex - 1);
  } else {
    tsukorokData.droneName = containers[6];
    }
  
  tsukorokData.timestampTsukorok = containers[4].toInt();
  tsukorokData.droneType = containers[5].toInt();
  tsukorokData.RSSI = containers[7].toInt();  
  tsukorokData.frequency = containers[8].toInt();
  tsukorokData.droneID = containers[9].toInt();
    
  return true;
}