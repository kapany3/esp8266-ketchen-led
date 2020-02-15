#include <Arduino.h>
#include "KapSensor.h"
#include "KapBeeper.h"
#include "KapLight.h"
#include "KapMqtt.h"
#include "KapTime.h"
#include "KapObjects.h"


KapSensor::KapSensor(int pin, unsigned long timeSensor, KapObjects* kapObjects)
{
  pinMode(pin, INPUT);
  _pin = pin;
  _timeSensor = timeSensor;
  _kapObjects = kapObjects;

  // Игнорировать сеноср сразу после старта
  _ignoreStarted = millis();
}

void KapSensor::process(){
  int state = digitalRead(_pin);
  unsigned long time;
  if(state) {
    unsigned long time = millis();
    if (time - _ignoreStarted > 10000 || time < _ignoreStarted) {
      bool status = _kapObjects->_light->status();
      bool isBeeped = false;
      if (!status) {
        // Свет не горел, включить с информированием mqtt
        _kapObjects->_light->toggle(_timeSensor);
      } else {
        // Свет горел, начать новый отсчет таймера, запомнить статус пика, т.к он похерится
        isBeeped = _kapObjects->_light->isBeeped();
        _kapObjects->_light->on(_timeSensor);
      }
      
      if ((!status || isBeeped) && _kapObjects->_beeper != NULL) {
        // Если лампа не горела или был пик о скором выключении
        _kapObjects->_beeper->beep(1);
      }
      
      if (!status && _kapObjects->_mqtt != NULL && _kapObjects->_time != NULL) {
        // Если не был включен, то информировать об испольовании сенсора
        char* tm = _kapObjects->_time->getTime();
        char comb[80];
        sprintf(comb, "Sensor: %s", tm);
        _kapObjects->_mqtt->setInfo(comb);
      }

      // Игнорировать дальнейшие сигналы от таймера  10 секунд
      startIgnore(time);
    }
  }
}

void KapSensor::startIgnore(unsigned long now) {
  _ignoreStarted = now == 0 ? millis() : now;
}
