/*
 * HomeScreen:
 *    This is the "home" screen. It displays the time, some weather information,
 *    and currency conversion data and controls
 *
 */


//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <TimeLib.h>
//                                  WebThing Includes
#include <GenericESP.h>
#include <gui/Display.h>
#include <gui/Theme.h>
#include <gui/ScreenMgr.h>
#include <screens/touch/EnterNumberScreen.h>
//                                  Local Includes
#include "../../CurrencyMonApp.h"
#include "../hardware/PhysicalButtons.h"
#include "HomeScreen.h"
//--------------- End:    Includes ---------------------------------------------


// ----- Coordinates of the various graphical elements
// The weather area is on top, the labels are on the bottom,
// and the clock area is defined to be the space in between

/*
  ASCII art HomeScreen layout:

      +--------------------------------------------+
      |    City Name      Temp      Description    |
      |             High: 78, Low: 65              |
      |              11      2222   33333          |
      |              11 ::  2   2        3         |
      |              11        2       333         |
      |              11 ::   2           3         |
      |              11    222222   33333          |
      | ------------------------------------------ |
      | ------------------------------------------ |
      | || Currency1  || Currency2|| Currency3  || |
      | ------------------------------------------ |
      | ------------------------------------------ |
      +--------------------------------------------+
*/

/*------------------------------------------------------------------------------
 *
 * CONSTANTS
 *
 *----------------------------------------------------------------------------*/

static constexpr float NotSet = -1;

static constexpr auto WeatherFont = Display.FontID::SB9;
static constexpr uint16_t WeatherFontHeight = 22;   // WeatherFont->yAdvance;
static constexpr uint16_t WeatherXOrigin = 0;
static constexpr uint16_t WeatherYOrigin = 0;
static constexpr uint16_t WeatherHeight = WeatherFontHeight;
static constexpr uint16_t WeatherWidth = Display.Width;

// FC is short for Forecast
static constexpr uint16_t FC_YOrigin = WeatherYOrigin + WeatherHeight + 2;
static constexpr uint16_t FC_Height = WeatherFontHeight;

// CB is short for "Currency Label"
// NOTE: The rightmost frame of Label[i] overlaps the leftmost frame
//       of Label[i+1]
static constexpr uint16_t CB_FrameSize = 2;                             // Size of the Frame
static constexpr uint16_t CB_Width = 106;                               // Includes Frame
static constexpr uint16_t CB_Height = 42;                               // Includes Frame
static constexpr uint16_t CB_XOrigin = 1;                               // X of origin of 1st currency button
static constexpr uint16_t CB_YOrigin = Display.Height - CB_Height;     // Y Origin of all currency labels
static constexpr auto CB_Font = Display.FontID::SB9;             // Font for currency value
static constexpr auto ButtonLabelFont = 2;                              // A small 5x7 font

static constexpr uint16_t ClockXOrigin = 0;                             // Starts at left edge of screen
static constexpr uint16_t ClockYOrigin = FC_YOrigin + FC_Height;        // Starts below the Playing area
static constexpr uint16_t ClockWidth = Display.Width;                  // Full width of the screen
static constexpr uint16_t ClockHeight = CB_YOrigin-ClockYOrigin;        // The space between the other 2 areas
static constexpr auto ClockFont = Display.FontID::D100;
static constexpr uint16_t ClockFontHeight = 109;    // ClockFont->yAdvance;

// Label Indices
static constexpr uint8_t Currency1Button = 0;
static constexpr uint8_t Currency2Button = Currency1Button + 1;
static constexpr uint8_t Currency3Button = Currency2Button + 1;
static constexpr uint8_t NCurrencyButtons = Currency3Button + 1;

static constexpr uint8_t WeatherAreaIndex = NCurrencyButtons;
static constexpr uint8_t ClockAreaIndex = WeatherAreaIndex + 1;
static constexpr uint8_t NTouchButtons = ClockAreaIndex + 1;

static constexpr uint8_t AdvanceButton = NTouchButtons;


/*------------------------------------------------------------------------------
 *
 * Constructors and Public methods
 *
 *----------------------------------------------------------------------------*/

HomeScreen::HomeScreen() {

  buttonHandler =[this](uint8_t id, PressType type) -> void {
    Log.verbose(F("In HomeScreen Label Handler, id = %d"), id);

    if (type > PressType::Normal) {
      String subheading = "Heap: Free/Frag = ";
      String subcontent = String(ESP.getFreeHeap()) + ", " + String(GenericESP::getHeapFragmentation()) + "%"; 
      wtAppImpl->screens.utilityScreen->setSub(subheading, subcontent);
      ScreenMgr.display(wtAppImpl->screens.utilityScreen);
    } else if (id == ClockAreaIndex || id == AdvanceButton) {
      cmApp->pluginMgr.displayPlugin(0);
    } else if (id == WeatherAreaIndex) {
      ScreenMgr.display(wtAppImpl->screens.weatherScreen); 
    } else if ((id >= Currency1Button) && (id <= Currency3Button)) {
      // This   ^^^^^^^^^^^^^^^^^^^^^ comparison can cause a warning since id is
      // an unsigned int (at least 0) and Currency1Button == 0. Leave the test in place
      // since Currency1Button is an arbitrary constant and could change.
      activeCurrency = id - Currency1Button;
      if (cmApp->currencies[activeCurrency].inactive()) return;

      cmApp->numpad->init(
        "Amount", 1,
        ([this](float a) { updateAmounts(a); ScreenMgr.display(this); }),
        true, 0.0, 100000.0);
      ScreenMgr.display(cmApp->numpad);
    }
  };

  buttonMappings = new WTButton::Mapping[(nButtonMappings = 1)];
  buttonMappings[0] = {PhysicalButtons::Button01 , AdvanceButton};

  labels = new Label[(nLabels = NTouchButtons)];
  uint16_t x = CB_XOrigin;
  for (int i = 0; i < NCurrencyButtons; i++) {
    labels[i].init(x, CB_YOrigin, CB_Width, CB_Height, i);
    x += CB_Width - CB_FrameSize;
    displayedValues[i] = NotSet;
  }

  // Because the weather area is slim and at the top of the screen, we make a bigger button
  // right in the middle to make it easier to touch. It overlaps the Clock button area
  // but has priority as it is earlier in the button list
  labels[WeatherAreaIndex].init(
    0, WeatherYOrigin, Display.Width, /*WeatherHeight*/50, WeatherAreaIndex);
  labels[ClockAreaIndex].init(
    0, ClockYOrigin, Display.Width, ClockHeight, ClockAreaIndex);
}

void HomeScreen::display(bool activating) {
  if (activating) { Display.tft.fillScreen(Theme::Color_Background); }

  drawClock(activating);
  drawWeather(activating);
  drawCurrencyButtons(activating);
  drawCurrencyLabels(activating);
  nextUpdateTime = millis() + 1 * 1000L;
}

void HomeScreen::processPeriodicActivity() {
  if (millis() >= nextUpdateTime) display();
}

/*------------------------------------------------------------------------------
 *
 * Private methods
 *
 *----------------------------------------------------------------------------*/

void HomeScreen::updateAmounts(float newBase) {
  if (cmApp->currencies[activeCurrency].exchangeRate < .0001) {
    Log.verbose("HomeScreen::updateAmounts: avoid divide by zero error");
    return;
  }
  for (int i = 0; i < NCurrencyButtons; i++) {
    if (cmApp->currencies[i].active()) {
      displayedValues[i] = cmApp->currencies[i].exchangeRate * 
          (newBase/cmApp->currencies[activeCurrency].exchangeRate);
    }
  }
}

void HomeScreen::drawClock(bool force) {
  static int lastTimeDisplayed = -1;
  time_t  t = now();
  int     hr = hour(t);
  int     min = minute(t);
  auto&   sprite = Display.sprite;

  int compositeTime = hr * 100 + min;
  if (!force && (compositeTime == lastTimeDisplayed)) return;
  else lastTimeDisplayed = compositeTime;

  char timeString[6]; // HH:MM<NULL>

  if (!wtApp->settings->uiOptions.use24Hour) {
    if (hr >= 12) { hr -= 12; }
    if (hr == 0) { hr = 12; }
  }

  timeString[0] = (hr < 10) ? ' ' : (hr/10 + '0');
  timeString[1] = '0' + (hr % 10);
  timeString[2] = ':';
  timeString[3] = '0' + (min / 10);
  timeString[4] = '0' + (min % 10);
  timeString[5] = '\0';

  sprite->setColorDepth(1);
  sprite->createSprite(ClockWidth, ClockFontHeight);
  sprite->fillSprite(Theme::Mono_Background);

  Display.setSpriteFont(ClockFont);
  sprite->setTextColor(Theme::Mono_Foreground);
  // With this large font some manual "kerning" is required to make it fit
  uint16_t baseline = ClockFontHeight-1;
  sprite->setCursor(   0, baseline); sprite->print(timeString[0]);
  sprite->setCursor(  70, baseline); sprite->print(timeString[1]);
  sprite->setCursor( 145, baseline); sprite->print(timeString[2]);
  sprite->setCursor( 160, baseline); sprite->print(timeString[3]);
  sprite->setCursor( 230, baseline); sprite->print(timeString[4]);

  sprite->setBitmapColor(Theme::Color_AlertGood, Theme::Color_Background);
  uint16_t yPlacement = ClockYOrigin+((ClockHeight-ClockFontHeight)/2);
  yPlacement -= 10; // Having it perfectly centered doesn't look as good,
                    // especially when no "next completion time" is displayed
  sprite->pushSprite(ClockXOrigin, yPlacement);
  sprite->deleteSprite();
}

void HomeScreen::drawWeather(bool) {
  if (!wtApp->owmClient) { Log.verbose(F("owmClient = NULL")); return; }
  if (!wtApp->settings->owmOptions.enabled) return;
  String currentWeather;
  String outlook;
  auto& sprite = Display.sprite;

  sprite->setColorDepth(1);
  sprite->createSprite(WeatherWidth, WeatherHeight);
  sprite->fillSprite(Theme::Mono_Background);

  uint32_t textColor = Theme::Color_NormalText;
  if (wtAppImpl->owmClient->weather.dt == 0) {
    textColor = Theme::Color_AlertError;
    currentWeather = "No Weather Data";
    outlook = "";
  } else {
    if (wtApp->settings->owmOptions.nickname.isEmpty())
      currentWeather = wtApp->owmClient->weather.location.city;
    else
      currentWeather = wtApp->settings->owmOptions.nickname;
    currentWeather += ": ";
    currentWeather += String((int)(wtAppImpl->owmClient->weather.readings.temp+0.5));
    currentWeather += (wtApp->settings->uiOptions.useMetric) ? "C, " : "F, ";
    currentWeather += wtApp->owmClient->weather.description.longer;

    Forecast *f = wtApp->owmClient->getForecast();
    outlook = "High: ";
    outlook += String(f[0].hiTemp, 0);
    outlook += ", Low: ";
    outlook += String(f[0].loTemp, 0);
  }

  Display.setSpriteFont(WeatherFont);
  sprite->setTextColor(Theme::Mono_Foreground);
  sprite->setTextDatum(MC_DATUM);
  sprite->drawString(currentWeather, WeatherWidth/2, WeatherHeight/2);

  sprite->setBitmapColor(textColor, Theme::Color_Background);
  sprite->pushSprite(WeatherXOrigin, WeatherYOrigin);

  // Now draw the forecast
  sprite->fillSprite(Theme::Mono_Background);
  sprite->setTextColor(Theme::Mono_Foreground);
  sprite->setTextDatum(TC_DATUM);
  sprite->drawString(outlook, WeatherWidth/2, 0);

  sprite->setBitmapColor(textColor, Theme::Color_Background);
  sprite->pushSprite(WeatherXOrigin, FC_YOrigin);

  sprite->deleteSprite();
}


void HomeScreen::drawCurrencyLabels(bool force) {
  if (!force) return;

  uint16_t yPos = CB_YOrigin;
  uint16_t xDelta = Display.Width/NCurrencyButtons;
  uint16_t xPos = 0 + xDelta/2;
  Display.tft.setTextDatum(BC_DATUM);
  Display.tft.setTextColor(Theme::Color_NormalText);
  for (int i = 0; i < NCurrencyButtons; i++) {
    String label = cmSettings->currencies[i].nickname;
    uint32_t labelColor = Theme::Color_NormalText;

    if (cmApp->currencies[i].inactive()) {
      label = "Unused";
      labelColor = Theme::Color_DimText;
    } else {
      if (label.isEmpty()) label = cmSettings->currencies[i].id;
    }

    Display.tft.setTextColor(labelColor);
    Display.tft.drawString(label, xPos, yPos, ButtonLabelFont);      
    xPos += xDelta;
  }
}

void HomeScreen::drawCurrencyButtons(bool) {
  for (int i = 0; i < NCurrencyButtons; i++) {
    uint32_t textColor = Theme::Color_NormalText;
    uint32_t bgColor = Theme::Color_Background;
    String value;

    if (cmApp->currencies[i].active()) {
      if (displayedValues[i] == NotSet) displayedValues[i] = cmApp->currencies[i].exchangeRate;
      value = String(displayedValues[i], 2);
      if (i == activeCurrency) textColor = Theme::Color_AlertGood;
    } else {
      bgColor = Theme::Color_Inactive;
      value = "N/A";
    }
    labels[i].drawSimple(
      value, CB_Font, CB_FrameSize,
      textColor, Theme::Color_Border, bgColor, true);
  }
}

