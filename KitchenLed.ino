#include "KapConfig.h"
#include "KapLight.h"
#include "KapSensor.h"
#include "KapNetwork.h"
#include "KapButton.h"
#include "KapMqtt.h"
#include "KapBeeper.h"

int BUT1_PIN = 5;
int LIGHT_PIN = 14;
int BEEP_PIN = 12;
int SENSOR_PIN = 4;


KapObjects* kapObjects;

const unsigned long timeSensor  =  120000;      //2 минуты
const unsigned long timeButton  =  600000;      //10 минут
const unsigned long timeButtonLong = 1800000;   //30 минут




void ledSwitcherClick(byte count, unsigned long now){
  Serial.print("Click count: ");
  Serial.println(count);
  if (count == 0) {
    kapObjects->_light->toggle(timeButton);
  } else if (count >= 10 && count < 15) {
    kapObjects->_network->toggleMode();
  } else if (count >= 15) {
    kapObjects->_network->otaEnable();
  } else {
    kapObjects->_light->toggle(timeButtonLong);
  }
}




void setup() {
  Serial.begin(115200);
  delay(1000);  
  Serial.println("\n\n\nHello, All");

  kapObjects = new KapObjects();
  kapObjects->_config = new KapConfig();
  kapObjects->_light = new KapLight(LIGHT_PIN, kapObjects);
  kapObjects->_beeper = new KapBeeper(BEEP_PIN);
  kapObjects->_sensor = new KapSensor(SENSOR_PIN, timeSensor, kapObjects);
  kapObjects->_network = new KapNetwork(80, kapObjects);
  kapObjects->_mqtt = new KapMqtt(kapObjects);
  kapObjects->ledSwitcher = new KapButton(BUT1_PIN, &ledSwitcherClick, kapObjects);
}

void loop() {
  kapObjects->ledSwitcher->process();
  kapObjects->_light->process();
  kapObjects->_sensor->process();
  kapObjects->_network->checkConnection();  
}
