/*
 * MTX_Display
 *                    
 */

#include "../DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <SPI.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <BPABasics.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
//                                  WebThing Includes
//                                  Local Includes
#include "../../Display.h"
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
//--------------- End:    Includes ---------------------------------------------

void MTX_Display::setDeviceOptions(const DisplayDeviceOptions* options) {
  // We setup the device during setDeviceOptions to keep it from potentially
  // displaying garbage while the app and frameworks are initializing
  deviceOptions = options;
  mtx = new Max72xxPanel(
      deviceOptions->csPin, deviceOptions->hDisplays, deviceOptions->vDisplays);
  _regionWidth = mtx->width();
  _regionHeight = mtx->height();
  setBrightness(7); // Don't blast the intensity right out of the gate
  // mtx->fillScreen(0);  // The Max72XXPanel constructor does this
  mtx->write();
}

void MTX_Display::device_begin() {
  // We can't set the orientation until now because _options is read from
  // a settings file and isn't ready at 'setDeviceOptions()' time
  setOrientation(_options->invertDisplay);
}

void MTX_Display::setBrightness(uint8_t b) {
Log.verbose("MTX_Display::setBrightness(%d)", b);
  if (b == _brightness) return;
  if (b == 0) {                   // Turn the display off
    mtx->shutdown(true);          // true -> enter shutdown state
    _brightness = 0;
Log.verbose("MTX_Display::setBrightness: shutting down");
    return;
  } else if (_brightness == 0) {  // Turn the display back on
    mtx->shutdown(false);         // false -> leave shutdown state
Log.verbose("MTX_Display::setBrightness: waking up");
  }

  _brightness = b < 101 ? b : 100;

  uint8_t deviceBrightness = (15 * (uint16_t)b)/100;
Log.verbose("MTX_Display::setBrightness: deviceBrightness = %d, _brightness = %d", deviceBrightness, _brightness);
  mtx->setIntensity(deviceBrightness);
}

void MTX_Display::setOrientation(bool flipped) {
  int nDisplays = deviceOptions->hDisplays * deviceOptions->vDisplays;
  int start = flipped ? (nDisplays - 1) : 0;
  int increment = flipped ? -1 : 1;
  int orientation =  flipped ? 3 : 1;

  for (int y = 0; y < deviceOptions->vDisplays; y++) {
    for (int x = 0; x < deviceOptions->hDisplays; x++) {
        mtx->setPosition(start, x, y);
        mtx->setRotation(start, orientation);
        start += increment;
    }
  }
}

void MTX_Display::streamScreenShotAsBMP(Stream &s) {
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

  uint32_t w = mtx->width();
  uint32_t h = mtx->height();
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
      uint16_t thePixel = mtx->readPixel(x, y);
      if (thePixel) { loByte = hiByte = 0xff; }
      else {loByte = hiByte = 0x00; }
      
      *ptr++ = loByte;
      *ptr++ = hiByte;
    }
    s.write(buf, w*2);
  }
}

void MTX_Display::fillWith(uint16_t color) {
  mtx->fillRect(0, 0, _regionWidth, _regionHeight, color);
}

void MTX_Display::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  mtx->drawRect(x, y, w, h, color);
  mtx->write();
}

void MTX_Display::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  mtx->fillRect(x, y, w, h, color);
  mtx->write();
}

void MTX_Display::drawStringInRegion(
    const char *text, int8_t fontID, uint8_t alignment,
    uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    uint16_t xOff, uint16_t yOff, 
    uint16_t, uint16_t bg)
{
  int16_t bb_x, bb_y;
  uint16_t bb_w, bb_h;
  mtx->getTextBounds(text, 0, 0, &bb_x, &bb_y, &bb_w, &bb_h);

  // Adjust X coordinate
  switch (alignment) {
    case TL_Align:
    case ML_Align:
    case BL_Align:
      // Nothing to do...
      break;
    case TC_Align:
    case MC_Align:
    case BC_Align:
      xOff -= bb_w/2;
      break;
    case TR_Align:
    case MR_Align:
    case BR_Align:
      xOff -= bb_w;
      break;
  }

  // Adjust Y coordinate
  switch (alignment) {
    case TL_Align:
    case TC_Align:
    case TR_Align:
      // Nothing to do...
      break;
    case MC_Align:
    case ML_Align:
    case MR_Align:
      yOff -= bb_h/2;
      break;
    case BL_Align:
    case BC_Align:
    case BR_Align:
      yOff -= bb_h;
      break;
  }

  mtx->fillRect(x, y, w, h, bg);
  if (fontID == BuiltInFont_ID) yOff -= getFontHeight(fontID);  // Adjust baseline
  setFont(fontID);
  mtx->setCursor(x+xOff, y+yOff);
  mtx->print(text);
  mtx->write();
}

// ----- Font-Related
static constexpr struct  {
    const char* name;
    const GFXfont* font;
  } FontList[] = {
    // ORDER MUST MATCH Display.FontID enum
    {"_5x7",  nullptr},
    {"S9",    &FreeSerif9pt7b},
    {"SB9",   &FreeSerifBold9pt7b},
    {"M12",   &FreeMono12pt7b},
    {"MB12",  &FreeMonoBold12pt7b}
  };

static constexpr uint8_t nFonts = countof(FontList);

void MTX_Display::setFont(uint8_t fontID)  {
  mtx->setFont(FontList[fontID].font);
}

int8_t MTX_Display::fontIDFromName(String fontName) const {
  for (int i = 0; i < nFonts; i++) {
    if (fontName == FontList[i].name) return i;
  }
  return -1;
}

uint8_t MTX_Display::getFontHeight(uint8_t fontID) const {
  if (fontID == BuiltInFont_ID) { return 8; }
  return FontList[fontID].font->yAdvance;
}

uint16_t MTX_Display::getTextWidth(String& text, uint8_t fontID) {
  return getTextWidth(text.c_str(), fontID);
}

void MTX_Display::cleanText(String& text) {
  bool clean = true;
  for (byte c : text) {
    if (c > 128) { clean = false; break; }
  }
  if (clean) { return; }
  text.replace("’", "'");
  text.replace("“", "\"");
  text.replace("”", "\"");
  text.replace("`", "'");
  text.replace("‘", "'");
  text.replace("„", "'");
  text.replace("\\\"", "'");
  text.replace("•", "-");
  text.replace("é", "e");
  text.replace("è", "e");
  text.replace("ë", "e");
  text.replace("ê", "e");
  text.replace("à", "a");
  text.replace("â", "a");
  text.replace("ù", "u");
  text.replace("ç", "c");
  text.replace("î", "i");
  text.replace("ï", "i");
  text.replace("ô", "o");
  text.replace("…", "...");
  text.replace("–", "-");
  text.replace("Â", "A");
  text.replace("À", "A");
  text.replace("æ", "ae");
  text.replace("Æ", "AE");
  text.replace("É", "E");
  text.replace("È", "E");
  text.replace("Ë", "E");
  text.replace("Ô", "O");
  text.replace("Ö", "Oe");
  text.replace("ö", "oe");
  text.replace("œ", "oe");
  text.replace("Œ", "OE");
  text.replace("Ù", "U");
  text.replace("Û", "U");
  text.replace("Ü", "Ue");
  text.replace("ü", "ue");
  text.replace("Ä", "Ae");
  text.replace("ä", "ae");
  text.replace("ß", "ss");
  text.replace("»", "'");
  text.replace("«", "'");
}

#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))

uint16_t MTX_Display::getTextWidth(const char* text, uint8_t fontID) {
  if (fontID == BuiltInFont_ID) {
    return strlen(text) * BuiltInFont_CharWidth;
  }

  const GFXfont* gfxFont = FontList[fontID].font;
  uint8_t first = pgm_read_byte(&gfxFont->first),
          last  = pgm_read_byte(&gfxFont->last);

  uint16_t width = 0;
  char c;
  for (const char* cur = text; (c = *cur) != '\0'; cur++) {
    const char c = *cur;
    if (c == '\n' || c == '\r') continue;

    if ((c >= first) && (c <= last)) { // Char present in this font
      GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c - first]);
      width += pgm_read_byte(&glyph->width);
    }
  }
  return width;
}

uint16_t MTX_Display::width() const  { return _regionWidth; }
uint16_t MTX_Display::height() const { return _regionHeight; }

void MTX_Display::setRegion(Region& region) {
Log.verbose("Setting region");
  mtx->clip(region.xMin, region.yMin, region.xMax, region.yMax);
  mtx->translate(region.xMin, region.yMin);
  _regionWidth = region.xMax - region.xMin + 1;
  _regionHeight = region.yMax - region.yMin + 1;
}

void MTX_Display::resetRegion() {
  mtx->resetClip();
  mtx->resetTranslation();
  _regionWidth = mtx->width();
  _regionHeight = mtx->height();
}
// ----- GLOBAL STATE
MTX_Display Display;

#endif
