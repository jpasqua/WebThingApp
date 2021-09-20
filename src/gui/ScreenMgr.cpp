/*
 * ScreenMgr
 *    Manages the screens used in a WebThing app
 *
 */


//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <TimeLib.h>
//                                  WebThing Includes
#include <WebThing.h>
//                                  Local Includes
#include "Display.h"
#include "ScreenMgr.h"
#include "Theme.h"
#include "FlexScreen.h"
//--------------- End:    Includes ---------------------------------------------

using Display::tft;

namespace ScreenMgr {
  namespace State {
    constexpr uint8_t MaxScreens = 20;

    struct {
      String name;
      Screen* screen;
    } map[MaxScreens];

    uint8_t nScreens = 0;
    Screen* curScreen;
    Screen* homeScreen;

    UIOptions* uiOptions;
    DisplayOptions* displayOptions;

    uint32_t lastInteraction = 0;
    Screen* suspendedScreen = nullptr;
  };

  namespace Internal {
    constexpr uint16_t InfoIconSize = 32;
    constexpr uint16_t InfoIconBorderSize = 5;
    constexpr uint16_t InfoIconX = (320 - InfoIconSize);
    constexpr uint16_t InfoIconY = 0;
    uint16_t *savedPixels = NULL;
    bool infoIconIsDisplayed = false;

    void unsuspend() {
      if (State::suspendedScreen) {
        display(State::suspendedScreen);
        State::suspendedScreen = nullptr;
      }
    }

    class BlankScreen : public Screen {
    public:
      uint8_t lastBrightness;

      BlankScreen() {
        auto buttonHandler =[this](int id, Button::PressType type) -> void {
          (void)id;   // We don't use this parameter - avoid a warning...
          (void)type; // We don't use this parameter - avoid a warning...
          Log.verbose("Waking up with millis() = %d", millis());
          // Let the screen redisplay while the brightness is off...
          ScreenMgr::Internal::unsuspend();
          // ...then let there be light
          Display::setBrightness(lastBrightness);
        };

        buttons = new Button[(nButtons = 1)];
        buttons[0].init(0, 0, Display::Width, Display::Height, buttonHandler, 0);
      }

      void display(bool activating = false) {
        (void)activating; // Not used, avoid warning
        lastBrightness = Display::getBrightness();
        Display::setBrightness(0);
      }

      void processPeriodicActivity() { }
    } blankScreen;

    void drawInfoIcon(
        uint16_t borderColor, char symbol, uint16_t fillColor, uint16_t textColor)
    {
      tft.readRect(InfoIconX, InfoIconY, InfoIconSize, InfoIconSize, savedPixels);
      uint16_t centerX = InfoIconX+(InfoIconSize/2);
      uint16_t centerY = InfoIconY+(InfoIconSize/2);
      tft.fillCircle(centerX, centerY, InfoIconSize/2-1, borderColor);
      tft.fillCircle(centerX, centerY, (InfoIconSize/2-1)-InfoIconBorderSize, fillColor);
      tft.setTextDatum(MC_DATUM);
      // tft.setFreeFont(&FreeSerifBoldItalic9pt7b);
      tft.setFreeFont(&FreeSansBold9pt7b);
      tft.setTextColor(textColor);
      tft.drawString(String(symbol), centerX, centerY);
    }

    void initInfoIcon() {
      // In theory it would be better to allocate/deallocate this as needed, but it causes
      // a lot more fragmentation and potentially a crash.
      savedPixels = (uint16_t *)malloc(InfoIconSize*InfoIconSize*sizeof(uint16_t));  // This is BIG!
    }

    void processSchedules() {
      static uint32_t eveningExecutedOnDay = UINT32_MAX;
      static uint32_t morningExecutedOnDay = UINT32_MAX;
      static uint32_t nextScheduleCheck = 0;

      if (State::uiOptions->schedule.active) {
        uint16_t morning = State::uiOptions->schedule.morning.hr * 100 + State::uiOptions->schedule.morning.min;
        uint16_t evening = State::uiOptions->schedule.evening.hr * 100 + State::uiOptions->schedule.evening.min;
        uint32_t curMillis = millis();
        uint32_t today = day();
        if (curMillis > nextScheduleCheck) {
          nextScheduleCheck = curMillis + (60-second()) * 1000L;
          uint16_t curTime = hour() * 100 + minute();
          if (curTime >= evening || curTime < morning) {
            if (eveningExecutedOnDay != today) {
              Display::setBrightness(State::uiOptions->schedule.evening.brightness);
              eveningExecutedOnDay = today;
            }
          } else if (morningExecutedOnDay != today) {
            Display::setBrightness(State::uiOptions->schedule.morning.brightness);
            morningExecutedOnDay = today;
          }
        }
      }
    }

    void processInput() {
      uint32_t curMillis = millis();
      uint16_t tx = 0, ty = 0;
      bool pressed = Display::tft.getTouch(&tx, &ty);
      if (pressed) State::lastInteraction = curMillis;
      State::curScreen->processInput(pressed, tx, ty);

      // Test whether we should blank the screen
      if (State::uiOptions->screenBlankMinutes && !State::suspendedScreen) {
        uint32_t timeToBlankScreen = State::lastInteraction +
            State::uiOptions->screenBlankMinutes * Basics::MillisPerMinute;
        if (curMillis > timeToBlankScreen) {
          Log.verbose("Going to sleep with millis() = %d", curMillis);
          State::suspendedScreen = State::curScreen;
          display(&Internal::blankScreen);
        }
      }
    }
  };  // ----- END: ScreenMgr::Internal

  void setup(UIOptions* uiOptions, DisplayOptions* displayOptions) {
    State::uiOptions = uiOptions;
    State::displayOptions = displayOptions;
    Display::begin(displayOptions);
    State::lastInteraction = millis();
    Internal::initInfoIcon();
  }

  void loop() {
    if (State::curScreen == NULL) return;
    Internal::processSchedules();
    Internal::processInput();
    State::curScreen->processPeriodicActivity();
  }


  bool registerScreen(String screenName, Screen* theScreen) {
    if (State::nScreens == State::MaxScreens) {
      Log.warning("No space for more screens while registering: %s", screenName.c_str());
      return false;
    }
    if (find(screenName) != NULL) {
      Log.warning("Trying to register a screen with a name that is in use: %s", screenName.c_str());
      return false;
    }
    State::map[State::nScreens].name = screenName;
    State::map[State::nScreens].screen = theScreen;
    State::nScreens++;
    return true;
  }

  void setAsHomeScreen(Screen* screen) {
    State::homeScreen = screen;
  }

  Screen* find(String name) {
    for (int i = 0; i < State::nScreens; i++) {
      if (State::map[i].name.equals(name)) return (State::map[i].screen);
    }
    return NULL;
  }

  void display(String name) {
    Screen* screen = find(name);
    if (screen == NULL) {
      Log.warning("Trying to display screen %s, but none is registered", name.c_str());
      return;
    }
    display(screen);
  }

  void display(Screen* screen) {
    State::curScreen = screen;
    screen->activate();
  }

  void displayHomeScreen() {
    if (State::homeScreen == NULL) {
      Log.warning("Trying to display a home screen when none has been set");
      return;
    }
    display(State::homeScreen);
  }

  FlexScreen* createFlexScreen(
      const JsonDocument &doc,
      uint32_t refreshInterval,
      const Basics::ReferenceMapper &mapper)
  {
    FlexScreen* flexScreen = new FlexScreen();
    JsonObjectConst descriptor = doc.as<JsonObjectConst>();
    if (!flexScreen->init(descriptor, refreshInterval, mapper)) {
      delete flexScreen;
      return NULL;
    }

    registerScreen(flexScreen->getScreenID(), flexScreen);
    return (flexScreen);
  }

  void showUpdatingIcon(uint16_t accentColor, char symbol) {
    if (Internal::infoIconIsDisplayed) return;
    Internal::drawInfoIcon(
      accentColor, symbol, Theme::Color_UpdatingFill, Theme::Color_UpdatingText);
    Internal::infoIconIsDisplayed = true;
  }

  void showUpdatingIcon(uint16_t accentColor) {
    showUpdatingIcon(accentColor, 'i');
  }

  void hideUpdatingIcon() {
    if (!Internal::infoIconIsDisplayed) return;
    tft.pushRect(
      Internal::InfoIconX, Internal::InfoIconY, Internal::InfoIconSize,
      Internal::InfoIconSize, Internal::savedPixels);
    Internal::infoIconIsDisplayed = false;
  }


};