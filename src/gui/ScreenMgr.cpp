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


/*------------------------------------------------------------------------------
 *
 * BlankScreen Implementation
 *
 *----------------------------------------------------------------------------*/

class BlankScreen : public Screen {
public:
  uint8_t lastBrightness = 100;
  bool blanking = false;

  BlankScreen() {
    buttonHandler =[this](int /*id*/, PressType /*type*/) -> void {
      Log.verbose("Waking up with millis() = %d", millis());
      ScreenMgr.unsuspend();  // Let the screen redisplay while the brightness is off...
Log.verbose("restoring brightness to %d", lastBrightness);
      Display.setBrightness(lastBrightness);  // ...then let there be light
      blanking = false;       // Remember we are no longer blanking
    };

    labels = new Label[(nLabels = 1)];
    labels[0].init(0, 0, Display.Width, Display.Height, 0);

    nButtonMappings = PassAllRawButtons;

    blanking = false;
  }

  void display(bool) override {
    if (!blanking) {
      blanking = true;
      lastBrightness = Display.getBrightness();
      Display.setBrightness(0);
    }
  }

  void processPeriodicActivity() override { }

  void updateSavedBrightness(uint8_t b) {
    lastBrightness = b;
Log.verbose("Updated lastBrightness to %d", lastBrightness);
  }

} blankScreen;


/*------------------------------------------------------------------------------
 *
 * BaseScreenMgr Implementation
 *
 *----------------------------------------------------------------------------*/

void BaseScreenMgr::setup(UIOptions* uiOptions, DisplayOptions* displayOptions) {
  _uiOptions = uiOptions;
  _displayOptions = displayOptions;

  auto dispatcher = [this](uint8_t pin, PressType pt) {
    Log.verbose("Physical button %d was pressed. PressType: %d", pin, pt);
    _lastInteraction = millis();

    if (_curScreen->physicalButtonPress(pin, pt))  return;  // We consumed the press
      // Note that a screen can choose to consume the forward or backward buttons
      // so the lines below may never be reached, even if they were pressed.
    if (pin == _forwardButton) moveThroughSequence(true);
    else if (pin == _backwardButton) moveThroughSequence(false);
  };

  WebThing::buttonMgr.setDispatcher(dispatcher);
  _lastInteraction = millis();
  device_setup();
}

void BaseScreenMgr::loop() {
  if (_curScreen == NULL) return;
  processSchedules();
  device_processInput();

  // Test whether we should blank the screen
  if (_uiOptions->screenBlankMinutes && !isSuspended()) {
    if (millis() > _lastInteraction + minutesToMS(_uiOptions->screenBlankMinutes)) suspend();
  }

  _curScreen->processPeriodicActivity();
}


bool BaseScreenMgr::registerScreen(String screenName, Screen* theScreen) {
  if (screenFromName[screenName] != NULL) {
    Log.warning("Trying to register a screen with a name that is in use: %s", screenName.c_str());
    return false;
  }
  screenFromName[screenName] = theScreen;
  return true;
}

void BaseScreenMgr::setAsHomeScreen(Screen* screen) {
  _homeScreen = screen;
}

void BaseScreenMgr::display(String name) {
  Screen* screen = screenFromName[name];
  if (screen == NULL) {
    Log.warning("Trying to display screen %s, but none is registered", name.c_str());
    return;
  }
  display(screen);
}

void BaseScreenMgr::display(Screen* screen) {
  _curScreen = screen;
  screen->activate();
}

void BaseScreenMgr::displayHomeScreen() {
  if (_homeScreen == NULL) {
    Log.warning("Trying to display a home screen when none has been set");
    return;
  }
  display(_homeScreen);
}

void BaseScreenMgr::refresh() {
  if (_curScreen) _curScreen->display(true);
}

FlexScreen*  BaseScreenMgr::createFlexScreen(
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


void BaseScreenMgr::unsuspend() {
  if (_suspendedScreen) {
    display(_suspendedScreen);
    _suspendedScreen = nullptr;
  }
}

void BaseScreenMgr::suspend() {
  _suspendedScreen = _curScreen;
  display(&blankScreen);
}

void BaseScreenMgr::processSchedules() {
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
          if (isSuspended()) blankScreen.updateSavedBrightness(_uiOptions->schedule.evening.brightness);
          else Display.setBrightness(_uiOptions->schedule.evening.brightness);
          eveningExecutedOnDay = today;
        }
      } else if (morningExecutedOnDay != today) {
        if (isSuspended()) blankScreen.updateSavedBrightness(_uiOptions->schedule.morning.brightness);
        else Display.setBrightness(_uiOptions->schedule.morning.brightness);
        morningExecutedOnDay = today;
      }
    }
  }
}

void BaseScreenMgr::setSequenceButtons(uint8_t forward, uint8_t backward) {
  _forwardButton = forward;
  _backwardButton = backward;
}

void BaseScreenMgr::beginSequence() {
  if (_sequence == nullptr || _sequence->empty()) { displayHomeScreen(); return; }
  _curSequenceIndex = 0;
  display(_sequence->at(_curSequenceIndex));
}

void BaseScreenMgr::moveThroughSequence(bool forward) {
  if (_sequence == nullptr || _sequence->empty()) { displayHomeScreen(); return; }
  if (forward) {
    _curSequenceIndex++;
    if (_curSequenceIndex == _sequence->size()) _curSequenceIndex = 0;
  } else {
    if (_curSequenceIndex == 0) _curSequenceIndex = _sequence->size()-1;
    else _curSequenceIndex--;
  }
  display(_sequence->at(_curSequenceIndex));
}


