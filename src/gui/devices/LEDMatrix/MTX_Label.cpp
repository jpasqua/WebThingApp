#include "../DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

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
  Display.mtx->fillRect(region.x, region.y, region.w, region.h, color);
}

void Label::drawSimple(
  const String& label, uint8_t font, uint8_t borderSize,
  uint16_t labelColor, uint16_t borderColor, uint16_t bgColor,
  bool)
{
  auto& mtx = Display.mtx;

  Display.setFont(font);
  // Clear the background
  mtx->fillRect(
    region.x+borderSize, region.y+borderSize,
    region.w-borderSize*2, region.h-borderSize*2, bgColor);

  // Draw the border
  for (int i = 0; i < borderSize; i++) {
    mtx->drawRect(region.x+i, region.y+i, region.w-(2*i), region.h-(2*i), borderColor);
  }

  // Draw the label
  Display.drawStringInRegion(
    label.c_str(), 0, Display.MC_Align,
    region.x, region.y, region.w, region.h, (region.w/2), (region.h/2),
    labelColor, bgColor);
}

void Label::drawProgress(
  float pct, const String& label, uint8_t font, uint8_t borderSize,
  uint16_t labelColor, uint16_t borderColor,
  uint16_t barColor, uint16_t bgColor, bool showPct,
  bool)
{
  auto mtx = Display.mtx;
  String note = showPct ? String((int)(pct*100)) + "%" : label;

  // Clear the background
  mtx->fillRect(
    region.x+borderSize, region.y+borderSize,
    region.w-borderSize*2, region.h-borderSize*2, bgColor);

  // Draw the border
  for (int i = 0; i < borderSize; i++) {
    mtx->drawRect(region.x+i, region.y+i, region.w-(2*i), region.h-(2*i), borderColor);
  }

  // Draw the bar
  mtx->fillRect(
    region.x+borderSize, region.y+borderSize,
    pct*(region.w-2*borderSize), (region.h-2*borderSize), barColor);

  // Draw the overlay text
  Display.drawStringInRegion(
    note.c_str(), font, Display.MC_Align,
    region.x, region.y, region.w, region.h, (region.w/2), (region.h/2),
    labelColor, bgColor);
}

#endif
