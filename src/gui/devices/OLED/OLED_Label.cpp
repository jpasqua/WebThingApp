#include "../DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
//                                  Local Includes
#include "../../Label.h"
#include "../../Display.h"
#include "../../Theme.h"
//--------------- End:    Includes ---------------------------------------------

Label::Label(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t id) {
  BaseLabel::init(x, y, w, h, id);
}

void Label::clear(uint16_t color) {
  Display.oled->setColor(color ? OLEDDISPLAY_COLOR::WHITE : OLEDDISPLAY_COLOR::BLACK);
  Display.oled->fillRect(region.x, region.y, region.w, region.h);
}

void Label::drawSimple(
  const String& label, uint8_t font, uint8_t borderSize,
  uint16_t, uint16_t, uint16_t bgColor,
  bool)
{
  auto oled = Display.oled;
  OLEDDISPLAY_COLOR background = bgColor ?    OLEDDISPLAY_COLOR::WHITE :
                                              OLEDDISPLAY_COLOR::BLACK;

  OLEDDISPLAY_COLOR foreground = background ? OLEDDISPLAY_COLOR::BLACK :
                                              OLEDDISPLAY_COLOR::WHITE;


  // Clear the background
  oled->setColor(background);
  oled->fillRect(
    region.x+borderSize, region.y+borderSize,
    region.w-borderSize*2, region.h-borderSize*2);

  // Draw the frame
  oled->setColor(foreground);
  for (int i = 0; i < borderSize; i++) {
    oled->drawRect(region.x+i, region.y+i, region.w-(2*i), region.h-(2*i));
  }

  // Draw the label
  oled->setColor(foreground);
  Display.setFont(font);
  oled->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  oled->drawString(region.x + (region.w/2), region.y+(region.h/2), label);
}

void Label::drawProgress(
  float pct, const String& label, uint8_t font, uint8_t borderSize,
  uint16_t, uint16_t borderColor,
  uint16_t, uint16_t, bool showPct,
  bool)
{
  auto oled = Display.oled;
  OLEDDISPLAY_COLOR foreground = borderColor ? OLEDDISPLAY_COLOR::WHITE : OLEDDISPLAY_COLOR::BLACK;
  OLEDDISPLAY_COLOR background = foreground ? OLEDDISPLAY_COLOR::BLACK : OLEDDISPLAY_COLOR::WHITE;
  OLEDDISPLAY_COLOR textColor = OLEDDISPLAY_COLOR::INVERSE;

  String note = showPct ? String((int)(pct*100)) + "%" : label;

  // Clear the background
  oled->setColor(background);
  oled->fillRect(
    region.x+borderSize, region.y+borderSize,
    region.w-borderSize*2, region.h-borderSize*2);

  // Draw the frame
  oled->setColor(foreground);
  for (int i = 0; i < borderSize; i++) {
    oled->drawRect(region.x+i, region.y+i, region.w-(2*i), region.h-(2*i));
  }

  // Draw the bar
  oled->setColor(foreground);
  oled->fillRect(
    region.x+borderSize, region.y+borderSize,
    pct*(region.w-2*borderSize), (region.h-2*borderSize));

  // Draw the overlay text
  oled->setColor(textColor);
  Display.setFont(font);
  oled->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  oled->drawString(region.x + (region.w/2), region.y+(region.h/2), note);
}

#endif
