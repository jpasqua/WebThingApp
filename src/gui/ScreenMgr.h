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
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * ScreenMgr Definition
 *
 *----------------------------------------------------------------------------*/

class ScreenManager {
public:
  // ----- Constructors
  ScreenManager() = default;

  // ----- Member Functions
  void setup(UIOptions* uiOptions, DisplayOptions* displayOptions);
  void loop();

  // ----- Screen Management functions
  bool registerScreen(String screenName, Screen* theScreen);
  void setAsHomeScreen(Screen* screen);
  Screen* find(String name);

  // ----- Screen Display functions
  void display(String name);
  void display(Screen* screen);
  void displayHomeScreen();

  void suspend();
  void unsuspend();
  inline bool isSuspended() { return(_suspendedScreen != nullptr); }

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
  void showActivityIcon(uint16_t accentColor, char symbol = 'i');

  /**
   * Remove the activity icon from the current screen and restore the original
   * screen content. Calling hide() when no icon is displayed
   * is safe and does nothing.
   */
  void hideActivityIcon();


protected:
  // ----- Member Functions
  void processSchedules();
  void processInput();
  void initActivityIcon();

  // ----- Data Members
  std::map<String, Screen*> screenFromName;
  Screen*   _curScreen;
  Screen*   _homeScreen;
  Screen*   _suspendedScreen = nullptr;

  UIOptions*      _uiOptions;
  DisplayOptions* _displayOptions;

  uint32_t _lastInteraction = 0;

private:
    bool isDisplayed;
    uint16_t *savedPixels = NULL;
};

extern ScreenManager ScreenMgr;
#endif  // ScreenMgr_h