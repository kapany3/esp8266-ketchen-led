#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include "KapObjects.h"

#ifndef KapMqtt_h
#define KapMqtt_h

class KapMqtt
{
  public:
    KapMqtt(KapObjects* kapObjects);
    void setStatus(const char*);
    void setInfo(const char*);
    void setTime(const char*);
    void process();
    void statusCb(char *data, uint16_t len);
  private:
    bool _canConnect = false;
    KapObjects* _kapObjects = NULL;
    unsigned long _connTime = 0;
    unsigned long _pingTime = 0;
    unsigned long _procTime = 0;
    WiFiClientSecure* _client = NULL;
    Adafruit_MQTT_Client* _mqtt = NULL;
    Adafruit_MQTT_Publish* _ledStatusPub = NULL;
    Adafruit_MQTT_Subscribe* _ledStatusSub = NULL;
    Adafruit_MQTT_Publish* _infoPub = NULL;
    Adafruit_MQTT_Publish* _timePub = NULL;
    bool connectMqtt();
};

#endif
