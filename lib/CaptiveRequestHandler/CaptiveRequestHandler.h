#ifndef _CaptiveRequestHandler_H
#define _CaptiveRequestHandler_H

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Config.h>
#include <WifiConfigPersistor.h>
#include <FS.h>
#include <LittleFS.h>

class CaptiveRequestHandler : public AsyncWebHandler
{
private:
    WifiConfigPersistor* _persistor;
public:
    CaptiveRequestHandler(WifiConfigPersistor* persistor) { _persistor = persistor; }
    virtual ~CaptiveRequestHandler() {}
    bool canHandle(AsyncWebServerRequest *);
    void handleRequest(AsyncWebServerRequest *);
};

#endif