/*
 * Label:
 *    A class that manages a rectangular region that can draw itself in various ways
 *                    
 * NOTES:
 * o A label can be displayed as a simple rectangle with a textual label, an
 *   optional border, and background color.
 * o A label may also be displayed as a progress bar.
 * o Labels may be drawn directly to the screen or they can be buffered using
 *   sprites.
 * o If they are drawn with sprites, the minimum allowable depth will be used
 *   depending on how many distinct colors are being displayed.
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
//                                  Local Includes
#include "Label.h"
#include "Display.h"
#include "Theme.h"
//--------------- End:    Includes ---------------------------------------------


Label::Label(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t id) {
  init(x, y, w, h, id);
}

void Label::init(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t id) {
  region.x = x; region.y = y; region.w = w; region.h = h;
  _id = id;
}

void Label::init(const Region& r, uint8_t id) {
  region = r;
  _id = id;
}

void Label::clear(uint16_t bg) {
  Display.tft.fillRect(region.x, region.y, region.w, region.h, bg);
}

void Label::drawSimple(
      const String& label, uint8_t font, uint8_t borderSize,
      uint16_t labelColor, uint16_t borderColor, uint16_t bgColor,
      bool buffer)
{
  auto& tft = Display.tft;

  if (buffer) {
    auto& sprite = Display.sprite;
    if (labelColor == borderColor) {  // We can do this with 1bpp
      sprite->setColorDepth(1);
      sprite->createSprite(region.w, region.h);
      sprite->fillSprite(Theme::Mono_Background);   // Initialize the sprite

      for (int i = 0; i < borderSize; i++) {      // Draw the frame
        sprite->drawRect(i, i, region.w-(2*i), region.h-(2*i), Theme::Mono_Foreground);
      }
      Display.fonts.setUsingID(font, sprite);
      sprite->setTextColor(Theme::Mono_Foreground);
      sprite->setTextDatum(MC_DATUM);
      sprite->drawString(label, (region.w/2), (region.h/2));
      
      sprite->setBitmapColor(labelColor, bgColor);
      sprite->pushSprite(region.x, region.y);
      sprite->deleteSprite();
    } else {                          // We need to go to 4bpp
      constexpr uint8_t BackgroundIndex = 0;
      constexpr uint8_t LabelIndex = 1;
      constexpr uint8_t BorderIndex = 2;
      uint16_t cmap[16];
      cmap[BackgroundIndex] = bgColor;
      cmap[LabelIndex] = labelColor;
      cmap[BorderIndex] = borderColor;

      sprite->setColorDepth(4);
      sprite->createSprite(region.w, region.h);
      sprite->createPalette(cmap);
      sprite->fillSprite(BackgroundIndex);   // Initialize the sprite

      for (int i = 0; i < borderSize; i++) {      // Draw the frame
        sprite->drawRect(i, i, region.w-(2*i), region.h-(2*i), BorderIndex);
      }
      Display.fonts.setUsingID(font, sprite);
      sprite->setTextColor(LabelIndex);
      sprite->setTextDatum(MC_DATUM);
      sprite->drawString(label, (region.w/2), (region.h/2));
      
      sprite->pushSprite(region.x, region.y);
      sprite->deleteSprite();
    }
  } else {
    tft.fillRect(region.x+borderSize, region.y+borderSize, region.w-borderSize*2, region.h-borderSize*2, bgColor);
    for (int i = 0; i < borderSize; i++) {    // Draw the frame
      tft.drawRect(region.x+i, region.y+i, region.w-(2*i), region.h-(2*i), borderColor);
    }

    Display.fonts.setUsingID(font, tft);
    tft.setTextColor(labelColor);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(label, region.x + (region.w/2), region.y+(region.h/2));
  }
}

void Label::drawProgress(
      float pct, const String& label, uint8_t font, uint8_t borderSize,
      uint16_t labelColor, uint16_t borderColor,
      uint16_t barColor, uint16_t bgColor, const String& showPct,
      bool buffer)
{
  auto& tft = Display.tft;
  String note = (label == showPct) ? String((int)(pct*100)) + "%" : label;

  if (buffer) {
    auto& sprite = Display.sprite;
    constexpr uint8_t BackgroundIndex = 0;
    constexpr uint8_t BarIndex = 1;
    constexpr uint8_t TextIndex = 2;
    constexpr uint8_t BorderIndex = 3;
    uint16_t cmap[16];
    cmap[BackgroundIndex] = bgColor;
    cmap[BarIndex] = barColor;
    cmap[TextIndex] = labelColor;
    cmap[BorderIndex] = borderColor;

    sprite->setColorDepth(4);
    sprite->createSprite(region.w, region.h);
    sprite->createPalette(cmap);
    sprite->fillSprite(BackgroundIndex);

    // Draw the frame
    for (int i = 0; i < borderSize; i++) {
      sprite->drawRect(0+i, 0+i, region.w-(2*i), region.h-(2*i), BorderIndex);
    }

    // Draw the bar
    sprite->fillRect(borderSize, borderSize, pct*(region.w-2*borderSize), (region.h-2*borderSize), BarIndex);

    // Draw the overlay text
    Display.fonts.setUsingID(font, sprite);
    sprite->setTextColor(TextIndex);
    sprite->setTextDatum(MC_DATUM);
    sprite->drawString(note, region.w/2, region.h/2);

    // Push to the display and cleanup
    sprite->pushSprite(region.x, region.y);
    sprite->deleteSprite();
  } else {
    // Draw the frame
    for (int i = 0; i < borderSize; i++) {
      tft.drawRect(region.x+i, region.y+i, region.w-(2*i), region.h-(2*i), borderColor);
    }

    // Draw the bar
    tft.fillRect(region.x+borderSize, region.y+borderSize, pct*(region.w-2*borderSize), (region.h-2*borderSize), barColor);

    // Draw the overlay text
    Display.fonts.setUsingID(font, tft);
    tft.setTextColor(labelColor);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(note, region.x+region.w/2, region.y+region.h/2);
  }
}
