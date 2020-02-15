#include <Arduino.h>
#include "KapLight.h"
#include "KapBeeper.h"
#include "KapSensor.h"
#include "KapMqtt.h"
#include "KapObjects.h"

KapLight::KapLight(int lightPin, KapObjects* kapObjects)
{
  pinMode(lightPin, OUTPUT);
  _kapObjects = kapObjects;
  _lightPin = lightPin;
  off();
}

void KapLight::on(unsigned long timeToGlow) {
  _timeToGlow = timeToGlow;
  _statusChangedTime = millis();
  _status = HIGH;
  _isBeeped = false;
  digitalWrite(_lightPin, HIGH);
  // delay(500);
}

void KapLight::off() {
  _status = LOW;
  _OffTime = millis();
  digitalWrite(_lightPin, LOW);
  if (_kapObjects->_sensor != NULL) {
    _kapObjects->_sensor->startIgnore(0);
  }
  // delay(500);
}

void KapLight::beep(int count) {
  if (_kapObjects->_beeper != NULL) {
     _kapObjects->_beeper->beep(count);
  }
}

void KapLight::process() {
  if (_status) {
    unsigned long time = millis();
    
    if(time > _statusChangedTime + _timeToGlow ||       //прошло миллисекунд с последнего движения
       time < _statusChangedTime && time > _timeToGlow  //произошло переполнение счетчика времени и с момента переполнения прошло миллисекунд
      ) {
      if (_status) {
        // с отправкой сообщения на сервер
        toggle(0);
      }
    } else if (!_isBeeped && 
               (time + _timeBeep > _statusChangedTime + _timeToGlow ||
                (time < _statusChangedTime) && (time + _isBeeped > _timeToGlow)
               )
              ) {
      _isBeeped = true;
      beep(3);
    }
  }
}

void KapLight::toggle(unsigned long timeToGlow) {
  if (_status) {
    off();
    _kapObjects->_mqtt->setStatus("OFF");
  } else {
    on(timeToGlow);
    _kapObjects->_mqtt->setStatus("ON");
  }
}

int KapLight::status() {
  return _status;
}

int KapLight::isBeeped() {
  return _isBeeped;
}

unsigned long KapLight::changeTime() {
  return _statusChangedTime;
}

unsigned long KapLight::timeToGlow() {
  return _timeToGlow;
}
