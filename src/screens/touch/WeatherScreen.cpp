/*
 * WeatherScreen:
 *    Display weather info for the selected city and a running clock. 
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
#include "../../WTAppImpl.h"
#include "../../gui/Display.h"
#include "../../gui/Theme.h"
#include "../../gui/ScreenMgr.h"
#include "WeatherScreen.h"
#include "WeatherIcons.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * CONSTANTS
 *
 *----------------------------------------------------------------------------*/

static constexpr auto TimeFont = Display.FontID::D20;
static constexpr uint16_t TimeFontWidth = 17;
static constexpr uint16_t TimeFontHeight = 22;
static constexpr uint16_t TimeFontColonWidth = 9;

static constexpr auto TempUnitsFont = TimeFont;
static constexpr uint16_t TempUnitsFontWidth = TimeFontWidth;
static constexpr uint16_t TempUnitsFontHeight = TimeFontHeight;

static constexpr auto TempFont = Display.FontID::D72;
static constexpr uint16_t TempFontWidth = 60;
static constexpr uint16_t TempFontHeight = 72;

static constexpr auto ReadingsFont = Display.FontID::S12;
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

  buttonHandler =[](int id, PressType type) -> void {
    Log.verbose(F("In WeatherScreen Label Handler, id = %d, type = %d"), id, type);
    if (type > PressType::Normal) ScreenMgr.displayHomeScreen();
    else ScreenMgr.display(wtAppImpl->screens.forecastScreen);
  };

  nLabels = 1;
  labels = new Label[nLabels];
  labels[0].init(0, 0, Display.Width, Display.Height, 0);
}


void WeatherScreen::display(bool) {  
  // We should not get here if owm is disabled or we were unable to contact the
  // OWM service, but just double check...
  if (!wtApp->owmClient) return;

  auto& tft = Display.tft;
  auto& weather = wtApp->owmClient->weather;
  
  bool metric = wtApp->settings->uiOptions.useMetric;

  tft.fillScreen(Theme::Color_WeatherBkg);
  if (weather.dt == 0) {
    Display.setFont(Display.FontID::SB18);
    tft.setTextColor(Theme::Color_AlertError);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(F("No Weather Data"), Display.XCenter, Display.YCenter);
    lastDT = weather.dt;
      // Don't keep flashing "No Weather Data". It will remain dispayed
      // until owmClient->weather.dt is updated
    return;
  }

  // ----- Draw Summary line at the top of the screen
  Display.setFont(Display.FontID::SB12);
  tft.setTextColor(Theme::Color_WeatherTxt);
  tft.setTextDatum(TL_DATUM);
  String& city = wtApp->settings->owmOptions.nickname;
  if (city.isEmpty()) { city = weather.location.city; }
  tft.drawString(city, XTopAreaInset, YTopArea);
  showTime();

  // ----- Draw the central display area
  float temp = weather.readings.temp;

  tft.pushImage(ImageInset, YCentralArea, WI_Width, WI_Height,
      getWeatherIcon(weather.description.icon), WI_Transparent);
  tft.pushImage(Display.Width-WI_Width-ImageInset, YCentralArea, WindIcon_Width, WindIcon_Height,
      getWindIcon(weather.readings.windSpeed), WI_Transparent);

  int textOffset = (WindIcon_Height-TempFontHeight)/2;
  Display.setFont(TempFont);
  tft.setTextColor(Theme::Color_Progress);
  tft.setTextDatum(TL_DATUM);
  int nDigits = temp < 10 ? 1 : (temp < 100 ? 2 : 3);
  int xLoc = Display.XCenter - ((nDigits*TempFontWidth+TempUnitsFontWidth)/2);
  tft.drawString(String((int)temp), xLoc, YCentralArea-textOffset);
  Display.setFont(TimeFont);
  tft.drawString(
      (metric ? "C" : "F"),
      xLoc+(nDigits*TempFontWidth),
      YCentralArea-textOffset+TempFontHeight-TempUnitsFontHeight);

  tft.setTextDatum(TC_DATUM);
  char firstChar = weather.description.longer[0];
  if (isLowerCase(firstChar)) {
    weather.description.longer[0] = toUpperCase(firstChar);
  }
  tft.setTextColor(Theme::Color_WeatherTxt);
  Display.setFont(ReadingsFont);
  tft.drawString(
      weather.description.longer,
      Display.XCenter,YCentralArea-textOffset+TempFontHeight + 5); // A little spacing in Y

  // Readings Area
  tft.setTextColor(Theme::Color_WeatherTxt);
  Display.setFont(ReadingsFont);
  tft.setTextDatum(TL_DATUM);
  String reading = "Humidty: " + String(weather.readings.humidity) + "%";
  tft.drawString(reading, XTopAreaInset, YReadingsArea);
  tft.setTextDatum(TR_DATUM);
  float pressure = weather.readings.pressure;
  if (!metric) pressure = Basics::hpa_to_inhg(pressure);
  reading = String(pressure) +  (metric ? "hPa" : "inHG"),
  tft.drawString(reading, Display.Width - XTopAreaInset, YReadingsArea);

  // NOTE: For some reason visibility seems to ignore the units setting and always return meters!!
  uint16_t visibility = (weather.readings.visibility);
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
  float feelsLike = weather.readings.feelsLike;

  units = metric ? "C" : "F";
  reading = "Feels " + String((int)(feelsLike+0.5)) +  units;
  tft.setTextColor(Theme::Color_Progress);
  Display.setFont(Display.FontID::SB12);
  tft.drawString(reading, Display.Width - XTopAreaInset, YReadingsArea+ReadingsFontHeight);

  lastDT = weather.dt;
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

  Display.setSpriteFont(TimeFont);
  sprite->setTextColor(Theme::Mono_Foreground);
  sprite->setTextDatum(TR_DATUM);
  sprite->drawString(
      WebThing::formattedTime(wtApp->settings->uiOptions.use24Hour, false),
      TimeDisplayWidth-1-XTopAreaInset, 0);

  sprite->setBitmapColor(Theme::Color_WeatherTxt, Theme::Color_WeatherBkg);
  sprite->pushSprite(Display.Width - TimeDisplayWidth, YTopArea);
  sprite->deleteSprite();
}

#endif






