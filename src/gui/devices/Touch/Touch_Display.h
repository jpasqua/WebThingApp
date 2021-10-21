/*
 * Touch_Display:
 *    A device-specific implementation of the Display interface for TFT_eSPI displays
 *                    
 * NOTES:
 * o The Font functions are provided to work around an issue in TFT_eSPI.
 *   + It assumes that it will be used an application that only includes the header once. 
 *   + This is true for a "normal" Arduino application with all files in the top
 *     level sketch folder. These are effectively concatenated at build time into
 *     a single compilation unit.
 *   + With more sophisticated file structures, the font data gets included multiple
 *     times cause the ap size to balloon. 
 */

#ifndef Touch_Display_h
#define	Touch_Display_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <TFT_eSPI.h>
//                                  Local Includes
#include "../../DisplayOptions.h"
//--------------- End:    Includes ---------------------------------------------


class Touch_Display : public BaseDisplay {
public:
  // ----- Types
  enum FontID {
    M9,  MB9,  MO9,  MBO9,
    S9,  SB9,  SO9,  SBO9,
    S12, SB12, SO12, SBO12,
    S18, SB18, SO18, SBO18,
    S24, SB24, SO24, SBO24,
    D20, D72,  D100
  };

  // ----- Constants
  static constexpr uint16_t Width = 320;
  static constexpr uint16_t Height = 240;
  static constexpr uint16_t XCenter = Width/2;
  static constexpr uint16_t YCenter = Height/2;
  static constexpr uint32_t ScreenShotSize = 2ul * Width * Height + 54;
                              // Pixel Buffer size + a 54 byte header

  // ----- Constructors
  Touch_Display() = default;
  ~Touch_Display() = default;

  // ----- Initialization -----
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
  virtual void flush() { };

  // ----- Screenshot functionality
  virtual void streamScreenShotAsBMP(Stream &s) override;

  // ----- Functions that unique to this device
  void calibrate(CalibrationData* newCalibrationData);
  void setSpriteFont(uint8_t fontID) const;

  // ----- Data Members
  TFT_eSPI tft;
  TFT_eSprite *sprite;
  const DisplayDeviceOptions* deviceOptions;


protected:
  virtual void device_begin() override;
};

extern Touch_Display Display;

#endif	// Touch_Display_h
