/*
 * ScreenMgr
 *    Manages the screens used in a WebThing app
 *
 */


//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <map>
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

ScreenManager ScreenMgr;


/*------------------------------------------------------------------------------
 *
 * BlankScreen Implementation
 *
 *----------------------------------------------------------------------------*/

class BlankScreen : public Screen {
public:
  uint8_t lastBrightness;

  BlankScreen() {
    auto buttonHandler =[this](int /*id*/, Button::PressType /*type*/) -> void {
      Log.verbose("Waking up with millis() = %d", millis());
      // Let the screen redisplay while the brightness is off...
      ScreenMgr.unsuspend();
      // ...then let there be light
      Display::setBrightness(lastBrightness);
    };

    buttons = new Button[(nButtons = 1)];
    buttons[0].init(0, 0, Display::Width, Display::Height, buttonHandler, 0);
  }

  void display(bool) override {
    lastBrightness = Display::getBrightness();
    Display::setBrightness(0);
  }

  void processPeriodicActivity() override { }
} blankScreen;


/*------------------------------------------------------------------------------
 *
 * ScreenMgr Implementation
 *
 *----------------------------------------------------------------------------*/

//
// ----- Private Member Functions
//

void ScreenManager::unsuspend() {
  if (_suspendedScreen) {
    display(_suspendedScreen);
    _suspendedScreen = nullptr;
  }
}

void ScreenManager::suspend() {
  _suspendedScreen = _curScreen;
  display(&blankScreen);
}

void ScreenManager::processSchedules() {
  static uint32_t eveningExecutedOnDay = UINT32_MAX;
  static uint32_t morningExecutedOnDay = UINT32_MAX;
  static uint32_t nextScheduleCheck = 0;

  if (_uiOptions->schedule.active) {
    uint16_t morning = _uiOptions->schedule.morning.hr * 100 + _uiOptions->schedule.morning.min;
    uint16_t evening = _uiOptions->schedule.evening.hr * 100 + _uiOptions->schedule.evening.min;
    uint32_t curMillis = millis();
    uint32_t today = day();
    if (curMillis > nextScheduleCheck) {
      nextScheduleCheck = curMillis + (60-second()) * 1000L;
      uint16_t curTime = hour() * 100 + minute();
      if (curTime >= evening || curTime < morning) {
        if (eveningExecutedOnDay != today) {
          Display::setBrightness(_uiOptions->schedule.evening.brightness);
          eveningExecutedOnDay = today;
        }
      } else if (morningExecutedOnDay != today) {
        Display::setBrightness(_uiOptions->schedule.morning.brightness);
        morningExecutedOnDay = today;
      }
    }
  }
}

void ScreenManager::processInput() {
  uint32_t curMillis = millis();
  uint16_t tx = 0, ty = 0;
  bool pressed = tft.getTouch(&tx, &ty);
  if (pressed) _lastInteraction = curMillis;
  _curScreen->processInput(pressed, tx, ty);
  _pbMgr->processInput();

  // Test whether we should blank the screen
  if (_uiOptions->screenBlankMinutes && !isSuspended()) {
    if (curMillis > _lastInteraction + minutesToMS(_uiOptions->screenBlankMinutes)) suspend();
  }
}

//
// ----- Public Member Functions
//

void ScreenManager::setup(UIOptions* uiOptions, DisplayOptions* displayOptions, PhysicalButtonMgr* pbMgr) {
  Log.verbose("ScreenManager::setup called");
  _uiOptions = uiOptions;
  _displayOptions = displayOptions;
  _pbMgr = pbMgr;

  auto dispatcher = [this](uint8_t id, BaseButton::PressType pt) {
    Log.verbose("Physical button %d was pressed. PressType: %d", id, pt);
    _lastInteraction = millis();
    if (!_curScreen->physicalButtonHandler) return;

    auto mapping = _curScreen->screenButtonFromPhysicalButton.find(id);
    if (mapping != _curScreen->screenButtonFromPhysicalButton.end()) {
      _curScreen->physicalButtonHandler(mapping->second, pt);
    }
  };

  _pbMgr->setDispatcher(dispatcher);
  Display::begin(_displayOptions);
  _lastInteraction = millis();
  activityIcon.init();
}

void ScreenManager::loop() {
  if (_curScreen == NULL) return;
  // processSchedules();
  processInput();
  _curScreen->processPeriodicActivity();
}


bool ScreenManager::registerScreen(String screenName, Screen* theScreen) {
  if (screenFromName[screenName] != NULL) {
    Log.warning("Trying to register a screen with a name that is in use: %s", screenName.c_str());
    return false;
  }
  screenFromName[screenName] = theScreen;
  return true;
}

void ScreenManager::setAsHomeScreen(Screen* screen) {
  _homeScreen = screen;
}

void ScreenManager::display(String name) {
  Screen* screen = screenFromName[name];
  if (screen == NULL) {
    Log.warning("Trying to display screen %s, but none is registered", name.c_str());
    return;
  }
  display(screen);
}

void ScreenManager::display(Screen* screen) {
  _curScreen = screen;
  screen->activate();
}

void ScreenManager::displayHomeScreen() {
  if (_homeScreen == NULL) {
    Log.warning("Trying to display a home screen when none has been set");
    return;
  }
  display(_homeScreen);
}

FlexScreen* ScreenManager::createFlexScreen(
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



/*------------------------------------------------------------------------------
 *
 * ActivityIcon Implementation
 *
 *----------------------------------------------------------------------------*/

void ActivityIcon::init() {
  // In theory it would be better to allocate/deallocate this as needed, but it causes
  // a lot more fragmentation and potentially a crash.
  savedPixels = (uint16_t *)malloc(Size*Size*sizeof(uint16_t));  // This is BIG!
}

// TO DO: Consider passing a reference to a string rather than a char
// Might end up faster than constructing a string from the char each time
void ActivityIcon::show(uint16_t accentColor, char symbol) {
  if (isDisplayed) return;

  tft.readRect(X, Y, Size, Size, savedPixels);
  uint16_t centerX = X+(Size/2);
  uint16_t centerY = Y+(Size/2);
  tft.fillCircle(centerX, centerY, Size/2-1, accentColor);
  tft.fillCircle(centerX, centerY, (Size/2-1)-BorderSize, Theme::Color_UpdatingFill);
  tft.setTextDatum(MC_DATUM);
  tft.setFreeFont(&FreeSansBold9pt7b);
  tft.setTextColor(Theme::Color_UpdatingText);

  char buf[2];
  buf[0] = symbol; buf[1] = '\0';
  tft.drawString(buf, centerX, centerY);

  isDisplayed = true;
}

void ActivityIcon::hide() {
  if (!isDisplayed) return;
  tft.pushRect(X, Y, Size, Size, savedPixels);
  isDisplayed = false;
}
