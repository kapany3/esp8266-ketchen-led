#include <Arduino.h>
#include "KapBeeper.h"
#include "KapObjects.h"

#ifndef KapLight_h
#define KapLight_h

class KapLight
{
  public:
    KapLight(int lightPin, KapObjects* kapObjects);
    void on(unsigned long time);
    void off();
    void process();
    void toggle(unsigned long timeToGlow);
    int status();
    int isBeeped();
    unsigned long changeTime();
    unsigned long timeToGlow();
  private:
    KapObjects* _kapObjects;
    int _lightPin;
    int _status = false;
    bool _isBeeped = false;
    unsigned long _timeBeep = 10000;       // За сколько миллисекунд до выключения пиликнуть
    unsigned long _statusChangedTime = 0;  // время предыдущего изменения статуса
    unsigned long _OffTime = 0;            // время выключения света по кнопке
    unsigned long _timeToGlow = 0;         // по умолчанию продолжительность свечения равна свечению от датчика
    void beep(int count);
};

#endif
