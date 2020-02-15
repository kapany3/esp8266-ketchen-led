#include <Arduino.h>
#include "KapObjects.h"

#ifndef KapSensor_h
#define KapSensor_h

class KapSensor
{
  public:
    KapSensor(int pin, unsigned long timeSensor, KapObjects*);
    void process();
    void startIgnore(unsigned long now);
  private:
    KapObjects* _kapObjects;
    int _pin;
    unsigned long _timeSensor;
    unsigned long _ignoreStarted;
};

#endif
