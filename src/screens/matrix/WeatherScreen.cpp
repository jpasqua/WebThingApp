/*
 * WeatherScreen:
 *    Display weather info for the selected city 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <Output.h>
#include <ESP_FS.h>
#include <WebThing.h>
//                                  Local Includes
#include "../../WTApp.h"
#include "../../WTAppImpl.h"
#include "../../gui/Display.h"
#include "../../gui/ScreenMgr.h"
#include "WeatherScreen.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * CONSTANTS
 *
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

WeatherScreen::WeatherScreen() {
  init();
  lastDT = 0;

  if (!settings.fromJSON(WSSettings::settingsFilePath)) {
    // No settings file, use defaults
    settings.fields.emplace_back(WSSettings::Field("City", true));
    settings.fields.emplace_back(WSSettings::Field("Temperature", true));
    settings.fields.emplace_back(WSSettings::Field("Wind", true));
    settings.fields.emplace_back(WSSettings::Field("Barometer", false));
    settings.fields.emplace_back(WSSettings::Field("Humidity", false));
    settings.fields.emplace_back(WSSettings::Field("Description", true));
    settings.fields.emplace_back(WSSettings::Field("Long Desc.", false));
    settings.toJSON(WSSettings::settingsFilePath);
  }
}


void WeatherScreen::innerActivation() {
  if (!wtApp->owmClient) {
    setText("No Weather Data");
    return;
  }

  auto& weather = wtApp->owmClient->weather;

  if (weather.dt == 0) {
    setText("No Weather Data");
    return;
  }

  if (lastDT && lastDT == weather.dt) {
    return; // We haven't updated the weather so no need to update the text
  }
  String weatherText;
  String delim = "";

  for (WSSettings::Field f : settings.fields) {
    if (!f.enabled) continue;
    weatherText += delim;
    delim = ", ";
    String key = f.id;
    key.toLowerCase();
    if (key.startsWith("city")) {
      String& city = wtApp->settings->owmOptions.nickname;
      if (city.isEmpty()) { city = weather.location.city; }
      weatherText += city;
      delim = " ";
    } else if (key.startsWith("temp")) {
      weatherText += String(weather.readings.temp, 0);
      weatherText += (wtApp->settings->uiOptions.useMetric ? "C" : "F");
    } else if (key.startsWith("humi")) {
      weatherText += String(weather.readings.humidity);
      weatherText += '%';
    } else if (key.startsWith("baro")) {
      weatherText += String(Output::baro(weather.readings.pressure));
      weatherText += Output::baroUnits();
    } else if (key.startsWith("wind")) {
      weatherText += String(weather.readings.windSpeed, 0);
      weatherText += wtApp->owmClient->dirFromDeg(weather.readings.windDeg);
    } else if (key.startsWith("desc")) {
      weatherText += weather.description.basic;
    } else if (key.startsWith("long")) {
      weatherText += weather.description.longer;
    }
  }

  setText(weatherText);
  lastDT = weather.dt;
}

// ----- Static methods

String WeatherScreen::getTextForIcon(String& icon) {
  if (icon.startsWith("01")) { return "Clear"; }
  if (icon.startsWith("02")) { return "Few Clouds"; }
  if (icon.startsWith("03")) { return "Scattered Clouds"; }
  if (icon.startsWith("04")) { return "Broken Clouds"; }
  if (icon.startsWith("09")) { return "Showers"; }
  if (icon.startsWith("10")) { return "Rain"; }
  if (icon.startsWith("11")) { return "Thunderstorm"; }
  if (icon.startsWith("13")) { return "Snow"; }
  if (icon.startsWith("50")) { return "Haze"; }
  else return "";
}

/*------------------------------------------------------------------------------
 *
 * WSSettings Implementation
 *
 *----------------------------------------------------------------------------*/

WSSettings::WSSettings() {
  // Test
  // fields.emplace_back(Field("city", true));
  // fields.emplace_back(Field("temp", false));
  // fields.emplace_back(Field("wind", true));
}

/* Available fields
 fields: [
  {id: "city", on: true},
  {id: "temp", on: true},
  {id: "humi", on: true}
  {id: "baro", on: true},
  {id: "wind", on: true}
  {id: "desc", on: true},
  {id: "ldesc", on: true},
];
*/


bool WSSettings::fromJSON(const char* filePath) {
  File f = ESP_FS::open(filePath, "r");
  if (!f) {
    Log.warning("No weather screen settings file was found: %s", filePath);
    return false;
  }

  DynamicJsonDocument doc(MaxDocSize);

  auto error = deserializeJson(doc, f);
  if (error) {
    Log.warning(F("Error parsing weather screen settings (%s): %s"), filePath, error.c_str());
    return false;
  }

  fromJSON(doc);
  if (fields.size() == 0) return false;
  logSettings();
  return true;
}

void WSSettings::fromJSON(const String &json) {
  DynamicJsonDocument doc(MaxDocSize);
  auto error = deserializeJson(doc, json);
  if (error) {
    Log.warning(F("Failed to parse weather screen field settings: %s"), error.c_str());
    Log.warning(F("%s"), json.c_str());
  }
  fromJSON(doc);
}

void WSSettings::fromJSON(const JsonDocument &doc) {
  fields.clear();
  const JsonArrayConst& jsonFields = doc["fields"].as<JsonArray>();
  Field field;
  for (const auto& jsonField : jsonFields) {
     field.enabled = jsonField["on"];
     field.id = jsonField["id"].as<String>();
     fields.push_back(field);
  }
}

void WSSettings::toJSON(JsonDocument &doc) {
  JsonArray jsonFields = doc.createNestedArray("fields");
  for (Field& field : fields) {
    JsonObject jsonField = jsonFields.createNestedObject();
    jsonField["id"] = field.id;
    jsonField["on"] = field.enabled;
  }
}


void WSSettings::toJSON(const char* filePath) {
  File settingsFile = ESP_FS::open(filePath, "w");
  if (!settingsFile) {
    Log.error(F("Failed to open file for writing: %s"), filePath);
  }
  Log.verbose("Writing weather screen settings to %s", filePath);
  toJSON(settingsFile);
  settingsFile.close();
}

void WSSettings::toJSON(Stream& s) {
  // DynamicJsonDocument doc(MaxDocSize);
  // toJSON(doc);
  // serializeJson(doc, s);

  s.println("{ \"fields\": [");
  bool first = true;
  for (Field& field : fields) {
    if (!first) s.println(", ");
    else first = false;
    s.print("{");
      s.print("\"id\":\""); s.print(field.id); s.print("\",");
      s.print("\"on\":"); s.print(field.enabled ? "true" : "false");
    s.print("}");
  }
  s.println("]}");
}

void WSSettings::toJSON(String &serialString) {
  DynamicJsonDocument doc(MaxDocSize);
  toJSON(doc);
  serializeJson(doc, serialString);
}

void WSSettings::logSettings() {
  Log.verbose(F("Weather Screen Field Selection"));
  for (const Field& field : fields) {
    Log.verbose(F("  %s, enabled: %T"), field.id.c_str(), field.enabled);
  }
}

#endif






