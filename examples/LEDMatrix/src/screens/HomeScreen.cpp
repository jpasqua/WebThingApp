#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <TimeLib.h>
//                                  WebThingApp
#include <gui/Display.h>
#include <gui/Theme.h>
//                                  Local Includes
#include "../../LEDMatrixApp.h"
//--------------- End:    Includes ---------------------------------------------


static inline uint16_t compose(int h, int m) { return(h * 100 + m); }

HomeScreen::HomeScreen() { }

void HomeScreen::display(bool activating) {
  auto mtx = Display.mtx;

  _colonVisible = false;
  mtx->fillScreen(Theme::Color_BLACK);

  int  m = minute();
  int  h = hour();
  _compositeTime = compose(h, m);

  char timeBuf[5];
  if (lmSettings->uiOptions.use24Hour) {
    timeBuf[0] = (h/10) + '0';
    timeBuf[1] = (h%10) + '0';
    timeBuf[2] = (m/10) + '0';
    timeBuf[3] = (m%10) + '0';
    timeBuf[4] = ' ';
  } else {
    bool am = true;
    if (h > 12) { h -= 12; am = false; }
    else if (h == 0) { h = 12;}
    else if (h == 12) { am = false; }
    timeBuf[0] = h < 10 ? ' ' : '1';
    timeBuf[1] = (h%10) + '0';
    timeBuf[2] = (m/10) + '0';
    timeBuf[3] = (m%10) + '0';
    timeBuf[4] = am ? 'A' : 'P';
  }

  mtx->drawChar( 0, 0, timeBuf[0], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  mtx->drawChar( 6, 0, timeBuf[1], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  mtx->drawChar(14, 0, timeBuf[2], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  mtx->drawChar(20, 0, timeBuf[3], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  mtx->drawChar(26, 0, timeBuf[4], Theme::Color_WHITE, Theme::Color_BLACK, 1);
  toggleColon();

  if ((mtx->width() >= 64) && (lmApp->owmClient != nullptr)) {
    constexpr int charWidthIncludingSpace = 6;
    int temp = lmApp->owmClient->weather.readings.temp; // Rounds the value
    int nChars = 1; // For the 'C' or 'F'
    nChars += temp < 0 ? 1 : 0; // Add space for the minus sign if needed
    nChars += temp > 99 ? 2 : (temp > 9 ? 2 : 1); // Space for the digits
    uint16_t x = mtx->width() - (nChars * charWidthIncludingSpace);
    uint16_t y = 0;
    mtx->setCursor(x, y);
    mtx->print(temp);
    mtx->print(lmApp->settings->uiOptions.useMetric ? 'C' : 'F');
  }
  mtx->write();
  _displayStartedAt = millis();
}

void HomeScreen::processPeriodicActivity() {
  static uint32_t _colonLastToggledAt = 0;
  uint32_t curMillis = millis();
  if (curMillis > _displayStartedAt + _homeScreenDisplayTime) {
    ScreenMgr.moveThroughSequence(true);
    return;
  }
  if (_compositeTime != compose(hour(), minute())) {
    display(true);
    _colonLastToggledAt = curMillis;
    return;
  }
  if (curMillis >= _colonLastToggledAt + 1000L) {
    toggleColon();
    Display.mtx->write();
    _colonLastToggledAt = curMillis;
  }
}


// ----- Private Functions

void HomeScreen::toggleColon() {
  _colonVisible = !_colonVisible;
  Display.mtx->writePixel(12, 2, _colonVisible);
  Display.mtx->writePixel(12, 4, _colonVisible);
}

#endif