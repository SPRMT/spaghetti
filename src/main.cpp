#include <main.h>

SFE_UBLOX_GNSS myGNSS; 
PsychicMqttClient client;
JsonDocument json, jsonHeartbeats;
AsyncWebServer server(80);

AsyncDelay TSUKOROKreadDelay;
AsyncDelay MQTTsendDelay;
Config ConfigMQTT;
tsukorokReadData tsukorokData;

const char* configFile = "/sprmt_config.json";


double latitude, longitude;

int8_t battery;

int timeBetweenHeartbeats = 30000;

bool connectGPS, connectTsukorok, detectDrone;

void setup()
{
  String temp;

  pinMode(ADC_battery_pin, INPUT);

  Serial.begin(115200); // Ініціалізуємо основний Serial для виведення даних
  Serial1.begin(115200, SERIAL_8N1, RXD3, TXD3); // Ініціалізуємо Serial1 для зв'язку з NEO-M9N
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // Ініціалізуємо Serial2 для зв'язку з цукорком

   WiFi.onEvent(WiFiEvent);

  loadConfig(configFile);

  ETH.begin();

  setupWebOTA();
  
  createJsonDocument(json);
  createJsonDocumentHeartbeats(jsonHeartbeats);

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  while (!time(nullptr)) {}
  
  temp =  "ws://" + ConfigMQTT.mqtt_ip_address + ":" + ConfigMQTT.mqtt_port + "/mqtt";
  temp.toCharArray(ConfigMQTT.mqtt_broker, temp.length() + 1);

  temp = "tsukorok/" + ConfigMQTT.sensor_id;
  temp.toCharArray(ConfigMQTT.topic_status, temp.length() + 1);
  
  client.onConnect(onMqttConnect);
  client.onMessage(onMqttMessage);
  client.setServer(ConfigMQTT.mqtt_broker);

  
  while (!client.connected()) {
    #ifdef USE_SERIAL_DEBUG
      Serial.println("MQTT connection failed, retrying...");
    #endif
    client.connect();
    delay(3000);
  }
  

  connectGPS = InitGPS();
  connectTsukorok = InitTsukorok();


  TSUKOROKreadDelay.start(1000, AsyncDelay::MILLIS);
  MQTTsendDelay.start(timeBetweenHeartbeats, AsyncDelay::MILLIS);
  MQTTsendDelay.expire();

}

void loop()
{
  if(MQTTsendDelay.isExpired()){ // Відправляємо дані на MQTT сервер

    battery = calculateBatteryPercentage();

    if(connectGPS) ReadGPS();
    else InitGPS();

    updateJsonDocumentHeartbeats(jsonHeartbeats);
    sendJsonDataMQTT(jsonHeartbeats, client, ConfigMQTT.topic_status);
    connectTsukorok = InitTsukorok();
    MQTTsendDelay.repeat();
  }

  if(TSUKOROKreadDelay.isExpired()){ // Читаємо дані Цукорка

    if(connectTsukorok) detectDrone = ReadTsukorok();
    else InitTsukorok();

    if(detectDrone)
    {
      ReadGPS();
      updateJsonDocument(json);
      sendJsonDataMQTT(json, client, ConfigMQTT.topic_status);
      MQTTsendDelay.repeat();
      detectDrone = false;
      
    }
    else TSUKOROKreadDelay.repeat();
  }

}
