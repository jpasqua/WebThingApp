/*
 * BaseFlexScreen:
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
#include <Output.h>
#include <DataBroker.h>
//                                  Local Includes
#include "Display.h"
#include "Theme.h"
#include "FlexScreen.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Local Utility Functions
 *
 *----------------------------------------------------------------------------*/

inline uint16_t mapColor(String colorSpecifier) {
  uint32_t hexVal = strtol(colorSpecifier.c_str(), nullptr, 16);

  uint16_t r = (hexVal >> 8) & 0xF800;
  uint16_t g = (hexVal >> 5) & 0x07E0;
  uint16_t b = (hexVal >> 3) & 0x001F;

  return (r | g | b);
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
  if (justify.equalsIgnoreCase(F("TL"))) { return BaseDisplay::TL_Align;}
  if (justify.equalsIgnoreCase(F("TC"))) { return BaseDisplay::TC_Align;}
  if (justify.equalsIgnoreCase(F("TR"))) { return BaseDisplay::TR_Align;}
  if (justify.equalsIgnoreCase(F("ML"))) { return BaseDisplay::ML_Align;}
  if (justify.equalsIgnoreCase(F("MC"))) { return BaseDisplay::MC_Align;}
  if (justify.equalsIgnoreCase(F("MR"))) { return BaseDisplay::MR_Align;}
  if (justify.equalsIgnoreCase(F("BL"))) { return BaseDisplay::BL_Align;}
  if (justify.equalsIgnoreCase(F("BC"))) { return BaseDisplay::BC_Align;}
  if (justify.equalsIgnoreCase(F("BR"))) { return BaseDisplay::BR_Align;}

  return BaseDisplay::TL_Align;
}

// Requires device-specific code
void mapFont(String fontName, int8_t& gfxFont, uint8_t& font) {
  // Use a default if no matching font is found
  font = 2;
  gfxFont = -1;

  if (fontName.length() == 1 && isDigit(fontName[0])) {
    font = fontName[0] - '0';
    return;
  } 

  gfxFont = Display.fontIDFromName(fontName);
} 

/*------------------------------------------------------------------------------
 *
 * BaseFlexScreen Implementation
 *
 *----------------------------------------------------------------------------*/

Screen::ButtonHandler BaseFlexScreen::_buttonDelegate;

BaseFlexScreen::~BaseFlexScreen() {
  // TO DO: Cleanup!
}

bool BaseFlexScreen::init(
    JsonObjectConst& screen,
    uint32_t refreshInterval,
    const Basics::ReferenceMapper &mapper)
{
  FlexItem::init();

  _mapper = mapper;
  _refreshInterval = refreshInterval;

  labels = new Label[(nLabels = 1)];
  labels[0].init(0, 0, Display.Width, Display.Height, 0);
  buttonHandler = _buttonDelegate;

  _clock = nullptr;
  return fromJSON(screen);
}

void BaseFlexScreen::renderProgressBar(FlexItem& item, const char* buf) {
  // buf is of the form:
  // message|percent     OR     message
  // If no perent is given, then the message is displayed within a full
  // progress bar. Otherwise a progress bar will be shown 'percent' full
  // with the percentage displayed within (eg 47%)
  int pct = 100;
  char* delimiter = strchr(buf, '|');
  if (delimiter) {
    pct = atoi(delimiter+1);
    *delimiter = '\0';
  }
Log.verbose("BaseFlexScreen::renderProgressBar: buf = %s, delimiter==null: %T",
  buf, (delimiter == nullptr));
  Label progressLabel(item._x, item._y, item._w, item._h, 0);
  progressLabel.drawProgress(
    ((float)pct)/100.0, buf, item._font, item._strokeWidth,
    Theme::Color_Border, Theme::Color_NormalText, 
    item._color, _bkg, (delimiter!=nullptr), true);
}

void BaseFlexScreen::displayItem(FlexItem& item) {
  int bufSize = Display.width()/6 + 1; // Assume 6 pixel spacing is smallest font
  char buf[bufSize];  

  item.generateText(buf, bufSize, _mapper);

  if (item._format.equalsIgnoreCase("#progress")) {
    renderProgressBar(item, buf);
    return;
  }

  Display.drawStringInRegion(
    buf, ((item._gfxFont >= 0) ? item._gfxFont : -item._font), item._datum,
    item._x, item._y, item._w, item._h, item._xOff, item._yOff,
    item._color, _bkg);

  for (int i = 0; i < item._strokeWidth; i++) {
    // Requires device-specific code
    Display.drawRect(item._x+i, item._y+i, item._w-2*i, item._h-2*i, item._color);
  }
}
void BaseFlexScreen::display(bool activating) {
  // Requires device-specific code
  if (activating) { Display.fillRect(0, 0, Display.Width, Display.Height, _bkg); }

  for (int i = 0; i < _nItems; i++) {
    displayItem(_items[i]);
  }
  Display.flush();
  lastDisplayTime = lastClockTime = millis();
}

void BaseFlexScreen::processPeriodicActivity() {
  uint32_t curMillis = millis();
  if (curMillis - lastDisplayTime > _refreshInterval) display(false);
  else if (_clock != nullptr  && (curMillis - lastClockTime > 1000L)) {
    displayItem(*_clock);
    lastClockTime = curMillis;
  }
}

// ----- Private functions

bool BaseFlexScreen::fromJSON(JsonObjectConst& screen) {
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

void FlexItem::generateText(char* buf, int bufSize, Basics::ReferenceMapper mapper) {
  const char *fmt = _format.c_str();

  if (fmt[0] != 0) {
    Basics::resetString(_val);    // Reuse the same value buffer across all FlexItems, so clear it out
    
    mapper(_key, _val);

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
        time_t curTime = now();
        sprintf(buf, fmt, Output::adjustedHour(hour(curTime)), minute(curTime), second(curTime));
        break;
      }
      case FlexItem::Type::STATUS: {
Log.verbose("FlexItem::generateText: STATUS _val = %s", _val.c_str());
        _val.toCharArray(buf, bufSize);
      }
    }
  } else {
    buf[0] = '\0';
  }
}

