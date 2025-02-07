#include <network.h>

void onMqttConnect(bool sessionPresent) {
    #ifdef USE_SERIAL_DEBUG
      Serial.println("Connected to MQTT.");
    #endif
    client.subscribe(ConfigMQTT.topic_detect, 0);
    client.subscribe(ConfigMQTT.topic_status, 0);
}

void onMqttMessage( char* topic, char* payload, int retain, int qos, bool dup) {
  int payloadLength;
  JsonDocument doc;
  payloadLength = strlen(payload);
  deserializeJson(doc, payload, payloadLength);

    #ifdef USE_SERIAL_DEBUG
        Serial.println(topic);
        Serial.print("Message:");
        for (int i = 0; i < payloadLength; i++) {
            Serial.print((char)payload[i]);
        }
        Serial.println();
        Serial.println("-----------------------"); 
    #endif
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      #ifdef USE_SERIAL_DEBUG
        Serial.println("ETH Started");
      #endif
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      #ifdef USE_SERIAL_DEBUG
        Serial.println("ETH Connected");
      #endif
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      #ifdef USE_SERIAL_DEBUG
        Serial.print("ETH MAC: ");
        Serial.print(ETH.macAddress());
        Serial.print(", IPv4: ");
        Serial.print(ETH.localIP());
        if (ETH.fullDuplex()) {
          Serial.print(", FULL_DUPLEX");
        }
        Serial.print(", ");
        Serial.print(ETH.linkSpeed());
        Serial.println("Mbps");
      #endif
      //eth_connected = true;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      #ifdef USE_SERIAL_DEBUG
        Serial.println("ETH Disconnected");
      #endif
      //eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      #ifdef USE_SERIAL_DEBUG
        Serial.println("ETH Stopped");
      #endif
      //eth_connected = false;
      break;
    default:
      break;
  }
}

void setupWebOTA() {
  // Main page with forms for uploading firmware and configuration
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<!DOCTYPE html><html><head>"
                  "<style>"
                  "body { background-color:rgb(56, 49, 145); color: white; font-family: Arial, sans-serif; text-align: center; }"
                  "h1 { margin-top: 50px; }"
                  "form { margin: 20px auto; display: inline-block; }"
                  "input[type='file'], input[type='submit'] { padding: 10px; margin: 5px; }"
                  "footer { margin-top: 50px; font-size: 32px; }"
                  "a.button { display: inline-block; padding: 10px 20px; margin: 10px; color: white; background-color:rgb(76, 6, 133); text-decoration: none; border-radius: 5px; font-size: 16px; font-weight: bold; }"
                  "</style>"
                  "</head><body>"
                  "<h1>FIRMWARE UPDATE</h1>"
                  "<p>MQTT Broker: <strong>" + (String) ConfigMQTT.mqtt_broker + "</strong></p>"
                  "<p>Sensor ID: <strong>" + ConfigMQTT.sensor_id + "</strong></p>"
                  "<p>FIRMWARE VERSION: <strong>" + FIRMWARE_VERSION + "</strong></p>"
                  "<form method='POST' action='/update' enctype='multipart/form-data'>"
                  "<input type='file' name='update'><input type='submit' value='Update Firmware'></form><br><br>"
                  "<h1>CONFIGURATION UPDATE</h1>"
                  "<form method='POST' action='/upload_config' enctype='multipart/form-data'>"
                  "<input type='file' name='config'><input type='submit' value='Upload Configuration'></form>"
                  "<footer>Spaghetti DEV</footer>"
                  "</body></html>";
    request->send(200, "text/html", html);
  });

  // Handle firmware update with styled response page
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
    String response = "<!DOCTYPE html><html><head>"
                      "<style>"
                      "body { background-color: rgb(56, 49, 145); color: white; font-family: Arial, sans-serif; text-align: center; padding-top: 50px; }"
                      "a.button { display: inline-block; padding: 10px 20px; margin: 10px; color: white; background-color: rgb(76, 6, 133); text-decoration: none; border-radius: 5px; font-size: 16px; font-weight: bold; }"
                      "</style></head><body>"
                      "<h2>Update Successful! Rebooting...</h2><br><a class='button' href='/'>Return to Home page</a>"
                      "</body></html>";
    if (Update.hasError()) {
      response = "<!DOCTYPE html><html><head>"
                 "<style>"
                 "body { background-color: rgb(56, 49, 145); color: white; font-family: Arial, sans-serif; text-align: center; padding-top: 50px; }"
                 "a.button { display: inline-block; padding: 10px 20px; margin: 10px; color: white; background-color: rgb(76, 6, 133); text-decoration: none; border-radius: 5px; font-size: 16px; font-weight: bold; }"
                 "</style></head><body>"
                 "<h2>Update Failed!</h2><br><a class='button' href='/'>Return to Home page</a>"
                 "</body></html>";
    }
    request->send(200, "text/html", response);
    delay(1000);
    ESP.restart();
  }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      #ifdef USE_SERIAL_DEBUG
        Serial.printf("Update Start: %s\n", filename.c_str());
      #endif
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        #ifdef USE_SERIAL_DEBUG
          Update.printError(Serial);
        #endif
      }
    }
    if (!Update.hasError()) {
      if (Update.write(data, len) != len) {
        #ifdef USE_SERIAL_DEBUG
          Update.printError(Serial);
        #endif
      }
    }
    if (final) {
      if (Update.end(true)) {
        #ifdef USE_SERIAL_DEBUG
          Serial.printf("Update Success: %u bytes\n", index + len);
        #endif
      } else {
        #ifdef USE_SERIAL_DEBUG
          Update.printError(Serial);
        #endif
      }
    }
  });

  // Handle configuration file upload with styled response page
  server.on("/upload_config", HTTP_POST, [](AsyncWebServerRequest *request) {
    String response = "<!DOCTYPE html><html><head>"
                      "<style>"
                      "body { background-color: rgb(56, 49, 145); color: white; font-family: Arial, sans-serif; text-align: center; padding-top: 50px; }"
                      "a.button { display: inline-block; padding: 10px 20px; margin: 10px; color: white; background-color: rgb(76, 6, 133); text-decoration: none; border-radius: 5px; font-size: 16px; font-weight: bold; }"
                      "</style></head><body>"
                      "<h2>Configuration Upload Successful! Rebooting...</h2><br><a class='button' href='/'>Return to Home page</a>"
                      "</body></html>";
    request->send(200, "text/html", response);
    delay(1000);
    ESP.restart();
  }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      #ifdef USE_SERIAL_DEBUG
        Serial.printf("Config Upload Start: %s\n", filename.c_str());
      #endif
      if (!SPIFFS.begin(true)) {
        #ifdef USE_SERIAL_DEBUG
          Serial.println("Failed to mount file system");
        #endif
        return;
      }
      File file = SPIFFS.open(configFile, FILE_WRITE);
      if (!file) {
        #ifdef USE_SERIAL_DEBUG
          Serial.println("Failed to open config file for writing");
        #endif
        return;
      }
      file.write(data, len);
      file.close();
    } else {
      File file = SPIFFS.open(configFile, FILE_APPEND);
      file.write(data, len);
      file.close();
    }
    if (final) {
      #ifdef USE_SERIAL_DEBUG
        Serial.printf("Config Upload Success: %u bytes\n", index + len);
      #endif
    }
  });

  // Start server
  server.begin();
}