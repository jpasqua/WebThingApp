/*
 * Touch_Display:
 *    A device-specific implementation of the Display interface for Monochrome LED Matrix displays
 *                    
 */

#ifndef MTX_Display_h
#define	MTX_Display_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <BPABasics.h>
//                                  Local Includes
#include "../../DisplayOptions.h"
//--------------- End:    Includes ---------------------------------------------


class DisplayDeviceOptions {
public:
  Basics::Pin csPin;    // Chip Select Pin
  Basics::Pin dinPin;   // DIN (MOSI) Pin
  Basics::Pin clkPin;   // CLK Pin
  uint8_t hDisplays;    // Number of horizontal displays
  uint8_t vDisplays;    // Number of vertical displays
};

class MTX_Display : public BaseDisplay {
public:
  // ----- Types
  enum FontID {
    _5x7, // Built-in fixed pitch font

    // Serif
    S9, SB9,

    // Mono
    M12, MB12
  }; 


  // ----- Constants
  static constexpr uint8_t  BuiltInFont_ID = 0;
  static constexpr uint16_t BuiltInFont_CharWidth = 6;
  static constexpr uint16_t Width = 64;
  static constexpr uint16_t Height = 8;
  static constexpr uint16_t XCenter = Width/2;
  static constexpr uint16_t YCenter = Height/2;
  static constexpr uint32_t ScreenShotSize = 2ul * Width * Height + 54;
                              // Pixel Buffer size + a 54 byte header

  // ----- Constructors
  MTX_Display() = default;
  ~MTX_Display() = default;


  // ----- Initialization -----
  // Call this while initializing hardware and before begin()
  virtual void setDeviceOptions(const DisplayDeviceOptions* options) override;

  // ----- Brightness control -----
  virtual void setBrightness(uint8_t b) override;

  // ----- Drawing functions -----
  virtual void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) override;
  virtual void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) override;
  virtual void drawStringInRegion(
    const char *text, int8_t fontID, uint8_t alignment,
    uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    uint16_t xOff, uint16_t yOff, 
    uint16_t fg, uint16_t bg) override;
  virtual void setFont(uint8_t fontID) override;
  virtual int8_t fontIDFromName(String fontName) const override;
  virtual uint8_t getFontHeight(uint8_t fontID) const override;
  virtual void flush() { mtx->write(); }
  virtual uint16_t width() const override;
  virtual uint16_t height() const override;

  // ----- Screenshot functionality
  virtual void streamScreenShotAsBMP(Stream &s) override;

  // ----- Functions that unique to this device
  void setOrientation(bool flipped);
  uint16_t getTextWidth(String& text, uint8_t fontID);
  uint16_t getTextWidth(const char* text, uint8_t fontID);

  // ----- Data Members
  Max72xxPanel*  mtx;
  const DisplayDeviceOptions* deviceOptions;

protected:
  virtual void device_begin() override;
};

extern MTX_Display Display;

#endif	// MTX_Display_h
