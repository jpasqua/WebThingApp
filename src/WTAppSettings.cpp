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


//
// ----- WTAppSettings Implementation
// 

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


//
// ----- OWMOptions Implementation
// 

OWMOptions::OWMOptions() {
  enabled = true;
  key = "";
  cityID =  5372223;
  language = "en";
  nickname = "";
}

void OWMOptions::fromJSON(const JsonDocument &doc) {
  JsonObjectConst owm = doc["owm"].as<JsonObjectConst>();
  enabled     = owm["enabled"];
  key         = owm["key"].as<const char*>();
  cityID      = owm["cityID"] | 5372223;
  nickname    = owm["nickname"].as<const char*>();
  language    = owm["language"]|"en";
  refresh     = owm["refresh"] | 60;
  fcstRefresh = owm["fcstRefresh"] | 8;
}


void OWMOptions::toJSON(JsonDocument &doc) {
  doc["owm"]["enabled"] = enabled;
  doc["owm"]["key"] = key;
  doc["owm"]["cityID"] = cityID;
  doc["owm"]["language"] = language;
  doc["owm"]["nickname"] = nickname;
  doc["owm"]["refresh"] = refresh;
  doc["owm"]["fcstRefresh"] = fcstRefresh;
}

void OWMOptions::fromJSON(const String& jsonString) {
  DynamicJsonDocument doc(256);
  auto error = deserializeJson(doc, jsonString);
  if (error) {
    Log.warning("Error (%s) while parsing: %s", error.c_str(), jsonString.c_str());
  }
  fromJSON(doc);
}

void OWMOptions::toJSON(String& jsonString) {
  DynamicJsonDocument doc(256);
  toJSON(doc);
  serializeJson(doc, jsonString);
}

void OWMOptions::logSettings() {
  Log.verbose("OpenWeatherMap Settings");
  Log.verbose("  enabled: %T", enabled);
  Log.verbose("  API Key: %s", key.c_str());
  Log.verbose("  City ID: %d", cityID);
  Log.verbose("  Language: %s", language.c_str());
  Log.verbose("  City nickname: %s", nickname.c_str());
  Log.verbose("  Weather Refresh (min): %d", refresh);
  Log.verbose("  Forecast Refresh (hr): %d", fcstRefresh);
}