/*
 * WTAppSettings
 *    Handle reading and writing settings information to the file system
 *    in JSON format.
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <ArduinoJson.h>
//                                  WebThing Includes
//                                  Local Includes
#include "WTAppSettings.h"
//--------------- End:    Includes ---------------------------------------------


WTAppSettings::WTAppSettings() { }

void WTAppSettings::fromJSON(const JsonDocument &doc) {
  uiOptions.fromJSON(doc);
  displayOptions.fromJSON(doc);
  owmOptions.fromJSON(doc);
  screenSettings.fromJSON(doc);
}

void WTAppSettings::toJSON(JsonDocument &doc) {
  uiOptions.toJSON(doc);
  displayOptions.toJSON(doc);
  owmOptions.toJSON(doc);
  screenSettings.toJSON(doc);
}

void WTAppSettings::logSettings() {
  uiOptions.logSettings();
  displayOptions.logSettings();
  owmOptions.logSettings();
  screenSettings.logSettings();
}



OWMOptions::OWMOptions() {
  enabled = true;
  key = "";
  cityID =  5372223;
  language = "en";
  nickname = "";
}

void OWMOptions::fromJSON(const JsonDocument &doc) {
  JsonObjectConst owm = doc["owm"].as<JsonObjectConst>();
  enabled = owm[F("enabled")];
  key = owm["key"].as<const char*>();
  cityID = owm[F("cityID")] | 5372223;
  nickname = owm["nickname"].as<const char*>();
  language = owm["language"]|"en";
}

void OWMOptions::toJSON(JsonDocument &doc) {
  doc["owm"]["enabled"] = enabled;
  doc["owm"]["key"] = key;
  doc["owm"]["cityID"] = cityID;
  doc["owm"]["language"] = language;
  doc["owm"]["nickname"] = nickname;
}

void OWMOptions::logSettings() {
  Log.verbose(F("OpenWeatherMap Settings"));
  Log.verbose(F("  enabled: %T"), enabled);
  Log.verbose(F("  API Key: %s"), key.c_str());
  Log.verbose(F("  City ID: %d"), cityID);
  Log.verbose(F("  Language: %s"), language.c_str());
  Log.verbose(F("  City nickname: %s"), nickname.c_str());
}