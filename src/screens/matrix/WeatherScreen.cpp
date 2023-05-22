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
  settings.read();
  if (settings.fields.size() == 0) {
    Log.verbose("No WeatherScreen settings were found, using defaults");
    settings.fields.emplace_back(WSSettings::Field("City", true));
    settings.fields.emplace_back(WSSettings::Field("Temperature", true));
    settings.fields.emplace_back(WSSettings::Field("Wind", true));
    settings.fields.emplace_back(WSSettings::Field("Barometer", false));
    settings.fields.emplace_back(WSSettings::Field("Humidity", false));
    settings.fields.emplace_back(WSSettings::Field("Description", true));
    settings.fields.emplace_back(WSSettings::Field("Long Desc.", false));
    settings.write();
  }

  init();
  lastDT = 0;
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

void WeatherScreen::settingsHaveChanged() {
  settings.write();
  lastDT = UINT32_MAX;  // Force a refresh next time through
}

/*------------------------------------------------------------------------------
 *
 * WSSettings Implementation
 *
 *----------------------------------------------------------------------------*/

WSSettings::WSSettings() {
  maxFileSize = 512;
  version = 1;
  init("/wta/WSSettings.json");
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

void WSSettings::fromJSON(const JsonDocument& doc) {
  fields.clear();
  const JsonArrayConst& jsonFields = doc["fields"].as<JsonArray>();
  Field field;
  for (const auto& jsonField : jsonFields) {
     field.enabled = jsonField["on"];
     field.id = jsonField["id"].as<String>();
     fields.push_back(field);
  }
}

void WSSettings::toJSON(JsonDocument& doc) {
  JsonArray jsonFields = doc.createNestedArray("fields");
  for (Field& field : fields) {
    JsonObject jsonField = jsonFields.createNestedObject();
    jsonField["id"] = field.id;
    jsonField["on"] = field.enabled;
  }
}

void WSSettings::logSettings() {
  Log.verbose(F("Weather Screen Field Selection"));
  for (const Field& field : fields) {
    Log.verbose(F("  %s, enabled: %T"), field.id.c_str(), field.enabled);
  }
}

#endif






