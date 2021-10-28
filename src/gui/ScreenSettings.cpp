#include <ArduinoLog.h>
#include "ScreenSettings.h"

ScreenSettings::ScreenSettings() {
  // Test
  // screenInfo.emplace_back(ScreenInfo("Home", true));
  // screenInfo.emplace_back(ScreenInfo("Weather", false));
  // screenInfo.emplace_back(ScreenInfo("AQI", true));
}

/*
 screens: [
  {name: "Home", on: true},
  {name: "AQI",  on: true},
  {name: "Wthr", on: true},
  {name: "Mix1", on: true},
];
*/


void ScreenSettings::fromJSON(const String &json) {
  DynamicJsonDocument doc(MaxDocSize);
  auto error = deserializeJson(doc, json);
  if (error) {
    Log.warning(F("Failed to parse screens settings: %s"), error.c_str());
    Log.warning(F("%s"), json.c_str());
  }
  fromJSON(doc);
}

void ScreenSettings::fromJSON(const JsonDocument &doc) {
  const JsonArrayConst& jsonScreens = doc["screens"].as<JsonArray>();
  ScreenInfo si;
  for (const auto& jsonScreen : jsonScreens) {
     si.enabled = jsonScreen["on"];
     si.id = jsonScreen["id"].as<String>();
     screenInfo.push_back(si);
  }
}

void ScreenSettings::toJSON(JsonDocument &doc) {
  JsonArray jsonScreens = doc.createNestedArray("screens");
  for (ScreenInfo& si : screenInfo) {
    JsonObject jsonScreen = jsonScreens.createNestedObject();
    jsonScreen["id"] = si.id;
    jsonScreen["on"] = si.enabled;
  }
}

void ScreenSettings::toJSON(Stream& s) {
  // DynamicJsonDocument doc(MaxDocSize);
  // toJSON(doc);
  // serializeJson(doc, s);

  s.println("{ \"screens\": [");
  bool first = true;
  for (ScreenInfo& si : screenInfo) {
    if (!first) s.print(", ");
    else first = false;
    s.print("{");
      s.print("\"id\":"); s.print(si.id); s.print(",");
      s.print("\"on\":"); s.print(si.enabled ? "true" : "false");
    s.print("}");
  }
  s.println("]}");
}

void ScreenSettings::toJSON(String &serialString) {
  DynamicJsonDocument doc(MaxDocSize);
  toJSON(doc);
  serializeJson(doc, serialString);
}

void ScreenSettings::logSettings() {
  Log.verbose(F("Screen Selection"));
  for (const ScreenInfo& si : screenInfo) {
    Log.verbose(F("  %s, enabled: %T"), si.id.c_str(), si.enabled);
  }
}


