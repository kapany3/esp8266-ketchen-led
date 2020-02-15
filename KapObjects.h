#include <Arduino.h>

#ifndef KapObjects_h
#define KapObjects_h

class KapConfig;
class KapBeeper;
class KapLight;
class KapSensor;
class KapTime;
class KapServer;
class KapNetwork;
class KapButton;
class KapMqtt;

class KapObjects
{
  public:
    KapObjects();
    
    KapConfig* _config = NULL;
    KapBeeper* _beeper = NULL;
    KapLight* _light = NULL;
    KapSensor* _sensor = NULL;
    KapTime* _time = NULL;
    KapServer* _server = NULL;
    KapNetwork* _network = NULL;
    KapMqtt* _mqtt = NULL;
    KapButton* ledSwitcher = NULL;
};

#endif
