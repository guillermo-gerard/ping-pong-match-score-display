#include "CaptiveRequestHandler.h"

bool CaptiveRequestHandler ::canHandle(AsyncWebServerRequest *request)
{
    return true;
}

void CaptiveRequestHandler ::handleRequest(AsyncWebServerRequest *request)
{
    Serial.print("url: ");
    Serial.println(request->url());

    if (request->url().compareTo("/save-settings") == 0)
    {
        Serial.println("Request to save settings received");

        String ssid = request->arg("ssid");
        String pass = request->arg("password");

        Config config;

        strcpy(config.ssid, ssid.c_str());
        strcpy(config.pass, pass.c_str());

        _persistor->Persist(config);

        request->send(200, "text/plain", "Credentials successfuly saved");
        ESP.restart();
    }

    if (LittleFS.exists(request->url()))
    {
        Serial.println("Sending the file");
        request->send(LittleFS, request->url(), String(), false);
        return;
    }

    request->send(LittleFS, "/config.html", String(), false);
}
