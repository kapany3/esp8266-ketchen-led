#include <Arduino.h>
#include "KapBeeper.h"

KapBeeper::KapBeeper(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void KapBeeper::beep(int count){
  for (int i = 0; i < count; i += 1) {
    tone(_pin, 2000 +  i * 500);
    delay(20);
    noTone(_pin);
    delay(10);
  }
}
