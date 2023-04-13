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


ScrollScreen::ScrollScreen() {
  nLabels = 0;
  labels = NULL;
}

void ScrollScreen::init(bool autoAdvance, uint32_t delay, uint8_t forceCycles) {
  _delayBetweenFrames = delay;
  _autoAdvance = autoAdvance;
  _mtxWidth = Display.mtx->width();
  _forceCycles = forceCycles;
}

void ScrollScreen::setText(String text, uint8_t fontID) {
  _text = text;
  _fontID = fontID;
  _offset = 0;
  _nextTimeToDisplay = 0;
  int16_t unused;
  _textWidth = Display.getTextWidth(_text, _fontID);
Log.verbose("Calculated text width = %d for %s", _textWidth, _text.c_str());
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
    _offset = 0;
    _nextTimeToDisplay = 0;
  }

  innerDisplay();
  if (!_forceCycles) return;

  _cyclesCompleted = 0;
  while (_cyclesCompleted < _forceCycles) {
    delay(_delayBetweenFrames);
    processPeriodicActivity();
  }
  _offset = _mtxWidth - 1;
  innerDisplay();
}

void ScrollScreen::processPeriodicActivity() {
  if (millis() > _nextTimeToDisplay) {
    innerDisplay();
    _offset++;
    if (_offset == _textWidth + _mtxWidth) {
      if (_forceCycles) {
        _cyclesCompleted++;
        delay(_delayBetweenFrames * 3);
      }
      if (_autoAdvance) {
        ScreenMgr.moveThroughSequence(true);  // Go to the next screen
        return;
      } else {
        _offset = 0;
      }
    }
    // ScrollToBlank + autoAdvance
    _nextTimeToDisplay = millis() + _delayBetweenFrames;
  }
}

#endif