
#include "json.h"

void createJsonDocument(JsonDocument &json){

  json["sensor_id"] = ConfigMQTT.sensor_id;
  json["timestamp"] = 0;

  JsonObject location = json["location"].to<JsonObject>();
  location["latitude"] = 0.0;
  location["longitude"] = 0.0;
  
  JsonObject tsukorokSensor = json["tsukorok_gsm_data"].to<JsonObject>();
  tsukorokSensor["status"] = "OFF";// active/OFF
  tsukorokSensor["device_timestamp"] = 0;
  tsukorokSensor["drone_type"] = 0;
  tsukorokSensor["drone_name"] = "NaN";
  tsukorokSensor["rssi"] = 0;
  tsukorokSensor["freq"] = 0.0;
  tsukorokSensor["drone_id"] = 0;

  JsonObject sensorData = json["WT32_data"].to<JsonObject>();
  sensorData["battery"] = 0;
  
}

void updateJsonDocument(JsonDocument &json) {
    time_t now;
    time(&now);  // Get the current time
    int currentTimestamp = static_cast<int>(now);  // Cast to int to match the JSON type

    json["timestamp"] = (String) currentTimestamp;

    json["location"]["latitude"] = latitude;
    json["location"]["longitude"] = longitude;

    if(connectTsukorok) json["tsukorok_gsm_data"]["status"] = "active";
    else json["tsukorok_gsm_data"]["status"] = "OFF";

    json["tsukorok_gsm_data"]["device_timestamp"] = tsukorokData.timestampTsukorok;
    json["tsukorok_gsm_data"]["drone_type"] = tsukorokData.droneType;
    json["tsukorok_gsm_data"]["drone_name"] = tsukorokData.droneName;
    json["tsukorok_gsm_data"]["rssi"] = tsukorokData.RSSI;
    json["tsukorok_gsm_data"]["freq"] = tsukorokData.frequency;
    json["tsukorok_gsm_data"]["drone_id"] = tsukorokData.droneID;

    json["WT32_data"]["battery"] = battery;
  
}

void createJsonDocumentHeartbeats(JsonDocument &json){

  json["sensor_id"] = ConfigMQTT.sensor_id;
  json["timestamp"] = 0;

  JsonObject location = json["location"].to<JsonObject>();
  location["latitude"] = 0.0;
  location["longitude"] = 0.0;

  JsonObject sensorData = json["WT32_data"].to<JsonObject>();
  sensorData["battery"] = 0;
  sensorData["tsukorok_status"] = "OFF";
  
}

void updateJsonDocumentHeartbeats(JsonDocument &json) {
    time_t now;
    time(&now);  // Get the current time
    int currentTimestamp = static_cast<int>(now);  // Cast to int to match the JSON type

    json["timestamp"] = (String) currentTimestamp;

    json["location"]["latitude"] = latitude;
    json["location"]["longitude"] = longitude;

    json["WT32_data"]["battery"] = battery;
    if(connectTsukorok) json["WT32_data"]["tsukorok_status"] = "active";
    else json["WT32_data"]["tsukorok_status"] = "OFF";
  
}

void sendJsonDataMQTT(JsonDocument &json, PsychicMqttClient &client, const char *topic) {

    // Виділити буфер для зберігання серіалізованого JSON
    size_t len = measureJson(json);
    char jsonBuffer[len + 1];

    // Серіалізувати JSON у масив символів з табуляцією
    serializeJson(json, jsonBuffer, len + 1);
    // Відправити JSON через MQTT
     client.publish(topic, 0, false, jsonBuffer, strlen(jsonBuffer), true);
}

bool loadConfig(const char* filename) {
    if (!SPIFFS.begin(true)) {
        #ifdef USE_SERIAL_DEBUG
        Serial.println("Failed to mount file system");
        #endif
        return false;
    }

    File file = SPIFFS.open(filename, FILE_READ);
    if (!file) {
        #ifdef USE_SERIAL_DEBUG
        Serial.println("Failed to open config file");
        #endif
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        #ifdef USE_SERIAL_DEBUG
        Serial.println("Failed to read file, using default configuration");
        #endif
        return false;
    }
    ConfigMQTT.mqtt_ip_address = doc["mqtt_ip_adress"].as<String>();
    ConfigMQTT.mqtt_port = doc["mqtt_port"].as<String>();
    ConfigMQTT.mqtt_path = doc["mqtt_path"].as<String>();
    ConfigMQTT.sensor_id = doc["sensor_id"].as<String>();
    latitude = doc["latitude"].as<double>();
    longitude = doc["longitude"].as<double>();
    timeBetweenHeartbeats = doc["time_between_heartbeats"].as<int>();

    timeBetweenHeartbeats = timeBetweenHeartbeats * 1000;
    timeBetweenHeartbeats = constrain(timeBetweenHeartbeats, 3000, 1800000);

    file.close();
    return true;
}
