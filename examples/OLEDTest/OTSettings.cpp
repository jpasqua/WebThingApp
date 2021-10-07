/*
 * OTSettings
 *    Setting for the OTCurrencyApp.
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
#include <FS.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <ArduinoJson.h>
//                                  Local Includes
#include "OLEDTestApp.h"
#include "OTSettings.h"
//--------------- End:    Includes ---------------------------------------------


OTSettings::OTSettings() {
  maxFileSize = 4096;
  version = OTSettings::CurrentVersion;
  rateApiKey = "";
  for (uint8_t i = 0; i < OTSettings::MaxCurrencies; i++) {
    currencies[i].id = "";
    currencies[i].nickname = "";
  }
}

void OTSettings::fromJSON(const JsonDocument &doc) {
  refreshInterval = doc[F("refreshInterval")];
  refreshInterval = max<uint32_t>(refreshInterval, MinRefreshInterval);

  rateApiKey = doc["rateApiKey"].as<const char*>();

  JsonArrayConst osArray = doc[F("currencies")];
  int i = 0;
  for (JsonObjectConst os : osArray) {
    currencies[i].id = os["id"].as<const char*>();
    currencies[i].nickname = os["nickname"].as<const char*>();
    if (++i == OTSettings::MaxCurrencies) break;
  }

  WTAppSettings::fromJSON(doc);
  logSettings();
}

void OTSettings::toJSON(JsonDocument &doc) {
  doc[F("refreshInterval")] = refreshInterval;
  doc["rateApiKey"] = rateApiKey;

  JsonArray jsonCurrencies = doc.createNestedArray(F("currencies"));
  for (int i = 0; i < OTSettings::MaxCurrencies; i++) {
    JsonObject jsonCurrency = jsonCurrencies.createNestedObject();
    jsonCurrency["id"] = currencies[i].id;
    jsonCurrency["nickname"] = currencies[i].nickname;
  }

  WTAppSettings::toJSON(doc);
}

void OTSettings::logSettings() {
  Log.verbose(F("refreshInterval: %d"), refreshInterval);
  Log.verbose(F("Exchange Rate API Key: %s"), rateApiKey.c_str());
  Log.verbose(F("Currencies"));
  for (int i = 0; i < OTSettings::MaxCurrencies; i++) {
    Log.verbose("  %s(%s)", currencies[i].id.c_str(), currencies[i].nickname.c_str());
  }

  WTAppSettings::logSettings();
}

