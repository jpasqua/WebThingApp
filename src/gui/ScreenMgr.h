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
//                                  Third Party Libraries
#include <ArduinoJson.h>
//                                  WebThing Includes
#include <BPABasics.h>
//                                  Local Includes
#include "UIOptions.h"
#include "DisplayOptions.h"
#include "PhysicalButtonMgr.h"
#include "FlexScreen.h"
//--------------- End:    Includes ---------------------------------------------


namespace ScreenMgr {

  void setup(UIOptions* uiOptions, DisplayOptions* displayOptions, PhysicalButtonMgr* pbMgr);
  void loop();

  // ----- Screen Management functions
  bool registerScreen(String screenName, Screen* theScreen);
  void setAsHomeScreen(Screen* screen);
  Screen* find(String name);

  // ----- Screen Display functions
  void display(String name);
  void display(Screen* screen);
  void displayHomeScreen();

  /**
   * Overlay the current screen with an icon to indicate that a potentially
   * long-running update is in progress. this lets the user know that the UI
   * will be unresponsive in this period. Calling showUpdatingIcon()
   * when the icon is already displayed is safe and does nothing.
   * @param   accentColor   An accent color to indicate what's happening
   * @param   symbol        The symbol to display within the icon
   */
  void showUpdatingIcon(uint16_t accentColor, char symbol = 'i');

  /**
   * Remove the "updating icon" from the current screen and restore the original
   * screen content. Calling hideUpdatingIcon() when no icon is displayed
   * is safe and does nothing.
   */
  void hideUpdatingIcon();

  // ----- Plugin-related functions
  FlexScreen* createFlexScreen(
      const JsonDocument &doc,
      uint32_t refreshInterval,
      const Basics::ReferenceMapper &mapper);


};

#endif  // ScreenMgr_h