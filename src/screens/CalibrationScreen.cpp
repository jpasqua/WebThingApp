/*
 * CalibrationScreen:
 *    Provides a way to calibrate the touch sensor on the screen
 *                    
 * NOTES:
 * o When the calibration process takes place, a new set of calibration
 *   data is generated. That data is sent to the Display handler and applied
 *   immediately.
 * o The settings object is updated with the new calibration data and saved
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <TimeLib.h>
//                                  WebThing Includes
//                                  Local Includes
#include "../WTApp.h"
#include "../gui/Display.h"
#include "../gui/Theme.h"
#include "../gui/ScreenMgr.h"
#include "CalibrationScreen.h"
//--------------- End:    Includes ---------------------------------------------

using Display::tft;


CalibrationScreen::CalibrationScreen() {
  init();
}

void CalibrationScreen::display(bool activating) {
  if (activating) state = pre;

  tft.fillScreen(Theme::Color_Background);
  if (state == pre) {
    Display::Font::setUsingID(Display::Font::FontID::SB9, tft);
    tft.setTextColor(Theme::Color_AlertGood);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(
      F("Touch to begin calibration"), Display::XCenter, Display::YCenter);
  } else if (state == post) {
    Display::Font::setUsingID(Display::Font::FontID::SB9, tft);
    tft.setTextColor(Theme::Color_AlertGood);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(F("Done! Touch to continue"), Display::XCenter, Display::YCenter);
    state = complete;
  }
}

void CalibrationScreen::processPeriodicActivity() {
  if (state == post) { display(); }
}

void CalibrationScreen::init() {

  auto buttonHandler =[this](int id, Button::PressType) -> void {
    Log.verbose(F("In CalibrationScreenButtonHandler, id = %d"), id);
    switch (state) {
      case pre:
        tft.fillScreen(Theme::Color_Background);
        Display::Font::setUsingID(Display::Font::FontID::SB9, tft);
        tft.setTextColor(Theme::Color_AlertGood);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(
          F("Touch each corner arrow"), Display::XCenter, Display::YCenter);
        tft.calibrateTouch(
          newCalibrationData.readings, TFT_WHITE, TFT_BLACK, 15);
        state = post;
        break;
      case complete:
        Display::calibrate(&newCalibrationData);
        wtApp->settings->displayOptions.calibrationData = newCalibrationData;
        wtApp->settings->write();
        ScreenMgr.displayHomeScreen();
        break;
      case post:
        // Assert: should never get here
        break;
    }
  };

  buttons = new Button[(nButtons = 1)];
  buttons[0].init(0, 0, Display::Width, Display::Height, buttonHandler, 0);
  state = pre;
}
