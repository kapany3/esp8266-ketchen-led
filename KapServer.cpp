#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "KapServer.h"
#include "KapTime.h"
#include "KapLight.h"
#include "KapConfig.h"
#include "KapNetwork.h"
#include "KapObjects.h"

KapServer::KapServer(KapObjects* kapObjects) {
  _kapObjects = kapObjects;
  _server = new ESP8266WebServer(80);
}

void KapServer::process() {
  _server->handleClient();
}

bool KapServer::isStarted() {
  return _isStarted;
}

void KapServer::serverBegin(bool isAP) {
  Serial.println("Start server");
  _isAP = isAP;
  _server->on("/", std::bind(&KapServer::handleRoot, this));
  _server->onNotFound(std::bind(&KapServer::handleNotFound, this));
  if (isAP) {
    _server->on("/getConfig", std::bind(&KapServer::handleGetConfig, this));
    _server->on("/setConfig", std::bind(&KapServer::handleSetConfig, this));
  }
  _server->begin();
  _isStarted = true;
}

void KapServer::serverEnd() {
  if (_isStarted) {
    Serial.println("Stop server");
    _server->stop();
    _isStarted = false;
  }
}

void KapServer::handleRoot() {
  String message = "";
  uint8_t argsCount = _server->args();

  if (!_isAP) {
    if (argsCount > 0) {
      String command = _server->argName(0);
      if (command == "ON") {
        int timeGlow = 600000;
        int time = _server->arg(0).toInt();
        if (time > 0) {
          timeGlow = time * 1000;
        }
        message += "LIGHT ON FOR TIME: " + String(timeGlow) + " MS";
        _kapObjects->_light->on(timeGlow);
      } else if (command == "OFF") {
        _kapObjects->_light->off();
        message += "LIGHT OFF";
      } else if (command == "SETTINGS") {
        handleFileRead("/config.html");
        return;
      }
    } else {
      if (_kapObjects->_light->status() == HIGH) {
        // Показать, сколько осталось светить
        message = "ON FOR " + String(_kapObjects->_light->changeTime() + _kapObjects->_light->timeToGlow() - millis()) + " MS";
      } else {
        message = "OFF";
      }
      char* now = _kapObjects->_time->getTime();
      message += "\nTime: " + String(now);
    }
    _server->send(200, "text/plain", message);
  } else {
    handleFileRead("/config.html");
  }
}

void KapServer::handleGetConfig() {
  String message = "";
  StaticJsonDocument<512> doc;

  KapConfigParams* conf = _kapObjects->_config->getConfig();

  doc["ssidName"] = conf->ssidName;
  doc["ssidPass"] = conf->ssidPass;
  doc["mqttServer"] = conf->mqttServer;
  doc["mqttLogin"] = conf->mqttLogin;
  doc["mqttPass"] = conf->mqttPass;
  doc["noSensorFromH"] = conf->noSensorFromH;
  doc["noSensorFromM"] = conf->noSensorFromM;
  doc["noSensorToH"] = conf->noSensorToH;
  doc["noSensorToM"] = conf->noSensorToM;

  if (serializeJson(doc, message) == 0) {
    Serial.println(F("Failed to write to string"));
  }

  _server->send(200, "text/plain", message);
}

void KapServer::handleSetConfig() {
  String message = "";
  KapConfigParams* conf = _kapObjects->_config->getConfig();
  
  uint8_t argsCount = _server->args();
  char buf[32];
  for (int i = 0; i < argsCount; i += 1) {
    String argName = _server->argName(i);
    _server->arg(i).toCharArray(buf, 32);
    
    if (argName == "ssid_name") {
      strlcpy(conf->ssidName, buf, sizeof(conf->ssidName));
    } else if (argName == "ssid_pass") {
      strlcpy(conf->ssidPass, buf, sizeof(conf->ssidPass));
    } else if (argName == "mqtt_server") {
      strlcpy(conf->mqttServer, buf, sizeof(conf->mqttServer));
    } else if (argName == "mqtt_login") {
      strlcpy(conf->mqttLogin, buf, sizeof(conf->mqttLogin));
    } else if (argName == "mqtt_pass") {
      strlcpy(conf->mqttPass, buf, sizeof(conf->mqttPass));
    }
  }
  _kapObjects->_config->saveConfig();
  _kapObjects->_network->toggleMode();
  _server->send(200, "text/plain", message);
}

void KapServer::handleNotFound() {
  if (!handleFileRead(_server->uri())) {                  // send it if it exists
      _server->send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  }
}

String KapServer::getContentType(String filename) {
  if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool KapServer::handleFileRead(String path) {  // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){  // If the file exists, either as a compressed archive, or normal
    if(SPIFFS.exists(pathWithGz))                          // If there's a compressed version available
      path += ".gz";                                         // Use the compressed version
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = _server->streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false;                                          // If the file doesn't exist, return false
}
