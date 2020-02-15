#include <Arduino.h>

#ifndef KapBeeper_h
#define KapBeeper_h

class KapBeeper
{
  public:
    KapBeeper(int pin);
    void beep(int count);
  private:
    int _pin;
};

#endif
