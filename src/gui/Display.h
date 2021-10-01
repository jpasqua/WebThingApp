/*
 * Display:
 *    A set of functions on top of the base display driver (TFT_eSPI) which
 *    provide some conveniences (and efficiencies).
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

#ifndef Display_h
#define	Display_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
//                                  Local Includes
#include "DisplayOptions.h"
//--------------- End:    Includes ---------------------------------------------


class DisplayObject {
public:
  // ----- Types
  class FontMgr {
  public:
    enum FontID {
      M9,  MB9,  MO9,  MBO9,
      S9,  SB9,  SO9,  SBO9,
      S12, SB12, SO12, SBO12,
      S18, SB18, SO18, SBO18,
      S24, SB24, SO24, SBO24,
      D20, D72,  D100
    };

    // ----- Functions
    void setUsingID(uint8_t fontID, TFT_eSPI& t) const;
    void setUsingID(uint8_t fontID, TFT_eSprite *s) const;
    int8_t idFromName(String fontName) const;
    uint8_t getHeight(uint8_t fontID) const;
  };

  // ----- Constants
  static constexpr uint16_t Width = 320;
  static constexpr uint16_t Height = 240;
  static constexpr uint16_t XCenter = Width/2;
  static constexpr uint16_t YCenter = Height/2;

  // ----- Constructors

  // ----- Member Functions
  void begin(DisplayOptions* displayOptions);
  void calibrate(CalibrationData* newCalibrationData);

  void setBrightness(uint8_t b);  // 0-100
  inline uint8_t getBrightness() const { return _brightness; }

  inline uint32_t getSizeOfScreenShotAsBMP() const {
    // Pixel Buffer size + a 54 byte header...
    return (2ul * Width * Height + 54);
  }
  void streamScreenShotAsBMP(Stream &s);

  // ----- Data Members
  FontMgr fonts;
  TFT_eSPI tft;
  TFT_eSprite *sprite;

private:
    uint8_t _brightness = 0;     // 0-100
    DisplayOptions* _options;
};

extern DisplayObject Display;

#endif	// Display_h