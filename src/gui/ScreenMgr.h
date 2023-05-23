#ifndef ScreenMgr_h
#define ScreenMgr_h

/*
 * ScreenMgr
 *    Manages the screens used in a WebThing app
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
#include <functional>
#include <map>
//                                  Third Party Libraries
#include <ArduinoJson.h>
//                                  WebThing Includes
#include <BPABasics.h>
//                                  Local Includes
#include "UIOptions.h"
#include "DisplayOptions.h"
#include "FlexScreen.h"
#include "ScreenSettings.h"
//--------------- End:    Includes ---------------------------------------------


class BaseScreenMgr {
public:
  // ----- Constructors
  BaseScreenMgr() = default;
  ~BaseScreenMgr() = default;

  // ----- Member Functions
  void setup(UIOptions* uiOptions, DisplayOptions* displayOptions);
  void loop();

  // ----- Screen Management functions
  bool registerScreen(String screenName, Screen* theScreen, bool special = false);
  void setAsHomeScreen(Screen* screen);
  Screen* screenFromName(String& name);

  // ----- Screen Display functions
  void display(String name);
  void display(Screen* screen);
  void displayHomeScreen();
  void refresh();
  void setBrightness(uint8_t b); // Respects screen blanking

  // ----- Screen Sequence functions
  using ScreenSequence = std::vector<Screen*>;
  void setSequence(ScreenSequence& newSequence) { sequence = std::move(newSequence); }
  void reconcileScreenSequence(ScreenSettings& screenSettings);
  void beginSequence();
  void moveThroughSequence(bool forward);
  void setSequenceButtons(uint8_t forward, uint8_t backward = 255);
  Screen* curScreen() { return _curScreen; }

  // ----- Plugin-related functions
  FlexScreen* createFlexScreen(
      const JsonDocument &doc,
      uint32_t refreshInterval,
      const Basics::ReferenceMapper &mapper);

  // ----- ActivityIcon functions
  /**
   * Overlay the current screen with an icon to indicate that a potentially
   * long-running activity is in progress. This lets the user know that the UI
   * will be unresponsive in this period. Calling show()
   * when the icon is already displayed is safe and does nothing.
   * @param   accentColor   An accent color to indicate what's happening
   * @param   symbol        The symbol to display within the icon
   */
  virtual void showActivityIcon(uint16_t accentColor, char symbol = 'i')  = 0;

  /**
   * Remove the activity icon from the current screen and restore the original
   * screen content. Calling hide() when no icon is displayed
   * is safe and does nothing.
   */
  virtual void hideActivityIcon() = 0;
  
  // TO DO: These should be private (or at least protected), but the BlankScreen
  // class needs to access them. Think about whether it's worth doing something about this.
  void suspend();
  void unsuspend();

  std::vector<Screen*> allScreens;
  ScreenSequence sequence;

protected:
  // ----- Member Functions
  void processSchedules();
  inline bool isSuspended() { return(_suspendedScreen != nullptr); }

  virtual void device_setup() = 0;
  virtual void device_processInput() = 0;
  virtual void device_changingScreens() { }
  
  // ----- Data Members
  Screen*   _curScreen;
  Screen*   _homeScreen;
  Screen*   _suspendedScreen = nullptr;

  uint8_t _curSequenceIndex = 0;
  uint8_t _forwardButton = 255;   // No pin assigned
  uint8_t _backwardButton = 255;  // No pin assigned

  UIOptions*      _uiOptions;
  DisplayOptions* _displayOptions;

  uint32_t _lastInteraction = 0;
};

#include "devices/DeviceSelect.h"
#include DeviceImplFor(ScreenMgr)

#endif  // ScreenMgr_h