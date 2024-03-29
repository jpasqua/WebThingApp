/*
 * Display:
 *    A set of functions on top of the base display driver which
 *    provide some conveniences (and efficiencies).
 *                    
 */

#ifndef Display_h
#define	Display_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoJson.h>
//                                  Local Includes
#include "DisplayOptions.h"
//--------------- End:    Includes ---------------------------------------------

class DisplayDeviceOptions;

class BaseDisplay {
public:
  // ----- Constants
  // For convenience, these correspond to the "DATUM" definitions
  // in the TFT_eSPI library
  static constexpr uint8_t TL_Align = 0;
  static constexpr uint8_t TC_Align = 1;
  static constexpr uint8_t TR_Align = 2;
  static constexpr uint8_t ML_Align = 3;
  static constexpr uint8_t MC_Align = 4;
  static constexpr uint8_t MR_Align = 5;
  static constexpr uint8_t BL_Align = 6;
  static constexpr uint8_t BC_Align = 7;
  static constexpr uint8_t BR_Align = 8;

  // ----- Constructors
  BaseDisplay() = default;
  ~BaseDisplay() = default;

  // ----- Initialization -----
  // Call this while initializing hardware and before begin()
  virtual void setDeviceOptions(const DisplayDeviceOptions* options) = 0;

  // Must be called before the display is used
  void begin(DisplayOptions* displayOptions) {
    _options = displayOptions;
    device_begin();
    _screenShotSize = 2ul * width() * height() + 54; // 16bpp buffer + 54 byte header
  }

  // ----- Brightness control -----
  virtual void setBrightness(uint8_t b) = 0;  // 0-100
  inline uint8_t getBrightness() const { return _brightness; }

  // ----- Drawing functions -----
  virtual void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) = 0;
  virtual void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) = 0;
  virtual void drawStringInRegion(
    const char *text, int8_t fontID, uint8_t alignment,
    uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    uint16_t xOff, uint16_t yOff, 
    uint16_t fg, uint16_t bg) = 0;
  virtual void setFont(uint8_t fontID) = 0;
  virtual int8_t fontIDFromName(String fontName) const = 0;
  virtual uint8_t getFontHeight(uint8_t fontID) const = 0;
  virtual void flush() { };
  virtual uint16_t width() const = 0;
  virtual uint16_t height() const = 0;
  
  // ----- Screenshot functionality
  virtual void streamScreenShotAsBMP(Stream &s) = 0;
  uint32_t screenShotSize() { return _screenShotSize; };


protected:
  // ----- Implemented by each device type
  virtual void device_begin() = 0;

  uint8_t _brightness = 0;     // 0-100
  DisplayOptions* _options;
  uint32_t _screenShotSize;
};

#include "devices/DeviceSelect.h"
#include DeviceImplFor(Display)

#endif	// Display_h