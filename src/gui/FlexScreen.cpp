/*
 * FlexScreen:
 *    Display values driven by a screen layout definition 
 *                    
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
#include <FS.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <BPABasics.h>
#include <DataBroker.h>
//                                  Local Includes
#include "Display.h"
#include "Theme.h"
#include "FlexScreen.h"
//--------------- End:    Includes ---------------------------------------------

using Display::tft;
using Display::sprite;


/*------------------------------------------------------------------------------
 *
 * Local Utility Functions
 *
 *----------------------------------------------------------------------------*/

inline uint16_t mapColor(String colorSpecifier) {
  uint32_t hexVal = strtol(colorSpecifier.c_str(), NULL, 16);
  return tft.color24to16(hexVal);
}

FlexItem::Type mapType(String t) {
  if (t.equalsIgnoreCase(F("INT"))) return FlexItem::Type::INT;
  if (t.equalsIgnoreCase(F("FLOAT"))) return FlexItem::Type::FLOAT;
  if (t.equalsIgnoreCase(F("STRING"))) return FlexItem::Type::STRING;
  if (t.equalsIgnoreCase(F("BOOL"))) return FlexItem::Type::BOOL;
  if (t.equalsIgnoreCase(F("CLOCK"))) return FlexItem::Type::CLOCK;
  if (t.equalsIgnoreCase(F("STATUS"))) return FlexItem::Type::STATUS;
  return FlexItem::Type::STRING;
}

uint8_t mapDatum(String justify) {
  if (justify.equalsIgnoreCase(F("TL"))) { return TL_DATUM;}
  if (justify.equalsIgnoreCase(F("TC"))) { return TC_DATUM;}
  if (justify.equalsIgnoreCase(F("TR"))) { return TR_DATUM;}
  if (justify.equalsIgnoreCase(F("ML"))) { return ML_DATUM;}
  if (justify.equalsIgnoreCase(F("MC"))) { return MC_DATUM;}
  if (justify.equalsIgnoreCase(F("MR"))) { return MR_DATUM;}
  if (justify.equalsIgnoreCase(F("BL"))) { return BL_DATUM;}
  if (justify.equalsIgnoreCase(F("BC"))) { return BC_DATUM;}
  if (justify.equalsIgnoreCase(F("BR"))) { return BR_DATUM;}

  return TL_DATUM;
}

void mapFont(String fontName, int8_t& gfxFont, uint8_t& font) {
  // Use a default if no matching font is found
  font = 2;
  gfxFont = -1;

  if (fontName.length() == 1 && isDigit(fontName[0])) {
    font = fontName[0] - '0';
    return;
  } 

  gfxFont = Display::Font::idFromName(fontName);
} 

/*------------------------------------------------------------------------------
 *
 * FlexScreen Implementation
 *
 *----------------------------------------------------------------------------*/

Button::ButtonCallback FlexScreen::_buttonDelegate;

FlexScreen::~FlexScreen() {
  // TO DO: Cleanup!
}

bool FlexScreen::init(
    JsonObjectConst& screen,
    uint32_t refreshInterval,
    const Basics::ReferenceMapper &mapper)
{
  FlexItem::init();

  _mapper = mapper;
  _refreshInterval = refreshInterval;

  buttons = new Button[(nButtons = 1)];
  buttons[0].init(0, 0, Display::Width, Display::Height, _buttonDelegate, 0);

  _clock = NULL;
  return fromJSON(screen);
}

void FlexScreen::display(bool activating) {
  if (activating) { tft.fillScreen(_bkg); }
  for (int i = 0; i < _nItems; i++) {
    _items[i].display(_bkg, _mapper);
  }
  lastDisplayTime = lastClockTime = millis();
}

void FlexScreen:: processPeriodicActivity() {
  uint32_t curMillis = millis();
  if (curMillis - lastDisplayTime > _refreshInterval) display(false);
  else if (_clock != NULL  && (curMillis - lastClockTime > 1000L)) {
    _clock->display(_bkg, _mapper);
    lastClockTime = curMillis;
  }
}

// ----- Private functions

bool FlexScreen::fromJSON(JsonObjectConst& screen) {
  // TO DO: If we are overwriting an existing screen
  // we need to clean up all the old data first

  JsonArrayConst itemArray = screen[F("items")];
  _nItems = itemArray.size();  
  _items = new FlexItem[_nItems];

  int i = 0;
  for (JsonObjectConst item : itemArray) {
    _items[i].fromJSON(item);
    if (_items[i]._dataType == FlexItem::Type::CLOCK) {
      _clock = &_items[i];
    }
    i++;
  }

  _bkg = mapColor(screen[F("bkg")].as<String>());
  _screenID = screen[F("screenID")].as<String>();

  return true;
}


/*------------------------------------------------------------------------------
 *
 * FlexItem Implementation
 *
 *----------------------------------------------------------------------------*/

String FlexItem::_val; // Temporary space shared by all FlexItems

void FlexItem::fromJSON(JsonObjectConst& item) {
  // What it is...
  _dataType = mapType(item[F("type")].as<String>());
  _key = item[F("key")].as<String>();

  // Where it goes...
  _x = item[F("x")]; _y = item[F("y")];
  _w = item[F("w")]; _h = item[F("h")];
  _xOff = item[F("xOff")]; _yOff = item[F("yOff")];

  // How it is displayed...
  mapFont(item[F("font")].as<String>(), _gfxFont, _font);
  _color = mapColor(item[F("color")].as<String>());
  _format = String(item[F("format")]|"");
  _datum = mapDatum(item[F("justify")].as<String>());

  _strokeWidth = item[F("strokeWidth")];
}

void FlexItem::display(uint16_t bkg, Basics::ReferenceMapper mapper) {
  const char *fmt = _format.c_str();

  if (fmt[0] != 0) {
    // Reuse the same value buffer across all FlexItems, so clear it out
    Basics::resetString(_val);
    
    mapper(_key, _val);

    sprite->setColorDepth(1);
    sprite->createSprite(_w, _h);
    sprite->fillSprite(Theme::Mono_Background);

    // TO DO: Use snprintf to determine the correct buffer size
    int bufSize = Display::Width/6 + 1; // Assume 6 pixel spacing is smallest font
    char buf[bufSize];

    switch (_dataType) {
      case FlexItem::Type::INT: sprintf(buf, fmt, _val.toInt()); break;
      case FlexItem::Type::FLOAT: sprintf(buf, fmt, _val.toFloat()); break;
      case FlexItem::Type::STRING: sprintf(buf, fmt, _val.c_str()); break;
      case FlexItem::Type::BOOL: {
        char c = _val[0];
        bool bv = (c == 't' || c == 'T' || c == '1') ;
        sprintf(buf, fmt, bv ? F("True") : F("False"));
        break;
      }
      case FlexItem::Type::CLOCK: {
        int firstDelim = _val.indexOf('|');
        int secondDelim = _val.lastIndexOf('|');
        int theHour = _val.substring(0, firstDelim).toInt();
        int theMinute = _val.substring(firstDelim+1, secondDelim).toInt();
        int theSecond = _val.substring(secondDelim+1).toInt();
        sprintf(buf, fmt, theHour, theMinute, theSecond);
        break;
      }
      case FlexItem::Type::STATUS:
        // A Status value consists of a number (often a status code) and a message
        // The form is code|message
        int index = _val.indexOf('|');
        if (index != -1) {
          String msg = _val.substring(0, index);
          int code = _val.substring(index+1).toInt();
          if (strncasecmp(fmt, "#progress", 9) == 0) {
            String showPct = Basics::EmptyString;
            if (fmt[9] == '|' && fmt[10] != 0) showPct = String(&fmt[10]);
            Button b(_x, _y, _w, _h, NULL, 0);
            b.drawProgress(
              ((float)code)/100.0, msg, _font, _strokeWidth,
              Theme::Color_Border, Theme::Color_NormalText, 
              _color, bkg, showPct, true);
            return;
          } else {
            sprintf(buf, fmt, _val.c_str(), code);
          }
        }
        break;
    }
    if (_gfxFont >= 0) { Display::Font::setUsingID(_gfxFont, sprite); }
    else { sprite->setTextFont(_font);}
    sprite->setTextColor(Theme::Mono_Foreground);
    sprite->setTextDatum(_datum);
    sprite->drawString(buf, _xOff, _yOff);

    sprite->setBitmapColor(_color, bkg);
    sprite->pushSprite(_x, _y);
    sprite->deleteSprite();
  }

  for (int i = 0; i < _strokeWidth; i++) {
    tft.drawRect(_x+i, _y+i, _w-2*i, _h-2*i, _color);
  }
}

