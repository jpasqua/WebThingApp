/*
 * EnterNumberScreen:
 *    Allow the user to enter a numeric value 
 *                    
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <float.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <WTBasics.h>
//                                  Local Includes
#include "../gui/Display.h"
#include "../gui/Theme.h"
#include "EnterNumberScreen.h"
//--------------- End:    Includes ---------------------------------------------

using Display::tft;


// -- Button IDs
static constexpr uint8_t FirstDigitButton = 0;
static constexpr uint8_t LastDigitButton = 9;
static constexpr uint8_t DecimalButton = 10;
static constexpr uint8_t BackspaceButton = DecimalButton+1;
static constexpr uint8_t PlusMinusButton = BackspaceButton+1;
static constexpr uint8_t ValueButton = PlusMinusButton+1;
static constexpr uint8_t TotalButtons = ValueButton+1;

// Fonts
static constexpr auto ValueFont = Display::Font::FontID::SB12;
static constexpr uint16_t ValueFontHeight = 29;

static constexpr auto DigitFont = Display::Font::FontID::SB9;
static constexpr uint16_t DigitFontHeight = 22;

// Field Locations
static constexpr auto TitleFont = Display::Font::FontID::SB12;
static constexpr uint16_t TitleFontHeight = 29;   // TitleFont->yAdvance;
static constexpr uint16_t TitleXOrigin = 0;
static constexpr uint16_t TitleYOrigin = 0;
static constexpr uint16_t TitleHeight = TitleFontHeight;
static constexpr uint16_t TitleWidth = Display::Width;

static constexpr uint16_t DigitWidth = 60;
static constexpr uint16_t DigitHeight = 60;

static constexpr uint16_t ValueXOrigin = 5;
static constexpr uint16_t ValueYOrigin = TitleHeight + 5;
static constexpr uint16_t ValueWidth = 100;
static constexpr uint16_t ValueHeight = 60;

static constexpr uint16_t DigitsXOrigin = ValueXOrigin;
static constexpr uint16_t DigitsYOrigin = ValueYOrigin + ValueHeight + 5;

static constexpr uint8_t ValueFrameSize = 2;
static constexpr uint8_t DigitFrameSize = 1;

EnterNumberScreen::EnterNumberScreen() {
  nButtons = 0; // No buttons until init is called!
  buttons = new Button[TotalButtons];
}

void EnterNumberScreen::init(
    String theTitle, float initialValue, WTBasics::FloatValCB cb, bool decimalsAllowed,
    float minAllowed, float maxAllowed) {
  title = theTitle;
  minVal = minAllowed;
  maxVal = maxAllowed;
  allowDecimals = decimalsAllowed;
  newValueCB = cb;
  _initialValue = constrain(initialValue, minVal, maxVal);

  long valAsLong = _initialValue;
  if (allowDecimals && (_initialValue - valAsLong != 0)) {
    formattedValue = String(_initialValue);
  } else {
    formattedValue = String(valAsLong);
  }

  auto buttonHandler =[this](int id, Button::PressType type) -> void {
    Log.verbose(F("In EnterNumberScreen Button Handler, id = %d, type = %d"), id, type);

    if (type > Button::PressType::NormalPress) {
      if (id == BackspaceButton) {
        // Clear the whole value
        if (minVal > 0) formattedValue = String(minVal);
        else formattedValue = "";
        display();
      } else {
        // Any other long press is interpreted as a cancellation, which is handled the same as
        // no change to the initial value
        if (newValueCB) newValueCB(_initialValue);
      }
      return;
    }

    if (id == ValueButton) {
      if (newValueCB) newValueCB(formattedValue.toFloat());
      return;
    }

    String currentValue = formattedValue;
    if (id >= FirstDigitButton && id <= LastDigitButton) {
      if (formattedValue.toFloat() == 0 && formattedValue.indexOf('.') == -1) {
        formattedValue = (char)('0' + (id-FirstDigitButton));
      }
      else {
        formattedValue += (char)('0' + (id-FirstDigitButton));
      }
    } else if (id == DecimalButton) {
      if (!allowDecimals) return;
      if (formattedValue.indexOf('.') == -1) formattedValue += '.';
    } else if (id == BackspaceButton) {
      int len = formattedValue.length();
      if (len > 0) formattedValue.remove(len-1);
    } else if (id == PlusMinusButton && formattedValue.length() > 0) {
      if (formattedValue[0] == '-') formattedValue.remove(0, 1);
      else formattedValue = "-" + formattedValue;
    }
    float newVal = formattedValue.toFloat();
    if (newVal < minVal || newVal > maxVal) {
      formattedValue = currentValue;
      Log.verbose("Value out of range: %s", formattedValue.c_str());
      // TO DO: Blink the screen or something
    } else {
      display();
    }
  };

  uint16_t vW = 4;    // Assume the value gets 4 slots
  uint16_t bsW = 1;   // ... and the backspace button gets 1

  if (minVal < 0) vW--;     // Need space for the "+/-" button
  if (allowDecimals) vW--;  // Need space for the "." button
  if (vW == 4) { vW--; bsW++; } // Balance out the space a little better

  uint16_t x = ValueXOrigin;
  uint16_t w = vW*DigitWidth;
  buttons[ValueButton].init(
      x, ValueYOrigin, w, DigitHeight, buttonHandler, ValueButton);   

  x += w;
  w = bsW*DigitWidth; 
  buttons[BackspaceButton].init(
      x, ValueYOrigin, w, DigitHeight, buttonHandler, BackspaceButton);       

  x += w;
  if (allowDecimals) {
    w = DigitWidth; 
    buttons[DecimalButton].init(
        x, ValueYOrigin, w, DigitHeight, buttonHandler, DecimalButton);      
    x += w;
  } else {
    buttons[DecimalButton].init(0, 0, 0, 0, buttonHandler, DecimalButton);      
  }

  if (minVal < 0) {
    w = DigitWidth; 
    buttons[PlusMinusButton].init(
        x, ValueYOrigin, w, DigitHeight, buttonHandler, PlusMinusButton);
  } else {
    buttons[PlusMinusButton].init(0, 0, 0, 0, buttonHandler, PlusMinusButton);
  }

  for (int i = FirstDigitButton; i <= LastDigitButton; i++) {
    buttons[i].init(
      DigitsXOrigin + ((i%5) * DigitWidth), DigitsYOrigin + (i/5) * DigitHeight,
      DigitWidth, DigitHeight, buttonHandler, i);        
  }

  nButtons = TotalButtons;
}

void EnterNumberScreen::display(bool activating) {
  if (activating) {
    tft.fillScreen(Theme::Color_Background);

    Display::Font::setUsingID(TitleFont, tft);
    tft.setTextColor(Theme::Color_AlertGood);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(title, TitleWidth/2, TitleHeight/2);

    String label(' ');
    // Draw all the static components -- everything except the value
    for (int i = FirstDigitButton; i <= LastDigitButton; i++) {
      label.setCharAt(0, (char)('0' + (i-FirstDigitButton)) );
      buttons[i].drawSimple(
          label, DigitFont, DigitFrameSize,
          Theme::Color_NormalText, Theme::Color_Border,
          Theme::Color_Background, false);
    }

    buttons[BackspaceButton].drawSimple(
        "DEL", DigitFont, DigitFrameSize,
        Theme::Color_AlertError, Theme::Color_Border,
        Theme::Color_Background, false);

    if (minVal < 0) {
      buttons[PlusMinusButton].drawSimple(
          "+/-", DigitFont, DigitFrameSize,
          Theme::Color_NormalText, Theme::Color_Border,
          Theme::Color_Background, false);
    }

    if (allowDecimals) {
      buttons[DecimalButton].drawSimple(
          ".", DigitFont, DigitFrameSize,
          Theme::Color_NormalText, Theme::Color_Border,
          Theme::Color_Background, false);
    }
  }

  buttons[ValueButton].drawSimple(
    formattedValue, ValueFont, ValueFrameSize,
    Theme::Color_AlertGood, Theme::Color_Border,
    Theme::Color_Background);
}

void EnterNumberScreen::processPeriodicActivity() {  }








