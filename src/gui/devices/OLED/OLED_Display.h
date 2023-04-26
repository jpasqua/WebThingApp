/*
 * Touch_Display:
 *    A device-specific implementation of the Display interface for TFT_eSPI displays
 *                    
 * NOTES:
 * o The Font functions are there to avoid including the font definitions multiple times
 *   and to aid with the implementation of FlexScreen. 
 */

#ifndef OLED_Display_h
#define	OLED_Display_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <OLEDDisplay.h>
//                                  Local Includes
#include "../../DisplayOptions.h"
//--------------- End:    Includes ---------------------------------------------


class DisplayDeviceOptions {
public:
  enum DeviceType {NONE, SH1106, SSD1306};
  DeviceType deviceType;
  uint8_t sclPin;
  uint8_t sdaPin;
  uint8_t address;
};

class OLED_Display : public BaseDisplay {
public:
  // ----- Types
  enum FontID {
    // Monospaced
    M5,

    // Sans-Serif
    S10, SB12, S16, S24,

    // Seven-Segment
    D16, D32,

    // Weather
    W21, W36
  };

  // ----- Constants
  static constexpr uint16_t Width = 128;
  static constexpr uint16_t Height = 64;
  static constexpr uint16_t XCenter = Width/2;
  static constexpr uint16_t YCenter = Height/2;
  static constexpr uint32_t ScreenShotSize = 2ul * Width * Height + 54;
                              // Pixel Buffer size + a 54 byte header

  // ----- Constructors
  OLED_Display() = default;
  ~OLED_Display() = default;


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
  virtual void flush() { oled->display(); }
  virtual uint16_t width() const override;
  virtual uint16_t height() const override;

  // ----- Screenshot functionality
  virtual void streamScreenShotAsBMP(Stream &s) override;

  // ----- Functions that unique to this device
  void setOrientation(bool flipped);

  // ----- Data Members
  OLEDDisplay*  oled;
  const DisplayDeviceOptions* deviceOptions;

protected:
  virtual void device_begin() override;
};

extern OLED_Display Display;

#endif	// OLED_Display_h
