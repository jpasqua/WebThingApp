/*
 * WeatherScreen:
 *    Display weather info for the selected city and a running clock. 
 *                    
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
//                                  Local Includes
#include "../WTApp.h"
#include "../WTAppImpl.h"
#include "../gui/Display.h"
#include "../gui/Theme.h"
#include "../gui/ScreenMgr.h"
#include "WeatherScreen.h"
#include "WeatherIcons.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * CONSTANTS
 *
 *----------------------------------------------------------------------------*/

static constexpr auto TimeFont = Display.fonts.FontID::D20;
static constexpr uint16_t TimeFontWidth = 17;
static constexpr uint16_t TimeFontHeight = 22;
static constexpr uint16_t TimeFontColonWidth = 9;

static constexpr auto TempUnitsFont = TimeFont;
static constexpr uint16_t TempUnitsFontWidth = TimeFontWidth;
static constexpr uint16_t TempUnitsFontHeight = TimeFontHeight;

static constexpr auto TempFont = Display.fonts.FontID::D72;
static constexpr uint16_t TempFontWidth = 60;
static constexpr uint16_t TempFontHeight = 72;

static constexpr auto ReadingsFont = Display.fonts.FontID::S12;
static constexpr uint16_t ReadingsFontHeight = 29;


static constexpr uint16_t YCentralArea = 50;
static constexpr uint16_t ImageInset = 2;
static constexpr uint16_t YTopArea = 3;
static constexpr uint16_t XTopAreaInset = 3;
static constexpr uint16_t YTopAreaHeight = 22;
static constexpr uint16_t YReadingsArea = Display.Height - 2*ReadingsFontHeight;

static constexpr uint16_t TimeDisplayWidth = 6*TimeFontWidth + 2*TimeFontColonWidth;


/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

WeatherScreen::WeatherScreen() {

  auto buttonHandler =[](int id, Button::PressType type) -> void {
    Log.verbose(F("In WeatherScreen Button Handler, id = %d, type = %d"), id, type);
    if (type > Button::PressType::NormalPress) ScreenMgr.displayHomeScreen();
    else ScreenMgr.display(wtAppImpl->forecastScreen);
  };

  nButtons = 1;
  buttons = new Button[nButtons];
  buttons[0].init(0, 0, Display.Width, Display.Height, buttonHandler, 0);
}


void WeatherScreen::display(bool) {  
  // We should not get here if owm is disabled or we were unable to contact the
  // OWM service, but just double check...
  if (!wtApp->owmClient) return;

  auto& tft = Display.tft;
  bool metric = wtApp->settings->uiOptions.useMetric;

  tft.fillScreen(Theme::Color_WeatherBkg);
  if (wtApp->owmClient->weather.dt == 0) {
    Display.fonts.setUsingID(Display.fonts.FontID::SB18, tft);
    tft.setTextColor(Theme::Color_AlertError);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(F("No Weather Data"), Display.XCenter, Display.YCenter);
    return;
  }

  // ----- Draw Summary line at the top of the screen
  Display.fonts.setUsingID(Display.fonts.FontID::SB12, tft);
  tft.setTextColor(Theme::Color_WeatherTxt);
  tft.setTextDatum(TL_DATUM);
  String& city = wtApp->settings->owmOptions.nickname;
  if (city.isEmpty()) { city = wtApp->owmClient->weather.location.city; }
  tft.drawString(city, XTopAreaInset, YTopArea);
  showTime();

  // ----- Draw the central display area
  float temp = wtApp->owmClient->weather.readings.temp;

  tft.pushImage(ImageInset, YCentralArea, WI_Width, WI_Height,
      getWeatherIcon(wtApp->owmClient->weather.description.icon), WI_Transparent);
  tft.pushImage(Display.Width-WI_Width-ImageInset, YCentralArea, WindIcon_Width, WindIcon_Height,
      getWindIcon(wtApp->owmClient->weather.readings.windSpeed), WI_Transparent);

  int textOffset = (WindIcon_Height-TempFontHeight)/2;
  Display.fonts.setUsingID(TempFont, tft);
  tft.setTextColor(Theme::Color_Progress);
  tft.setTextDatum(TL_DATUM);
  int nDigits = temp < 10 ? 1 : (temp < 100 ? 2 : 3);
  int xLoc = Display.XCenter - ((nDigits*TempFontWidth+TempUnitsFontWidth)/2);
  tft.drawString(String((int)temp), xLoc, YCentralArea-textOffset);
  Display.fonts.setUsingID(TimeFont, tft);
  tft.drawString(
      (metric ? "C" : "F"),
      xLoc+(nDigits*TempFontWidth),
      YCentralArea-textOffset+TempFontHeight-TempUnitsFontHeight);

  tft.setTextDatum(TC_DATUM);
  char firstChar = wtApp->owmClient->weather.description.longer[0];
  if (isLowerCase(firstChar)) {
    wtApp->owmClient->weather.description.longer[0] = toUpperCase(firstChar);
  }
  tft.setTextColor(Theme::Color_WeatherTxt);
  Display.fonts.setUsingID(ReadingsFont, tft);
  tft.drawString(
      wtApp->owmClient->weather.description.longer,
      Display.XCenter,YCentralArea-textOffset+TempFontHeight + 5); // A little spacing in Y

  // Readings Area
  tft.setTextColor(Theme::Color_WeatherTxt);
  Display.fonts.setUsingID(ReadingsFont, tft);
  tft.setTextDatum(TL_DATUM);
  String reading = "Humidty: " + String(wtApp->owmClient->weather.readings.humidity) + "%";
  tft.drawString(reading, XTopAreaInset, YReadingsArea);
  tft.setTextDatum(TR_DATUM);
  float pressure = wtApp->owmClient->weather.readings.pressure;
  if (!metric) pressure = Basics::hpa_to_inhg(pressure);
  reading = String(pressure) +  (metric ? "hPa" : "inHG"),
  tft.drawString(reading, Display.Width - XTopAreaInset, YReadingsArea);

  // NOTE: For some reason visibility seems to ignore the units setting and always return meters!!
  uint16_t visibility = (wtApp->owmClient->weather.readings.visibility);
  String units = "km";
  if (metric) { visibility /= 1000;  }
  else {
    units = "mi";
    visibility = (uint16_t)Basics::km_to_m( ((float)(visibility+500)) / 1000.0);
  }
  reading = "Visibility: " + String(visibility) +  units;
  tft.setTextDatum(TL_DATUM);
  tft.drawString(reading, XTopAreaInset, YReadingsArea+ReadingsFontHeight);

  tft.setTextDatum(TR_DATUM);
  float feelsLike = wtApp->owmClient->weather.readings.feelsLike;

  units = metric ? "C" : "F";
  reading = "Feels " + String((int)(feelsLike+0.5)) +  units;
  tft.setTextColor(Theme::Color_Progress);
  Display.fonts.setUsingID(Display.fonts.FontID::SB12, tft);
  tft.drawString(reading, Display.Width - XTopAreaInset, YReadingsArea+ReadingsFontHeight);

  lastDT = wtApp->owmClient->weather.dt;
}

void WeatherScreen::processPeriodicActivity() {
  if (lastDT != wtApp->owmClient->weather.dt) { display(); return; }
  if (millis() - lastClockUpdate >= 1000L) { showTime(); return; }
}


/*------------------------------------------------------------------------------
 *
 * Private methods
 *
 *----------------------------------------------------------------------------*/

void WeatherScreen::showTime() {
  auto& sprite = Display.sprite;
  
  sprite->setColorDepth(1);
  sprite->createSprite(TimeDisplayWidth, TimeFontHeight);
  sprite->fillSprite(Theme::Mono_Background);

  lastClockUpdate = millis();

  Display.fonts.setUsingID(TimeFont, sprite);
  sprite->setTextColor(Theme::Mono_Foreground);
  sprite->setTextDatum(TR_DATUM);
  sprite->drawString(
      WebThing::formattedTime(wtApp->settings->uiOptions.use24Hour, false),
      TimeDisplayWidth-1-XTopAreaInset, 0);

  sprite->setBitmapColor(Theme::Color_WeatherTxt, Theme::Color_WeatherBkg);
  sprite->pushSprite(Display.Width - TimeDisplayWidth, YTopArea);
  sprite->deleteSprite();
}








