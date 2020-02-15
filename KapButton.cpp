#include <Arduino.h>
#include "KapButton.h"
#include "KapBeeper.h"
#include "KapLight.h"
#include "KapObjects.h"


KapButton::KapButton(int pin, void (*clickFunc)(byte, unsigned long), KapObjects* kapObjects)
{
  pinMode(pin, INPUT);
  _pin = pin;
  _clickFunc = clickFunc;
  _kapObjects = kapObjects;
}

void KapButton::process(){
  int state = digitalRead(_pin);
  unsigned long time;
  if (_start > 0) {
    // Было нажатие
    time = millis();
    if (/*time - _start > 300 && */state) {
      //Отжата // кнопка после debounce
      _clickFunc((time - _start) / 1000, time);
      _start = 0;
      _stop = time;
      _count = -1;
    } else {
      byte count = (time - _start) / 1000;
      if (count != _count && count > 0) {
        Serial.println(count);
        _count = count;
        beep(1);
      }
    }
  } else if(!state) {
    time = millis();
    if (time - _stop > 500) {
      // Чтоб не было повторного нажатия после отжатия
      _start = millis();
      beep(1);
    }
  }
}

void KapButton::beep(int count) {
  if (_kapObjects->_beeper != NULL) {
     _kapObjects->_beeper->beep(count);
  } else if (_kapObjects->_light != NULL && !_kapObjects->_light->status()) {
     _kapObjects->_light->on(50);
  }
}
