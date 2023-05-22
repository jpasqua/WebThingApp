
/*
 * ForecastScreen:
 *    Display the forecast for the selected city 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <BPABasics.h>
//                                  WebThing Includes
#include <WebThing.h>
//                                  Local Includes
#include "../../WTApp.h"
#include "../../WTAppImpl.h"
#include "../../gui/Display.h"
#include "../../gui/ScreenMgr.h"
#include "ForecastScreen.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

ForecastScreen::ForecastScreen() {
  settings.read();

  bool updateSettings = false;
  if (settings.heading.size() == 0) {
    settings.heading.emplace_back(FSSettings::Field("City", true));
    settings.heading.emplace_back(FSSettings::Field("Label", true));
    settings.label = "5-Day";
    updateSettings = true;
  }

  if (settings.fields.size() == 0) {
    settings.fields.emplace_back(FSSettings::Field("Day", true));
    settings.fields.emplace_back(FSSettings::Field("Hi-Lo", true));
    settings.fields.emplace_back(FSSettings::Field("Description", true));
    settings.fields.emplace_back(FSSettings::Field("Long Desc.", false));
    updateSettings = true;
  }
  if (updateSettings) { settings.write(); }

  // Perform other initialization
  nLabels = 0;
  labels = NULL;

  init();
  _forecastText.clear();
}

void ForecastScreen::innerActivation() {
  if (!wtApp->owmClient) {
    _forecastText = "No forecast available";
    return;
  };

  Forecast* forecast = wtApp->owmClient->getForecast();
  if (!_forecastText.isEmpty()) { // We may already have a valid forecast string
    if (wtApp->owmClient->timeOfLastForecastUpdate() == _timeOfLastForecast) return;
  }

  String& city = wtApp->settings->owmOptions.nickname;
  if (city.isEmpty()) { city = wtApp->owmClient->weather.location.city; }

  _forecastText.clear();
  for (FSSettings::Field f: settings.heading) {
    if (!f.enabled) continue;
    if (f.id.equalsIgnoreCase("city")) { _forecastText += city; _forecastText += ' '; }
    else if (f.id.equalsIgnoreCase("label")) {
      int len = settings.label.length();
      if (len) {
        _forecastText += settings.label;
        char lastChar = _forecastText[len-1];
        if (lastChar != ' ' && lastChar != ':') _forecastText += ' ';
      }
    }
  }

  for (int i = 0; i < wtApp->owmClient->ForecastElements; i++) {
    if (i) _forecastText += ", ";
    appendForecastForDay(&forecast[i], _forecastText);
  }


  setText(_forecastText, Display.BuiltInFont_ID);
  _timeOfLastForecast = wtApp->owmClient->timeOfLastForecastUpdate();
}

void ForecastScreen::settingsHaveChanged() {
  settings.write();
  _timeOfLastForecast = 0;  // Force the text to be updated upon the next display
}

// ----- Private Methods

void ForecastScreen::appendForecastForDay(Forecast* forecast, String& appendTo) {
  bool first = true;
  for (FSSettings::Field f : settings.fields) {
    if (!f.enabled) continue;
    if (!first) { appendTo += ' '; }
    if (f.id.equalsIgnoreCase("day")) {
      char* dayAsString = dayStr(weekday(forecast->dt));
      appendTo += dayAsString[0];
      appendTo += dayAsString[1];
      appendTo += dayAsString[2];
    } else if (f.id.equalsIgnoreCase("hi-lo")) {
      appendTo += (int)forecast->loTemp;
      appendTo += '/';
      appendTo += (int)forecast->hiTemp;
    } else if (f.id.equalsIgnoreCase("description")) {
      appendTo += wtApp->owmClient->getTextForIcon(forecast->icon);
    } else if (f.id.equalsIgnoreCase("long desc.")) {
      appendTo += wtApp->owmClient->getTextForIcon(forecast->icon);
    }
    first = false;
  }
}


/*------------------------------------------------------------------------------
 *
 * FSSettings Implementation
 *
 *----------------------------------------------------------------------------*/

FSSettings::FSSettings() {
  maxFileSize = 512;
  version = 1;
  init("/wta/FSSettings.json");
}

void FSSettings::fromJSON(const JsonDocument &doc) {
  heading.clear();
  fields.clear();

  Field field;

  const JsonArrayConst& jsonHeading = doc["heading"].as<JsonArray>();
  for (const auto& jsonField : jsonHeading) {
     field.enabled = jsonField["on"];
     field.id = jsonField["id"].as<String>();
     heading.push_back(field);
  }

  label = doc["label"].as<String>();

  const JsonArrayConst& jsonFields = doc["fields"].as<JsonArray>();
  for (const auto& jsonField : jsonFields) {
     field.enabled = jsonField["on"];
     field.id = jsonField["id"].as<String>();
     fields.push_back(field);
  }
}

void FSSettings::toJSON(JsonDocument &doc) {
  JsonArray jsonHeading = doc.createNestedArray("heading");
  for (Field& field : heading) {
    JsonObject jsonField = jsonHeading.createNestedObject();
    jsonField["id"] = field.id;
    jsonField["on"] = field.enabled;
  }

  doc["label"] = label;

  JsonArray jsonFields = doc.createNestedArray("fields");
  for (Field& field : fields) {
    JsonObject jsonField = jsonFields.createNestedObject();
    jsonField["id"] = field.id;
    jsonField["on"] = field.enabled;
  }
}

void FSSettings::logSettings() {
  Log.verbose(F("Forecast Screen Settings"));
  Log.verbose(F("  Heading"));
  for (const Field& field : heading) {
    Log.verbose(F("    %s, enabled: %T"), field.id.c_str(), field.enabled);
  }
  Log.verbose(F("    Label: %s"), label.c_str());
  Log.verbose(F("  Fields"));
  for (const Field& field : fields) {
    Log.verbose(F("    %s, enabled: %T"), field.id.c_str(), field.enabled);
  }
}



#endif






