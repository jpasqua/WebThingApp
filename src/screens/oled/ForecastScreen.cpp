
/*
 * ForecastScreen:
 *    Display up to 3 days of the forecast for the selected city 
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
#include "ForecastScreen.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

void ForecastScreen::display(bool) {
  auto& oled = Display.oled;

  // We should not get here if owm is disabled or we were unable to contact the
  // OWM service, but just double check...
  if (!wtApp->owmClient) return;

  oled->clear();
  Forecast* forecast = wtApp->owmClient->getForecast();

  uint8_t daysToDisplay = startingDay == 0 ? 3 : 2;
  for (int i = 0; i < daysToDisplay; i++) {
    drawForecastDetails(&forecast[startingDay+i], i*44);
  }

  Display.setFont(Display.FontID::S10);
  oled->setTextAlignment(TEXT_ALIGN_LEFT);
  String& city = wtApp->settings->owmOptions.nickname;
  if (city.isEmpty()) { city = wtApp->owmClient->weather.location.city; }
  oled->drawString(0, 52, city);

  oled->setTextAlignment(TEXT_ALIGN_RIGHT);
  char curTempBuf[12];
  snprintf(
      curTempBuf, 12, "Now: %.0f%s",
      wtApp->owmClient->weather.readings.temp,
      (wtApp->settings->uiOptions.useMetric ? "°C" : "°F"));
  oled->drawString(Display.Width-1, 52, curTempBuf);

  oled->display();
}

void ForecastScreen::processPeriodicActivity() {
  // Nothing to do here...
}

// ----- Private Methods

static char dayBuf[] = "DDD"; // A 3 character buf with null termination

void ForecastScreen::drawForecastDetails(Forecast* forecast, int xOff) {
  auto& oled = Display.oled;

  memcpy(dayBuf, dayStr(weekday(forecast->dt)), 3);
  strupr(dayBuf);

  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  Display.setFont(Display.FontID::S10);
  oled->drawString(xOff +20,  0, dayBuf);

  Display.setFont(Display.FontID::W21);
  oled->drawString(xOff + 20, 12, WeatherScreen::getGlyphForIcon(forecast->icon));

  Display.setFont(Display.FontID::S10);
  char hiLoBuf[8];
  snprintf(hiLoBuf, 8, "%.0f|%.0f", forecast->loTemp, forecast->hiTemp);
  oled->drawString(xOff + 20, 34, hiLoBuf);
}

#endif






