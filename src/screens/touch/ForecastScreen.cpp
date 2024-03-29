/*
 * ForecastScreen:
 *    Display the 5-day forecast. Since we are arranging the readings in two columns
 *    with the 3 forecast cells each, we've got an extra spot. Make the first spot
 *    the current readings and the next 5 spots the 5-day forecast.
 *
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_TOUCH

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
//                                  Local Includes
#include "../../WTApp.h"
#include "../../clients/OWMClient.h"
#include "../../gui/Display.h"
#include "../../gui/Theme.h"
#include "../../gui/ScreenMgr.h"
#include "ForecastScreen.h"
#include "WeatherIcons.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * CONSTANTS
 *
 *----------------------------------------------------------------------------*/

static constexpr auto ReadingsFont = Display.FontID::SB9;
static constexpr uint16_t ReadingsFontHeight = 22;  // ReadingsFont->yAdvance;

static constexpr uint16_t TileWidth = Display.Width/2;
static constexpr uint16_t TileHeight = Display.Height/3;
static constexpr uint16_t TextVPad = (TileHeight - (2*ReadingsFontHeight))/2;

static constexpr int16_t TinyFont = 2;  // A small 5x7 font

/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

ForecastScreen::ForecastScreen() {

  buttonHandler =[&](int id, PressType type) -> void {
    Log.verbose(F("In ForecastScreen Label Handler, id = %d, type = %d"), id, type);
    if (type > PressType::Normal) ScreenMgr.display("Weather");
    ScreenMgr.displayHomeScreen();
  };

  labels = new Label[(nLabels = 1)];
  labels[0].init(0, 0, Display.Width, Display.Height, 0);
}


void ForecastScreen::display(bool) {
  if (!wtApp->owmClient) return; // Make sure we've got an OpenWeatherMap client

  Display.tft.fillScreen(Theme::Color_WeatherBkg);
  uint16_t x = 0, y = 0;

  // The first element of the forecast display is really the current temperature
  Forecast current;
  current.dt = wtApp->owmClient->weather.dt + WebThing::getGMTOffset();
  current.hiTemp = wtApp->owmClient->weather.readings.temp;
  current.loTemp = Forecast::NoReading;
  current.icon = wtApp->owmClient->weather.description.icon;
  displaySingleForecast(&current, x, y);
  y += TileHeight;

  Forecast *f = wtApp->owmClient->getForecast();
  for (int i = 0; i < OWMClient::ForecastElements; i++) {
    displaySingleForecast(&f[i], x, y);
    if (i == 1) x += TileWidth;
    y = (y + TileHeight) % Display.Height;
  }
}

void ForecastScreen::processPeriodicActivity() {
  // Nothing to do here...
}

void ForecastScreen::displaySingleForecast(Forecast* f, uint16_t x, uint16_t y) {
  auto& tft = Display.tft;
  
  tft.pushImage(
      x, y+((TileHeight-WI_Height)/2),
      WI_Width, WI_Height,
      getWeatherIcon(f->icon),
      WI_Transparent);
  x += WI_Width;

  tft.setTextColor(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  String reading = "";
  String units = wtApp->settings->uiOptions.useMetric ? "C" : "F";

  if (f->loTemp != Forecast::NoReading) reading = String((int)(f->loTemp+0.5)) + " / ";
  reading += String((int)(f->hiTemp+0.5)) + units;

  y += TextVPad;
  tft.setTextColor(TFT_BLACK);
  Display.setFont(ReadingsFont);
  tft.drawString(reading, x, y);

  y += ReadingsFontHeight;
  reading = dayShortStr(weekday(f->dt));
  int h = hour(f->dt);
  bool pm = isPM(f->dt);
  if (!wtApp->settings->uiOptions.use24Hour) {
    if (pm) h -= 12;
    if (h == 0) h = 12;
    reading += " " + String(h);
  } else {
    reading += " ";
    if (h < 10) reading += "0";
    reading += String(h);
  }

  x += tft.drawString(reading, x, y) + 1;
  if (wtApp->settings->uiOptions.use24Hour) {
    reading = ":00";
  } else {
    reading = (pm ? "PM" : "AM");
  }
  tft.drawString(reading, x+1, y+1, TinyFont);
}

#endif