/*
 * CMSettings
 *    Setting for the CMCurrencyApp.
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
#include "CurrencyMonApp.h"
#include "CMSettings.h"
//--------------- End:    Includes ---------------------------------------------

const uint32_t  CMSettings::CurrentVersion = 0x0001;

CMSettings::CMSettings() {
  maxFileSize = 4096;
  version = CMSettings::CurrentVersion;
  rateApiKey = "";
  for (uint8_t i = 0; i < CMSettings::MaxCurrencies; i++) {
    currencies[i].id = "";
    currencies[i].nickname = "";
  }
}

void CMSettings::fromJSON(JsonDocument &doc) {
  refreshInterval = doc[F("refreshInterval")];
  refreshInterval = max<uint32_t>(refreshInterval, MinRefreshInterval);

  rateApiKey = String(doc["rateApiKey"]|"");

  JsonArrayConst osArray = doc[F("currencies")];
  int i = 0;
  for (JsonObjectConst os : osArray) {
    currencies[i].id = os["id"].as<String>();
    currencies[i].nickname = os["nickname"].as<String>();
    i++;
    if (i == CMSettings::MaxCurrencies) break;
  }

  WTAppSettings::fromJSON(doc);
  logSettings();
}

void CMSettings::toJSON(JsonDocument &doc) {
  doc[F("refreshInterval")] = refreshInterval;
  doc["rateApiKey"] = rateApiKey;

  JsonArray jsonCurrencies = doc.createNestedArray(F("currencies"));
  for (int i = 0; i < CMSettings::MaxCurrencies; i++) {
    JsonObject jsonCurrency = jsonCurrencies.createNestedObject();
    jsonCurrency["id"] = currencies[i].id;
    jsonCurrency["nickname"] = currencies[i].nickname;
  }

  WTAppSettings::toJSON(doc);
}

void CMSettings::logSettings() {
  Log.verbose(F("refreshInterval: %d"), refreshInterval);
  Log.verbose(F("Exchange Rate API Key: %s"), rateApiKey.c_str());
  Log.verbose(F("Currencies"));
  for (int i = 0; i < CMSettings::MaxCurrencies; i++) {
    Log.verbose("  %s(%s)", currencies[i].id.c_str(), currencies[i].nickname.c_str());
  }

  WTAppSettings::logSettings();
}

