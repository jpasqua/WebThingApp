/*
 * LMSettings
 *    Setting for the LMCurrencyApp.
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
#include "LEDMatrixApp.h"
#include "LMSettings.h"
//--------------- End:    Includes ---------------------------------------------


LMSettings::LMSettings() {
  maxFileSize = 4096;
  version = LMSettings::CurrentVersion;
  for (int i = 0; i < MaxPrinters; i++) { printer[i].init(); }
}

void LMSettings::fromJSON(const JsonDocument &doc) {
  aio.username = String(doc["aioUsername"]|"");
  aio.key = String(doc["aioKey"]|"");
  aio.groupName = String(doc["aioGroup"]|"");

  scrollDelay = doc["scrollDelay"] | 20;

  JsonArrayConst osArray = doc[F("printerSettings")];
  int i = 0;
  for (JsonObjectConst os : osArray) {
    printer[i++].fromJSON(os);
    if (i == MaxPrinters) break;
  }
  printerRefreshInterval = doc[F("printerRefreshInterval")];
  if (printerRefreshInterval == 0) printerRefreshInterval = 30; // Sanity check

  WTAppSettings::fromJSON(doc);
  logSettings();
}

void LMSettings::toJSON(JsonDocument &doc) {
  doc["aioUsername"] = aio.username;
  doc["aioKey"] = aio.key;
  doc["aioGroup"] = aio.groupName;
  
  doc[F("scrollDelay")] = scrollDelay;

  JsonArray printerSettings = doc.createNestedArray(F("printerSettings"));
  for (int i = 0; i < MaxPrinters; i++) {
    printer[i].toJSON(printerSettings.createNestedObject());
  }
  doc[F("printerRefreshInterval")] = printerRefreshInterval;

  WTAppSettings::toJSON(doc);
}

void LMSettings::logSettings() {
  Log.verbose(F("LEDMatrix Settings"));
  Log.verbose(F("  aio.username = %s"), aio.username.c_str());
  Log.verbose(F("  aio.key = %s"), aio.key.c_str());
  Log.verbose(F("  aio.groupName = %s"), aio.groupName.c_str());
  Log.verbose(F("  scrollDelay = %d"), scrollDelay);
  for (int i = 0; i < MaxPrinters; i++) {
    Log.verbose(F("Printer Settings %d"), i);
    printer[i].logSettings();
  }
  Log.verbose(F("Printer refresh interval: %d"), printerRefreshInterval);

  WTAppSettings::logSettings();
}

