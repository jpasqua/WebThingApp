/*
 * OLED_Display
 *                    
 */

#include "../DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <BPABasics.h>
#include <SH1106Wire.h>
#include <SSD1306Wire.h>
//                                  WebThing Includes
//                                  Local Includes
#include "../../Display.h"
#include "fonts/OLED_Sans5.h"
#include "fonts/OLED_DSEG7_Classic_Bold_32.h"
//--------------- End:    Includes ---------------------------------------------

inline OLEDDISPLAY_COLOR mono(uint16_t color) {
  return (color ? OLEDDISPLAY_COLOR::WHITE : OLEDDISPLAY_COLOR::BLACK);
}

void OLED_Display::setDeviceOptions(const DisplayDeviceOptions* options) {
  deviceOptions = options;
}

void OLED_Display::device_begin() {
  if (deviceOptions->deviceType == DisplayDeviceOptions::DeviceType::SH1106) {
    oled = new SH1106Wire(deviceOptions->address, deviceOptions->sdaPin, deviceOptions->sclPin);
  }
  else {  // opts->deviceType == DisplayDeviceOptions::DeviceType::SSD1306
    oled = new SSD1306Wire(deviceOptions->address, deviceOptions->sdaPin, deviceOptions->sclPin);
  }
  oled->init();
  setOrientation(_options->invertDisplay );
}

void OLED_Display::setBrightness(uint8_t b) {
  if (b == _brightness) return;
  _brightness = b < 101 ? b : 100;

  uint8_t deviceBrightness = (255 * (uint16_t)b)/100;
  oled->setBrightness(deviceBrightness);
}

void OLED_Display::setOrientation(bool flipped) {
  oled->resetOrientation();
  if (flipped) oled->flipScreenVertically();
}

void OLED_Display::streamScreenShotAsBMP(Stream &s) {
  // Adapted from https://forum.arduino.cc/index.php?topic=406416.0
  byte hiByte, loByte;

  uint8_t bmFlHdr[14] = {
    'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0
  };
  // 54 = std total "old" Windows BMP file header size = 14 + 40
  
  uint8_t bmInHdr[40] = {
    40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 16, 0
  };   
  // 40 = info header size
  //  1 = num of color planes
  // 16 = bits per pixel
  // all other header info = 0, including RI_RGB (no compr), DPI resolution

  uint32_t w = oled->getWidth();
  uint32_t h = oled->getHeight();
  unsigned long bmpSize = 2ul * h * w + 54; // pix data + 54 byte hdr
  
  bmFlHdr[ 2] = (uint8_t)(bmpSize      ); // all ints stored little-endian
  bmFlHdr[ 3] = (uint8_t)(bmpSize >>  8); // i.e., LSB first
  bmFlHdr[ 4] = (uint8_t)(bmpSize >> 16);
  bmFlHdr[ 5] = (uint8_t)(bmpSize >> 24);

  bmInHdr[ 4] = (uint8_t)(w      );
  bmInHdr[ 5] = (uint8_t)(w >>  8);
  bmInHdr[ 6] = (uint8_t)(w >> 16);
  bmInHdr[ 7] = (uint8_t)(w >> 24);
  bmInHdr[ 8] = (uint8_t)(h      );
  bmInHdr[ 9] = (uint8_t)(h >>  8);
  bmInHdr[10] = (uint8_t)(h >> 16);
  bmInHdr[11] = (uint8_t)(h >> 24);

  s.write(bmFlHdr, sizeof(bmFlHdr));
  s.write(bmInHdr, sizeof(bmInHdr));

  for (int16_t y = h-1; y >= 0; y--) {
    byte buf[w*2];
    byte *ptr = &buf[0];
    for (uint16_t x = 0; x < w; x++) {
      uint8_t theByte = oled->buffer[x + (y / 8) * w];
      uint8_t thePixel = theByte & (1 << (y & 7));
      if (thePixel) { loByte = hiByte = 0xff; }
      else {loByte = hiByte = 0x00; }
      
      *ptr++ = loByte;
      *ptr++ = hiByte;
    }
    s.write(buf, w*2);
  }
}

void OLED_Display::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  Display.oled->setColor(mono(color));
  Display.oled->drawRect(x, y, w, h);
  Display.oled->display();
}

void OLED_Display::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  Display.oled->setColor(mono(color));
  Display.oled->fillRect(x, y, w, h);
  Display.oled->display();
}

void OLED_Display::drawStringInRegion(
    const char *text, int8_t fontID, uint8_t alignment,
    uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    uint16_t xOff, uint16_t yOff, 
    uint16_t, uint16_t bg)
{
  OLEDDISPLAY_COLOR bgColor = mono(bg);

  Display.oled->setColor(bgColor);
  Display.oled->fillRect(x, y, w, h);

  switch (alignment) {
    case TL_DATUM:
    case TC_DATUM:
    case TR_DATUM:
    case MC_DATUM:
      break;
    case ML_DATUM:
      alignment = TL_DATUM;
      yOff += h/2;
      break;
    case MR_DATUM:
      alignment = TR_DATUM;
      yOff += h/2;
      break;
    case BL_DATUM:
      alignment = TL_DATUM;
      yOff += h;
      break;
    case BC_DATUM:
      alignment = TC_DATUM;
      yOff += w/2;
      break;
    case BR_DATUM:
      alignment = TR_DATUM;
      yOff += h;
      break;
  }

  switch (alignment) {
    case TL_DATUM:
      Display.oled->setTextAlignment(TEXT_ALIGN_LEFT);
      break;
    case TC_DATUM:
      Display.oled->setTextAlignment(TEXT_ALIGN_CENTER);
      break;
    case TR_DATUM:
      Display.oled->setTextAlignment(TEXT_ALIGN_RIGHT);
      break;
    case MC_DATUM:
      Display.oled->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      break;
  }

  Display.oled->setColor(OLEDDISPLAY_COLOR::INVERSE);
  Display.setFont(fontID);
  Display.oled->drawString(x+xOff, y+yOff, text);
  Display.oled->display();
}

// ----- Font-Related

static constexpr struct  {
    const char *name;
    const uint8_t *font;
  } FontList[] = {
    // ORDER MUST MATCH Display.FontID enum
    {"S5",    SansSerif_plain_5},
    {"S10",   ArialMT_Plain_10},
    {"S16",   ArialMT_Plain_16},
    {"S24",   ArialMT_Plain_24},
    {"D32",   DSEG7_Classic_Bold_32},
  };
static constexpr uint8_t nFronts = ARRAY_SIZE(FontList);

void OLED_Display::setFont(uint8_t fontID)  { oled->setFont(FontList[fontID].font); }

int8_t OLED_Display::fontIDFromName(String fontName) const {
  for (int i = 0; i < nFronts; i++) {
    if (fontName == FontList[i].name) return i;
  }
  return -1;
}

uint8_t OLED_Display::getFontHeight(uint8_t fontID) const {
  return pgm_read_byte(FontList[fontID].font + HEIGHT_POS);
}

// ----- GLOBAL STATE
OLED_Display Display;

#endif
