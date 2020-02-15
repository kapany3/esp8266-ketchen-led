#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include "KapMqtt.h"
#include "KapLight.h"
#include "KapConfig.h"
#include "KapConfigStruct.h"

KapMqtt* kapMqttInstance = NULL;

void staticStatusCb(char *str, uint16_t len) {
  kapMqttInstance->statusCb(str, len);
}


KapMqtt::KapMqtt(KapObjects* kapObjects)
{
  _kapObjects = kapObjects;
  kapMqttInstance = this;
  KapConfigParams* conf = _kapObjects->_config->getConfig();

  if (conf->mqttServer == "") {
    return;
  }
  _canConnect = true;
  
  _client = new WiFiClientSecure();
  _client->setInsecure();
  _mqtt = new Adafruit_MQTT_Client(_client, conf->mqttServer, 8883, conf->mqttLogin, conf->mqttPass);
  _infoPub = new Adafruit_MQTT_Publish(_mqtt, "kitchenLed/info");
  _timePub = new Adafruit_MQTT_Publish(_mqtt, "kitchenLed/time");
  _ledStatusPub = new Adafruit_MQTT_Publish(_mqtt, "kitchenLed/status");
  _ledStatusSub = new Adafruit_MQTT_Subscribe(_mqtt, "kitchenLed/status");
  _ledStatusSub->setCallback(staticStatusCb);
  _mqtt->subscribe(_ledStatusSub);
}

void KapMqtt::process() {
  if (_canConnect && connectMqtt()) {
    unsigned long now = millis();
    if (_procTime + 1000 < now || now < _procTime) {
      _mqtt->processPackets(10);
      _procTime = now;
    }
    if (_pingTime + 60000 < now || now < _pingTime) {
      Serial.println("Mqtt ping");
      _pingTime = now;
      int fail = 0;
      while(!_mqtt->ping()) {
        fail += 1;
        Serial.print("Mqtt ping failed: ");
        Serial.println(fail);
        if (fail > 2) {
          Serial.println("Mqtt disconnect");
          _mqtt->disconnect();
          return;
        }
      }
    }
  }
}

void KapMqtt::setStatus(const char* status) {
  if (!_ledStatusPub->publish(status)) {
    Serial.println(F("Mqtt failed publish status"));
  } else {
    Serial.println(F("Mqtt status published successful"));
  }
}

void KapMqtt::setInfo(const char* info) {
  if (!_infoPub->publish(info)) {
    Serial.println(F("Mqtt failed publish info"));
  }
}

void KapMqtt::setTime(const char* time) {
  if (!_timePub->publish(time)) {
    Serial.println(F("Mqtt failed publish time"));
  }
}

bool KapMqtt::connectMqtt() {
  int8_t ret;

  if (_mqtt->connected()) {
    return true;
  }

  unsigned long now = millis();
  if (_connTime + 5000 < now || now < _connTime) {
    Serial.println("Connecting to MQTT... ");
    if((ret = _mqtt->connect()) != 0) { // connect will return 0 for connected
       Serial.println(_mqtt->connectErrorString(ret));
       _mqtt->disconnect();
       _connTime = millis();
       return false;
    }
    Serial.println("MQTT Connected!");
    return true;
  }

  return false;
}

void KapMqtt::statusCb(char *str, uint16_t len) {
  int status = _kapObjects->_light->status();
  if ((strcmp(str, "ON") == 0) && (status == LOW)) {
    _kapObjects->_light->on(300000);
  } else if ((strcmp(str, "OFF") == 0) && (status == HIGH)) {
    _kapObjects->_light->off();
  } else {
    Serial.print("Mqtt status callback: ");
    Serial.println(str);
  }
}
