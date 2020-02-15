#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef KapConfigStruct_h
#define KapConfigStruct_h

typedef struct{
  char ssidName[32];
  char ssidPass[32];
  char mqttServer[32];
  char mqttLogin[32];
  char mqttPass[32];
  int noSensorFromH;
  int noSensorFromM;
  int noSensorToH;
  int noSensorToM;
}  KapConfigParams;

#endif
