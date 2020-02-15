#include <Arduino.h>

#include "KapObjects.h"

#ifndef KapTime_h
#define KapTime_h

class KapTime
{
  public:
    KapTime(KapObjects* kapObjects);
    void showTime();
    char* getTime();
    bool isTimeUpdated();
    bool updated();
  private:
    bool _isUpdated = false;
    KapObjects* _kapObjects = NULL;
    void timeIsSetScheduled();
};

#endif
