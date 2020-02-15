#include <Arduino.h>
#include <TZ.h>
#include <ESP8266WiFi.h>
#include <coredecls.h>                  // settimeofday_cb()
#include <Schedule.h>
#include <PolledTimeout.h>
#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <sntp.h>                       // sntp_servermode_dhcp()

#include "KapObjects.h"
#include "KapTime.h"
#include "KapMqtt.h"

extern "C" int clock_gettime(clockid_t unused, struct timespec *tp);

static timeval tv;
static timespec tp;
static time_t now;
static uint32_t now_ms, now_us;

static esp8266::polledTimeout::periodicMs showTimeNow(60000);


KapTime::KapTime(KapObjects* kapObjects){
  _kapObjects = kapObjects;
  time_t rtc = 0;
  timeval tv = { rtc, 0 };
  timezone tz = { 0, 0 };
  settimeofday(&tv, &tz);
  settimeofday_cb(std::bind(&KapTime::timeIsSetScheduled, this));
  configTime(TZ_Europe_Moscow, "pool.ntp.org");
}

char* KapTime::getTime()  {
  now = time(nullptr);
  return ctime(&now);
}

bool KapTime::updated()  {
  return _isUpdated;
}

void KapTime::showTime() {
  now = time(nullptr);
/*
  gettimeofday(&tv, nullptr);
  clock_gettime(0, &tp);
  now_ms = millis();
  now_us = micros();
  Serial.println();
  printTm("localtime:", localtime(&now));
  Serial.println();
  printTm("gmtime:   ", gmtime(&now));
  Serial.println();
  Serial.print("clock:     ");
  Serial.print((uint32_t)tp.tv_sec);
  Serial.print("s / ");
  Serial.print((uint32_t)tp.tv_nsec);
  Serial.println("ns");
  Serial.print("millis:    ");
  Serial.println(now_ms);
  Serial.print("micros:    ");
  Serial.println(now_us);
  Serial.print("gtod:      ");
  Serial.print((uint32_t)tv.tv_sec);
  Serial.print("s / ");
  Serial.print((uint32_t)tv.tv_usec);
  Serial.println("us");
  Serial.print("time:      ");
  Serial.println((uint32_t)now);
*/
  const char* tm = ctime(&now);
  // Serial.print(tm);
  if (_kapObjects->_mqtt != NULL) {
    _kapObjects->_mqtt->setTime(tm);
  }
/*
  for (int i = 0; i < SNTP_MAX_SERVERS; i++) {
    IPAddress sntp = *sntp_getserver(i);
    const char* name = sntp_getservername(i);
    if (sntp.isSet()) {
      Serial.printf("sntp%d:     ", i);
      if (name) {
        Serial.printf("%s (%s) ", name, sntp.toString().c_str());
      } else {
        Serial.printf("%s ", sntp.toString().c_str());
      }
    }
  }
*/
}
/*
#define PTM(w) \
  Serial.print(" " #w "="); \
  Serial.print(tm->tm_##w);

void Time::printTm(const char* what, const tm* tm) {
  Serial.print(what);
  PTM(isdst); PTM(yday); PTM(wday);
  PTM(year);  PTM(mon);  PTM(mday);
  PTM(hour);  PTM(min);  PTM(sec);
}
*/
void KapTime::timeIsSetScheduled() {
  //now = time(nullptr);
  // gettimeofday(&tv, nullptr);
  //showTime();
  _isUpdated = true;
}

bool KapTime::isTimeUpdated() {
  return showTimeNow && _isUpdated;
}
