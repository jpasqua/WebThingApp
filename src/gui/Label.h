/*
 * Label.h:
 *    A touch-based button class.
 *
 *
 */

#ifndef Button_h
#define Button_h

#if defined(ESP32)
  #include <functional>
#endif
#include "BaseButton.h"

class Label : public BaseButton {
public:
  // ----- Types
  typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
  } Region;

  // ----- Constructors
  Label() = default;
  Label(uint16_t x, uint16_t y, uint16_t w, uint16_t h, ButtonCallback callback, uint8_t id);

  // ----- Member Functions
  void init(Region& r, ButtonCallback callback, uint8_t id);
  void init(uint16_t x, uint16_t y, uint16_t w, uint16_t h, ButtonCallback callback, uint8_t id);

  bool processTouch(uint16_t tx, uint16_t ty, PressType type);

  void clear(uint16_t bg);

  // Draw a simple button with a border, label, and interior background
  // The button can be drawn directly to the display or to a sprite,
  // then copied to the display to avoid flickering. If buffering is on, a 1bpp
  // buffer will be used if possible (when labelColor == borderColor), otherwise
  // a 4bpp buffer will be used.
  // 
  // @param label       The text to be displayed. It is the callers responsibility to ensure
  //                    it will fit within the bounds of the button. It will be drawn middle center
  // @param font        The font that will be used to draw the label
  // @param borderSize  The size in pixels of the border
  // @param labelColor  The color to be used to draw the label
  // @param borderColor The color to be used to draw the border
  // @param bgColor     The color to be used to draw the interior of the button
  // @param buffer      Should this be buffered offscreen then copied to the display
  void drawSimple(
      const String& label, uint8_t font, uint8_t borderSize,
      uint16_t labelColor, uint16_t borderColor, uint16_t bgColor,
      bool buffer = false);

  // Draw a button whose interior represents a progress bar.
  // The button can be drawn directly to the display or to a 4bpp sprite,
  // then copied to the display to avoid flickering. 
  // 
  // @param pct         The progess to be reflected in the bar (0.0-1.0).
  // @param label       The text to be displayed. It is the callers responsibility to ensure
  //                    it will fit within the bounds of the button. It will be drawn middle center.
  //                    If label is equal to showPct, then the pct will be drawn rather than the label
  // @param font        The font that will be used to draw the label
  // @param borderSize  The size in pixels of the border
  // @param labelColor  The color to be used to draw the label
  // @param borderColor The color to be used to draw the border
  // @param barColor    The color to be used to draw the progress bar
  // @param bgColor     The color to be used for the unfilled part of the progress bar
  // @param buffer      Should this be buffered offscreen then copied to the display
  void drawProgress(
        float pct, const String& label, uint8_t font, uint8_t borderSize,
        uint16_t labelColor, uint16_t borderColor,
        uint16_t barColor, uint16_t bgColor, const String& showPct,
        bool buffer = false);


  // ----- Data Members
  Region region;

private:
  ButtonCallback _callback;
  uint8_t _id;  
};

#endif  // Button_h