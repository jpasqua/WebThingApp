#include "../DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../FlexScreen.h"
#include "../../ScreenMgr.h"
//--------------- End:    Includes ---------------------------------------------


void FlexScreen::prepForDisplay() {
  _delayBetweenFrames = std::max(_items[0]._x, MinDelayBetweenFrames);
    // We overload the meaning of "_x" when used as part of a MTX_FlexScreen
  _fontID = _items[0]._gfxFont;
  _offset = 0;
  _nextTimeToDisplay = 0;

  // Build up the text
  char buf[Display.mtx->width()/6 + 1];  // Assume 6 pixel spacing is smallest font

  _text.clear();
  for (int i = 0; i < _nItems; i++) {
    _items[i].generateText(buf, _mapper);
    _text += buf;
  }
  _textWidth = Display.getTextWidth(_text, _fontID);

  uint16_t verticalPadding = (Display.mtx->height() - Display.getFontHeight(_fontID))/2;
  if (_fontID == 0) { // Built-in font
    _baseline = verticalPadding;
  } else {
    _baseline = Display.mtx->height() - verticalPadding;
  }

  Display.setFont(_fontID);
}

void FlexScreen::innerDisplay() {
  auto& mtx = Display.mtx;
  mtx->fillScreen(0);
  mtx->setCursor((_mtxWidth - 1) - _offset, _baseline);
  mtx->print(_text);
  mtx->write();
}

void FlexScreen::display(bool activating) {
  if (activating) { prepForDisplay(); }
  innerDisplay();
}

void FlexScreen::processPeriodicActivity() {
  if (millis() > _nextTimeToDisplay) {
    innerDisplay();
    _nextTimeToDisplay = millis() + _delayBetweenFrames;
    _offset++;
    if (_offset == _textWidth + _mtxWidth) {
      ScreenMgr.moveThroughSequence(true);  // Go to the next screen
    }
  }
}

#endif // DEVICE_TYPE == DEVICE_TYPE_MTX