#include <Arduino.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "KapConfigStruct.h"
#include "KapConfig.h"
#include "KapObjects.h"

KapConfig::KapConfig() {
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  loadConfig();
}

void KapConfig::loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config.json");
  }

  String content = configFile.readString();
  // Serial.println("Config: " + content);
  configFile.close();

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, content);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
  }

  strlcpy(_configParams.ssidName, doc["ssidName"] | "", sizeof(_configParams.ssidName));
  strlcpy(_configParams.ssidPass, doc["ssidPass"] | "", sizeof(_configParams.ssidPass));
  strlcpy(_configParams.mqttServer, doc["mqttServer"] | "", sizeof(_configParams.mqttServer));
  strlcpy(_configParams.mqttLogin, doc["mqttLogin"] | "", sizeof(_configParams.mqttLogin));
  strlcpy(_configParams.mqttPass, doc["mqttPass"] | "", sizeof(_configParams.mqttPass));
  _configParams.noSensorFromH = doc["noSensorFromH"] | 23;
  _configParams.noSensorFromM = doc["noSensorFromM"] | 0;
  _configParams.noSensorToH = doc["noSensorToH"] | 7;
  _configParams.noSensorToM = doc["noSensorToM"] | 0;
}

void KapConfig::setSsid(const char* ssid, const char* pass) {
  strlcpy(_configParams.ssidName, ssid, sizeof(_configParams.ssidName));
  strlcpy(_configParams.ssidPass, pass, sizeof(_configParams.ssidPass));
}
void KapConfig::setMqtt(const char* mqtt, const char* login, const char* pass) {
  strlcpy(_configParams.mqttServer, mqtt, sizeof(_configParams.mqttServer));
  strlcpy(_configParams.mqttLogin, pass, sizeof(_configParams.mqttLogin));
  strlcpy(_configParams.mqttPass, pass, sizeof(_configParams.mqttPass));
}

bool KapConfig::saveConfig() {
  // Open config file for writing.
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config.json for writing");
    return false;
  }

  StaticJsonDocument<512> doc;

  doc["ssidName"] = _configParams.ssidName;
  doc["ssidPass"] = _configParams.ssidPass;
  doc["mqttServer"] = _configParams.mqttServer;
  doc["mqttLogin"] = _configParams.mqttLogin;
  doc["mqttPass"] = _configParams.mqttPass;
  doc["noSensorFromH"] = _configParams.noSensorFromH;
  doc["noSensorFromM"] = _configParams.noSensorFromM;
  doc["noSensorToH"] = _configParams.noSensorToH;
  doc["noSensorToM"] = _configParams.noSensorToM;

  if (serializeJson(doc, configFile) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  
  configFile.close();

  return true;
}

KapConfigParams* KapConfig::getConfig() {
  return &_configParams;
}
