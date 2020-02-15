#include <Arduino.h>
#include "KapObjects.h"

#ifndef KapButton_h
#define KapButton_h

class KapButton
{
  public:
    KapButton(int pin, void (*clickFunc)(byte, unsigned long), KapObjects*);
    void process();
  private:
    KapObjects* _kapObjects;
    int _pin;
    unsigned long _start = 0;
    unsigned long _stop = 0;
    byte _count = 0;
    void (*_clickFunc)(byte count, unsigned long pressTime);
    void beep(int count);
};

#endif
