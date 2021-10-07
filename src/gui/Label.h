/*
 * BaseLabel.h
 *
 */

#ifndef BaseLabel_h
#define BaseLabel_h

#if defined(ESP32)
  #include <functional>
#endif

class Region {
public:
  inline bool contains(uint16_t tx, uint16_t ty) {
    return ((tx >= x) && (tx < x+w) && (ty >= y) && (ty < y+h));
  }

  inline uint16_t bottom() { return (y + h); }
  inline uint16_t left() { return (x + w); }

  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
};

class BaseLabel {
public:
  // ----- Constructors
  BaseLabel() = default;
  ~BaseLabel() = default;

  void init(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t id) {
    region.x = x; region.y = y; region.w = w; region.h = h;
    _id = id;
  }

  void init(const Region& r, uint8_t id) {
    region = r;
    _id = id;
  }

  // ----- Member Functions
  
  virtual void clear(uint16_t color) = 0;

  // Draw a simple button with a border, BaseLabel, and interior background
  // The button can be drawn directly to the display or to a sprite,
  // then copied to the display to avoid flickering. If buffering is on, a 1bpp
  // buffer will be used if possible (when BaseLabelColor == borderColor), otherwise
  // a 4bpp buffer will be used.
  // 
  // @param BaseLabel       The text to be displayed. It is the callers responsibility to ensure
  //                    it will fit within the bounds of the button. It will be drawn middle center
  // @param font        The font that will be used to draw the BaseLabel
  // @param borderSize  The size in pixels of the border
  // @param BaseLabelColor  The color to be used to draw the BaseLabel
  // @param borderColor The color to be used to draw the border
  // @param bgColor     The color to be used to draw the interior of the button
  // @param buffer      Should this be buffered offscreen then copied to the display
  virtual void drawSimple(
      const String& label, uint8_t font, uint8_t borderSize,
      uint16_t labelColor, uint16_t borderColor, uint16_t bgColor,
      bool buffer = false) = 0;

  // Draw a button whose interior represents a progress bar.
  // The button can be drawn directly to the display or to a 4bpp sprite,
  // then copied to the display to avoid flickering. 
  // 
  // @param pct         The progess to be reflected in the bar (0.0-1.0).
  // @param label       The text to be displayed. It is the callers responsibility to ensure
  //                    it will fit within the bounds of the button. It will be drawn middle center.
  //                    If BaseLabel is equal to showPct, then the pct will be drawn rather than the BaseLabel
  // @param font        The font that will be used to draw the BaseLabel
  // @param borderSize  The size in pixels of the border
  // @param labelColor  The color to be used to draw the BaseLabel
  // @param borderColor The color to be used to draw the border
  // @param barColor    The color to be used to draw the progress bar
  // @param bgColor     The color to be used for the unfilled part of the progress bar
  // @param showPct     If the label is textually equal to this parameter, then the percentage
  //                    value will be shown rather than the label text.
  // @param buffer      Should this be buffered offscreen then copied to the display
  virtual void drawProgress(
      float pct, const String& label, uint8_t font, uint8_t borderSize,
      uint16_t labelColor, uint16_t borderColor,
      uint16_t barColor, uint16_t bgColor, const String& showPct,
      bool buffer = false) = 0;


  // ----- Data Members
  Region region;
  uint8_t _id;  
};

#include "devices/DeviceSelect.h"
#include DeviceImplFor(Label)

#endif  // BaseLabel_h