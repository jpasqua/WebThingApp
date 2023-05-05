
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
  _forecastText = city;
  _forecastText += " Forecast: ";

  for (int i = 0; i < wtApp->owmClient->ForecastElements; i++) {
    if (i) _forecastText += ", ";
    appendForecastForDay(&forecast[i], _forecastText);
  }

  setText(_forecastText, Display.BuiltInFont_ID);
  _timeOfLastForecast = wtApp->owmClient->timeOfLastForecastUpdate();
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
  appendTo += WeatherScreen::getTextForIcon(forecast->icon);
}

#endif






