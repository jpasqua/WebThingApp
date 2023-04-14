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

  String& city = wtApp->settings->owmOptions.nickname;
  if (city.isEmpty()) { city = weather.location.city; }

  weatherText += city;
  weatherText += " ";
  weatherText += String(weather.readings.temp, 0);
  weatherText += (wtApp->settings->uiOptions.useMetric ? "C" : "F");
  weatherText += ' ';
  weatherText += getTextForIcon(weather.description.icon);
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

#endif






