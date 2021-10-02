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
#include <BPABasics.h>
//                                  Local Includes
#include "../gui/Display.h"
#include "../gui/Theme.h"
#include "EnterNumberScreen.h"
//--------------- End:    Includes ---------------------------------------------


// -- Label IDs
static constexpr uint8_t FirstDigitLabel = 0;
static constexpr uint8_t LastDigitLabel = 9;
static constexpr uint8_t DecimalLabel = 10;
static constexpr uint8_t BackspaceLabel = DecimalLabel+1;
static constexpr uint8_t PlusMinusLabel = BackspaceLabel+1;
static constexpr uint8_t ValueLabel = PlusMinusLabel+1;
static constexpr uint8_t N_Labels = ValueLabel+1;

// Fonts
static constexpr auto ValueFont = Display.fonts.FontID::SB12;
static constexpr uint16_t ValueFontHeight = 29;

static constexpr auto DigitFont = Display.fonts.FontID::SB9;
static constexpr uint16_t DigitFontHeight = 22;

// Field Locations
static constexpr auto TitleFont = Display.fonts.FontID::SB12;
static constexpr uint16_t TitleFontHeight = 29;   // TitleFont->yAdvance;
static constexpr uint16_t TitleXOrigin = 0;
static constexpr uint16_t TitleYOrigin = 0;
static constexpr uint16_t TitleHeight = TitleFontHeight;
static constexpr uint16_t TitleWidth = Display.Width;

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
  nLabels = 0; // No labels until init is called!
  labels = new Label[N_Labels];
}

void EnterNumberScreen::init(
    const String& theTitle, float initialValue, Basics::FloatValCB cb, bool decimalsAllowed,
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

  buttonHandler =[this](int id, PressType type) -> void {
    Log.verbose(F("In EnterNumberScreen Label Handler, id = %d, type = %d"), id, type);

    if (type > PressType::Normal) {
      if (id == BackspaceLabel) {
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

    if (id == ValueLabel) {
      if (newValueCB) newValueCB(formattedValue.toFloat());
      return;
    }

    String currentValue = formattedValue;
    if (id >= FirstDigitLabel && id <= LastDigitLabel) {
      if (formattedValue.toFloat() == 0 && formattedValue.indexOf('.') == -1) {
        formattedValue = (char)('0' + (id-FirstDigitLabel));
      }
      else {
        formattedValue += (char)('0' + (id-FirstDigitLabel));
      }
    } else if (id == DecimalLabel) {
      if (!allowDecimals) return;
      if (formattedValue.indexOf('.') == -1) formattedValue += '.';
    } else if (id == BackspaceLabel) {
      int len = formattedValue.length();
      if (len > 0) formattedValue.remove(len-1);
    } else if (id == PlusMinusLabel && formattedValue.length() > 0) {
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
  labels[ValueLabel].init(x, ValueYOrigin, w, DigitHeight, ValueLabel);   

  x += w;
  w = bsW*DigitWidth; 
  labels[BackspaceLabel].init(x, ValueYOrigin, w, DigitHeight, BackspaceLabel);       

  x += w;
  if (allowDecimals) {
    w = DigitWidth; 
    labels[DecimalLabel].init(x, ValueYOrigin, w, DigitHeight, DecimalLabel);      
    x += w;
  } else {
    labels[DecimalLabel].init(0, 0, 0, 0, DecimalLabel);      
  }

  if (minVal < 0) {
    w = DigitWidth; 
    labels[PlusMinusLabel].init(x, ValueYOrigin, w, DigitHeight, PlusMinusLabel);
  } else {
    labels[PlusMinusLabel].init(0, 0, 0, 0, PlusMinusLabel);
  }

  for (int i = FirstDigitLabel; i <= LastDigitLabel; i++) {
    labels[i].init(
      DigitsXOrigin + ((i%5) * DigitWidth), DigitsYOrigin + (i/5) * DigitHeight,
      DigitWidth, DigitHeight, i);        
  }

  nLabels = N_Labels;
}

void EnterNumberScreen::display(bool activating) {
  if (activating) {
    auto& tft = Display.tft;
    tft.fillScreen(Theme::Color_Background);

    Display.fonts.setUsingID(TitleFont, tft);
    tft.setTextColor(Theme::Color_AlertGood);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(title, TitleWidth/2, TitleHeight/2);

    String label(' ');
    // Draw all the static components -- everything except the value
    for (int i = FirstDigitLabel; i <= LastDigitLabel; i++) {
      label.setCharAt(0, (char)('0' + (i-FirstDigitLabel)) );
      labels[i].drawSimple(
          label, DigitFont, DigitFrameSize,
          Theme::Color_NormalText, Theme::Color_Border,
          Theme::Color_Background, false);
    }

    labels[BackspaceLabel].drawSimple(
        "DEL", DigitFont, DigitFrameSize,
        Theme::Color_AlertError, Theme::Color_Border,
        Theme::Color_Background, false);

    if (minVal < 0) {
      labels[PlusMinusLabel].drawSimple(
          "+/-", DigitFont, DigitFrameSize,
          Theme::Color_NormalText, Theme::Color_Border,
          Theme::Color_Background, false);
    }

    if (allowDecimals) {
      labels[DecimalLabel].drawSimple(
          ".", DigitFont, DigitFrameSize,
          Theme::Color_NormalText, Theme::Color_Border,
          Theme::Color_Background, false);
    }
  }

  labels[ValueLabel].drawSimple(
    formattedValue, ValueFont, ValueFrameSize,
    Theme::Color_AlertGood, Theme::Color_Border,
    Theme::Color_Background);
}

void EnterNumberScreen::processPeriodicActivity() {  }








