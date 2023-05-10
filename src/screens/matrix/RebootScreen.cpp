/*
 * RebootScreen:
 *    Confirmation screen to trigger a reboot 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <TimeLib.h>
//                                  WebThing Includes
//                                  Local Includes
#include "../../gui/Display.h"
#include "../../gui/Theme.h"
#include "../../gui/ScreenMgr.h"
#include "RebootScreen.h"
//--------------- End:    Includes ---------------------------------------------


static constexpr int ConfirmationButton = 1;
static constexpr int CancelButton = 2;

RebootScreen::RebootScreen() {
  init(false);

  buttonHandler =[this](int id, PressType type) -> void {
    Log.verbose(F("In RebootScreen ButtonHandler, id = %d"), id);
    if (id == ConfirmationButton && type > PressType::Normal) { ESP.restart(); }
    if (id == CancelButton) ScreenMgr.displayHomeScreen();
  };

  // Buttons are specified by the app when it is ready. See setButtons() below
  buttonMappings = &confirmCancelMappings[0];
  nButtonMappings = 0;
}

void RebootScreen::innerActivation() {
  if (nButtonMappings == 0) {
    setText("Rebooting...");
  } else {
    setText("Reboot request! Adv to reboot, Prev to cancel");
  }
  autoCancelTime = millis() + 60 * 1000L; // If nothing has happened in a minute, cancel
Log.verbose("RebootScreen::innerActivation: autoCancelTime = %d", autoCancelTime);
}

bool RebootScreen::innerPeriodic() {
static uint32_t ggg = 5000;
  if (millis() >= autoCancelTime) {
    ScreenMgr.displayHomeScreen();
    return true;
  }
  if (millis() > ggg) {
    ggg = millis() + 5000L;
    Log.verbose("RebootScreen::innerPeriodic: ggg = %d", ggg);
  }
  return false;
}

void RebootScreen::setButtons(Basics::Pin confirmPin, Basics::Pin cancelPin) {
  nButtonMappings = 0;
  if (confirmPin != Basics::UnusedPin) confirmCancelMappings[nButtonMappings++] = {confirmPin, ConfirmationButton};
  if (cancelPin != Basics::UnusedPin) confirmCancelMappings[nButtonMappings++] = {cancelPin, CancelButton};
}

#endif

