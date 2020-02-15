#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "KapLight.h"
#include "KapNetwork.h"
#include "KapTime.h"
#include "KapConfig.h"
#include "KapConfigStruct.h"
#include "KapServer.h"
#include "KapMqtt.h"
#include "KapObjects.h"

KapNetwork::KapNetwork(int port, KapObjects* kapObjects) {
  _kapObjects = kapObjects;
  _kapObjects->_time = new KapTime(kapObjects);
  _kapObjects->_server = new KapServer(kapObjects);

  KapConfigParams* conf = _kapObjects->_config->getConfig();

  if (conf->ssidName == "") {
    Serial.println("No WiFi connection information available.");
    startAP();
  } else {
    startStation(30);
  }

  if (!MDNS.begin("kitchen-led")) {
    Serial.println("Error setting up MDNS responder!");
  }  
}

KapTime *KapNetwork::getTime() {
  return _kapObjects->_time;
}

void KapNetwork::otaEnable() {
  Serial.println("Enable OTA");
  ArduinoOTA.setHostname("kitchen-led");
  // ArduinoOTA.setPassword("KitchenPassword");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
      SPIFFS.end();
    }
    Serial.println("Start updating " + type);
  });  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  _otaEnabled = true;
}

void KapNetwork::toggleMode() {
  Serial.println("Toggle mode");
  if (_apMode == WIFI_STA) {
    startAP();
  } else {
    startStation(60);
  }
}


void KapNetwork::startAP() {
  _kapObjects->_server->serverEnd();
  Serial.println("Starting AP");
  _apMode = WIFI_AP;
  WiFi.mode(WIFI_AP);
  delay(10);
  IPAddress local_IP(192,168,10,1);
  IPAddress gateway(192,168,10,1);
  IPAddress subnet(255,255,255,0);
  if (WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("AP config OK");
  } else {
    Serial.println("AP config FAILED");
  }
  WiFi.softAP("LED", "ESP8266LED");
  _kapObjects->_server->serverBegin(true);
}

void KapNetwork::startStation(int waitSecs) {
  if (_apMode != WIFI_STA && _kapObjects->_server->isStarted()) {
    _kapObjects->_server->serverEnd();
  }
  _apMode = WIFI_STA;
  
  KapConfigParams* conf = _kapObjects->_config->getConfig();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(conf->ssidName, conf->ssidPass);

  Serial.println("Trying to connect to ssid " + String(conf->ssidName));

  // Wait for connection
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter < waitSecs) {
    delay(1000);
    Serial.print(".");
    counter += 1;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to " + String(conf->ssidName));
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    _kapObjects->_server->serverBegin(false);
    Serial.println("HTTP server started");
    _kapObjects->_time->showTime();
  } else {
    Serial.println("\nFailed to connect");
    _disconnectTime = millis();
  }
}

void KapNetwork::checkConnection() {
  if (_kapObjects->_server->isStarted() && (_apMode == WIFI_AP  || WiFi.status() == WL_CONNECTED)) {
    if (_disconnectTime != 0) {
      Serial.println("WiFi connected");
      _disconnectTime = 0;
    }
    // Если в режиме точки доступа или подключен к роутеру
    _kapObjects->_server->process();
    if (_apMode == WIFI_STA) {
      _kapObjects->_mqtt->process();
      if (_otaEnabled) {
        ArduinoOTA.handle();
      }
    }
  } else if (_apMode == WIFI_STA) {
    // Информировать об отключении раз в 60 секунд
    unsigned long now = millis();
    if (now - _disconnectTime > 60000 || now < _disconnectTime) {
      Serial.println("WiFi disconnected");
      _disconnectTime = now;
    }
  }
  if (_kapObjects->_time->isTimeUpdated()) {
    _kapObjects->_time->showTime();
  }
}
