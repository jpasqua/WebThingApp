/*
 * ScrollScreen:
 *    Base class for scrolling text displays.
 *      
 * NOTES:   
 * o There are delays in the code below which would normally be a "non-no",
 *   however, they are only used when "forceCycles" is in play. This is
 *   known to be a blocking operation.
 *           
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../gui/Display.h"
#include "../../gui/ScreenMgr.h"
#include "../../gui/Theme.h"
#include "ScrollScreen.h"
//--------------- End:    Includes ---------------------------------------------


uint32_t ScrollScreen::defaultDelayBetweenFrames = 50;  // static (class-level) variable
void ScrollScreen::setDefaultFrameDelay(uint32_t delay) {
  defaultDelayBetweenFrames = delay;
}

ScrollScreen::ScrollScreen() {
  nLabels = 0;
  labels = NULL;
}

void ScrollScreen::init(bool autoAdvance, uint32_t delay, uint8_t forceCycles) {
  _delayBetweenFrames = delay;
  _autoAdvance = autoAdvance;
  _mtxWidth = Display.mtx->width();
  _forceCycles = forceCycles;
  _goHome = false;
}

void ScrollScreen::setText(String text, uint8_t fontID) {
  _text = text;
  Display.cleanText(_text); // Make sure all the characters are suitable for the LEDMatrix
  _fontID = fontID;
  _offset = 0;
  _nextTimeToDisplay = 0;
  _textWidth = Display.getTextWidth(_text, _fontID);
  uint16_t verticalPadding = (Display.mtx->height() - Display.getFontHeight(_fontID))/2;
  if (_fontID == 0) { // Built-in font
    _baseline = verticalPadding;
  } else {
    _baseline = Display.mtx->height() - verticalPadding;
  }
}

void ScrollScreen::innerDisplay() {
  auto& mtx = Display.mtx;

  mtx->fillScreen(Theme::Color_BLACK);
  mtx->setCursor((_mtxWidth - 1) - _offset, _baseline);
  mtx->print(_text);
  mtx->write();
}

void ScrollScreen::display(bool activating) {
  if (activating) {
    innerActivation();
    if (_text.isEmpty() && _autoAdvance) {
      ScreenMgr.moveThroughSequence(true);  // Go to the next screen
      return;
    }
    _offset = 0;
    _nextTimeToDisplay = 0;
    Display.setFont(_fontID);
  }

  innerDisplay();
  if (!_forceCycles) return;

  _cyclesCompleted = 0;
  while (_cyclesCompleted < _forceCycles) {
    uint32_t dbf = _delayBetweenFrames ? _delayBetweenFrames : defaultDelayBetweenFrames;
    delay(dbf);
    processPeriodicActivity();
  }
  _offset = _mtxWidth - 1;
  innerDisplay();
}

void ScrollScreen::processPeriodicActivity() {
  if (innerPeriodic()) return;
  uint32_t dbf = _delayBetweenFrames ? _delayBetweenFrames : defaultDelayBetweenFrames;
  if (millis() > _nextTimeToDisplay) {
    innerDisplay();
    _offset++;
    if (_offset == _textWidth + _mtxWidth) {
      if (_forceCycles) {
        _cyclesCompleted++;
        delay(dbf * 3);
      }
      if (_autoAdvance) {
        // Go to the next screen
        if (_goHome) { ScreenMgr.displayHomeScreen(); }
        else { ScreenMgr.moveThroughSequence(true); } 
        return;
      } else {
        _offset = 0;
      }
    }
    // ScrollToBlank + autoAdvance
    _nextTimeToDisplay = millis() + dbf;
  }
}

#endif