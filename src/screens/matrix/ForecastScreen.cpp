
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
//                                  WebThing Includes
#include <BPABasics.h>
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

  if (settings.showCity) {
    if (settings.cityBeforeHeading) {
      _forecastText = city;               _forecastText += ' ';
      _forecastText += settings.heading;  _forecastText += ' ';
    }
    else {
      _forecastText = settings.heading;   _forecastText += ' ';
      _forecastText += city;              _forecastText += ' ';
    }
  } else {
    _forecastText = settings.heading;     _forecastText += ' ';
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
  char* dayAsString = dayStr(weekday(forecast->dt));
  appendTo += dayAsString[0];
  appendTo += dayAsString[1];
  appendTo += dayAsString[2];
  appendTo += ' ';
  appendTo += (int)forecast->loTemp;
  appendTo += '/';
  appendTo += (int)forecast->hiTemp;
  appendTo += ' ';
  appendTo += wtApp->owmClient->getTextForIcon(forecast->icon);
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

  showCity = true;
  cityBeforeHeading = true;
  String heading = "Forecast";
}

void FSSettings::fromJSON(const JsonDocument &doc) {
  JsonObjectConst fsScreen = doc["fsScreen"];
  showCity = fsScreen["showCity"] | true;
  cityBeforeHeading = fsScreen["cityBeforeHeading"] | true;
  heading = fsScreen["heading"].as<String>();
}

void FSSettings::toJSON(JsonDocument &doc) {
  JsonObject fsScreen = doc.createNestedObject("fsScreen");
  fsScreen["showCity"] = showCity;
  fsScreen["cityBeforeHeading"] = cityBeforeHeading;
  fsScreen["heading"] = heading;
}

void FSSettings::logSettings() {
  Log.verbose(F("Forecast Screen Config"));
  Log.verbose(F("  Show City: %T"), showCity);
  Log.verbose(F("  City Before Heading: %T"), cityBeforeHeading);
  Log.verbose(F("  Heading: %s"), heading.c_str());
}



#endif






