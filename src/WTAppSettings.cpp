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
//                                  Local Includes
#include "WTAppSettings.h"
//--------------- End:    Includes ---------------------------------------------


WTAppSettings::WTAppSettings() { }

void WTAppSettings::fromJSON(JsonDocument &doc) {
  uiOptions.fromJSON(doc);
  displayOptions.fromJSON(doc);
  owmOptions.fromJSON(doc);
}

void WTAppSettings::toJSON(JsonDocument &doc) {
  uiOptions.toJSON(doc);
  displayOptions.toJSON(doc);
  owmOptions.toJSON(doc);
}

void WTAppSettings::logSettings() {
  uiOptions.logSettings();
  displayOptions.logSettings();
  owmOptions.logSettings();
}



OWMOptions::OWMOptions() {
  enabled = true;
  key = "";
  cityID =  5372223;
  language = "en";
  nickname = "";
}

void OWMOptions::fromJSON(JsonDocument &doc) {
  enabled = doc[F("owm")][F("enabled")];
  key = doc[F("owm")][F("key")].as<String>();
  cityID = doc[F("owm")][F("cityID")];
  language = doc[F("owm")][F("language")].as<String>();
  nickname = String(doc[F("owm")][F("nickname")]|"");
}

void OWMOptions::toJSON(JsonDocument &doc) {
  doc[F("owm")][F("enabled")] = enabled;
  doc[F("owm")][F("key")] = key;
  doc[F("owm")][F("cityID")] = cityID;
  doc[F("owm")][F("language")] = language;
  doc[F("owm")][F("nickname")] = nickname;
}

void OWMOptions::logSettings() {
  Log.verbose(F("OpenWeatherMap Settings"));
  Log.verbose(F("  enabled: %T"), enabled);
  Log.verbose(F("  API Key: %s"), key.c_str());
  Log.verbose(F("  City ID: %d"), cityID);
  Log.verbose(F("  Language: %s"), language.c_str());
  Log.verbose(F("  City nickname: %s"), nickname.c_str());
}