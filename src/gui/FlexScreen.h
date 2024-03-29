/*
 * BaseFlexScreen:
 *    Display values driven by a screen layout definition 
 *
 * NOTES:
 * o BaseFlexScreen is central to the Plugin system for WebThing applications
 * o It creates a Screen driven by a definition given in a JSON document
 * o That document specifies the layout of the elements on the screen
 *   and the data that should be used to fill those elements
 * o The data is accessed via the WebThing DataBroker. The JSON document
 *   provides the keys to be used and BaseFlexScreen uses the DataBroker to
 *   get values associated with the keys at runtime.
 * o There is only one user interaction defined for a BaseFlexScreen. Touching
 *   anywhere on the screen invoked the ButtonDelegate supplied when the
 *   BaseFlexScreen is instantiated.
 */

#ifndef BaseFlexScreen_h
#define BaseFlexScreen_h


//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoJson.h>
#include <BPABasics.h>
//                                  WebThing Includes
//                                  Local Includes
#include "Screen.h"
#include "Label.h"
//--------------- End:    Includes ---------------------------------------------


class FlexItem {
public:
  using Type = enum {INT, FLOAT, STRING, BOOL, CLOCK, STATUS};

  void fromJSON(JsonObjectConst& item);
  void generateText(char* buf, int bufSize, Basics::ReferenceMapper mapper);

  static String _val; // Temporary mapping string shared by all FlexItems
  static void init() { _val.reserve(32); }

  uint16_t _x, _y;    // Location of the field
  uint16_t _w, _h;    // Size of the field
  uint16_t _xOff;     // x offset of text within field
  uint16_t _yOff;     // y offset of text within field
  int8_t   _gfxFont;  // The ID of the GFXFont to use. If negative, use a built-in font
  uint8_t  _font;     // Font to use if no GFXFont was given
  uint16_t _color;    // Color to use
  String   _format;   // Format string to use when displaying the value
  uint8_t  _datum;    // Justification of the output
  uint8_t _strokeWidth; 

  String _key;        // The key that will be used to get the value
  Type _dataType;
};


class BaseFlexScreen : public Screen {
public:
  static void setButtonDelegate(Screen::ButtonHandler delegate) { _buttonDelegate = delegate; }

  // ----- Functions that are specific to BaseFlexScreen
  virtual ~BaseFlexScreen();

  bool init(
      JsonObjectConst& screen,
      uint32_t refreshInterval,
      const Basics::ReferenceMapper &mapper);
  String getScreenID() { return _screenID; }

  // ----- Functions defined in Screen class
  virtual void display(bool activating = false);
  virtual void processPeriodicActivity();

  virtual void displayItem(FlexItem& item);
  virtual void renderProgressBar(FlexItem& item, const char* buf);

protected:
  static   Screen::ButtonHandler _buttonDelegate;

  FlexItem* _items;             // An array of items on the screen
  uint8_t   _nItems;            // Number of items
  uint16_t  _bkg;               // Background color of the screen
  String    _screenID;          // UUID of the screen. Not used in the UI, but human readable
                                // is helpful for testing / debugging
  uint32_t _refreshInterval;    // How often to refresh the display
  Basics::ReferenceMapper _mapper; // Maps a key from thee screen definition to a value
  uint32_t  lastDisplayTime;    // Last time the display() function ran
  uint32_t  lastClockTime;
  FlexItem* _clock;

  bool fromJSON(JsonObjectConst& screen);
};

#include "devices/DeviceSelect.h"
#include DeviceImplFor(FlexScreen)

#endif  // BaseFlexScreen_h
