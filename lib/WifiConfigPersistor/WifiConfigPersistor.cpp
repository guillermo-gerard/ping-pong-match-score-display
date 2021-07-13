#include "WifiConfigPersistor.h"
#include <FS.h>
#include <LittleFS.h>
#include <string.h>
#include <ArduinoJson.h>

// Saves the configuration to a file
bool WifiConfigPersistor ::Persist(Config config) //const char *filename)//, const Config &config)
{
  LittleFS.remove(_filename);

  File file = LittleFS.open(_filename, "w");
  if (!file)
  {
    Serial.println(F("Failed to create file"));
    return false;
  }
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use https://arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<128> doc;

  doc["ssid"] = config.ssid;
  doc["pass"] = config.pass;

  if (serializeJson(doc, file) == 0)
  {
    Serial.println(F("Failed to write to file"));
    return false;
  }
  Serial.println("File saved");
  file.close();
  return true;
}

// Loads the configuration from a file
Config WifiConfigPersistor ::Retrieve()
{
  // Open file for reading
  File file = LittleFS.open(_filename, "r");

  StaticJsonDocument<128> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  Config config;

  strcpy(config.ssid, doc["ssid"] | "");
  strcpy(config.pass, doc["pass"] | "");

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();

  return config;
}

// Prints the content of a file to the Serial (used in the debug step, not needed anymore)
// void printFile(const char *filename)
// {
//   // Open file for reading
//   File file = LittleFS.open(filename, "r");
//   if (!file)
//   {
//     Serial.println(F("Failed to read file"));
//     return;
//   }
//   // Extract each characters by one by one
//   while (file.available())
//   {
//     Serial.print((char)file.read());
//   }
//   Serial.println();
//   // Close the file
//   file.close();
// }