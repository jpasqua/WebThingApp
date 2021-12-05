/*
 * WeatherScreen:
 *    Display weather info for the selected city 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED

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

}


void WeatherScreen::display(bool) {
  // We should not get here if owm is disabled or we were unable to contact the
  // OWM service, but just double check...
  if (!wtApp->owmClient) return;


  auto& oled = Display.oled;
  auto& weather = wtApp->owmClient->weather;

  oled->clear();
  if (weather.dt == 0) {
    Display.setFont(Display.FontID::S16);
    oled->setColor(OLEDDISPLAY_COLOR::WHITE);
    oled->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    oled->drawString(Display.XCenter, Display.YCenter, F("No Weather Data"));
    lastDT = weather.dt;
      // Don't keep flashing "No Weather Data". It will remain dispayed
      // until owmClient->weather.dt is updated
    oled->display();
    return;
  }


  bool metric = wtApp->settings->uiOptions.useMetric;

  Display.setFont(Display.FontID::S10);
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  oled->drawString(Display.XCenter, 38, weather.description.longer);

  Display.setFont(Display.FontID::S24);
  oled->setTextAlignment(TEXT_ALIGN_LEFT);
  String temp = String(weather.readings.temp, 0) + (metric ? "°C" : "°F");
  oled->drawString(60, 5, temp);

  Display.setFont(Display.FontID::W36);
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  oled->drawString(Display.XCenter/2, 0, getGlyphForIcon(weather.description.icon));

  Display.setFont(Display.FontID::S10);
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  String& city = wtApp->settings->owmOptions.nickname;
  if (city.isEmpty()) { city = weather.location.city; }
  oled->drawString(Display.XCenter, 52, city);

  oled->display();

  lastDT = weather.dt;
}

void WeatherScreen::processPeriodicActivity() {
  if (!wtApp->owmClient) return;
  if (lastDT != wtApp->owmClient->weather.dt) display();
}


// ----- Static methods

String WeatherScreen::getGlyphForIcon(String& icon) {
  // clear sky
  if (icon == "01d")  { return "B"; }
  if (icon == "01n")  { return "C"; }
  // few clouds
  if (icon == "02d")  { return "H"; }
  if (icon == "02n")  { return "4"; }
  // scattered clouds
  if (icon == "03d")  { return "N"; }
  if (icon == "03n")  { return "5"; }
  // broken clouds
  if (icon == "04d")  { return "Y"; }
  if (icon == "04n")  { return "%"; }
  // shower rain
  if (icon == "09d")  { return "R"; }
  if (icon == "09n")  { return "8"; }
  // rain
  if (icon == "10d")  { return "Q"; }
  if (icon == "10n")  { return "7"; }
  // thunderstorm
  if (icon == "11d")  { return "P"; }
  if (icon == "11n")  { return "6"; }
  // snow
  if (icon == "13d")  { return "W"; }
  if (icon == "13n")  { return "#"; }
  // haze
  if (icon == "50d")  { return "M"; }
  if (icon == "50n")  { return "M"; }
  // Nothing matched: N/A
  return ")";
}

#endif






