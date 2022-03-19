/*
 * RebootScreen:
 *    Confirmation screen to trigger a reboot 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED

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
  buttonHandler =[this](int id, PressType type) -> void {
    Log.verbose(F("In RebootScreen ButtonHandler, id = %d"), id);
    if (id == ConfirmationButton && type > PressType::Normal) { ESP.restart(); }
    if (id == CancelButton) ScreenMgr.displayHomeScreen();
  };

  // Buttons are specified by the app when it is ready. See setButtons()
  buttonMappings = &confirmCancelMappings[0];
  nButtonMappings = 0;
}

void RebootScreen::display(bool) {
  // Log.verbose("RebootScreen::display, Display.deviceOptions.deviceType = %d", Display.deviceOptions->deviceType);
  if (Display.deviceOptions->deviceType == DisplayDeviceOptions::NONE) { ESP.restart(); }

  auto& oled = Display.oled;
  oled->clear();

  // Draw the top line...
  oled->setTextAlignment(TEXT_ALIGN_LEFT);
  uint16_t x = 0;

  String text = "Long press ";
  Display.setFont(Display.FontID::S10);
  oled->drawString(x, 2, text);
  x += oled->getStringWidth(text);

  text = "Advance";
  Display.setFont(Display.FontID::SB12);
  oled->drawString(x, 0, text);
  x += oled->getStringWidth(text);

  text = " to";
  Display.setFont(Display.FontID::S10);
  oled->drawString(x, 2, text);

  // Draw the center (Reboot)
  oled->setColor(OLEDDISPLAY_COLOR::WHITE);
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  Display.setFont(Display.FontID::S24);
  oled->drawString(Display.XCenter, 14, "Reboot");

  // Draw the bottom line
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  Display.setFont(Display.FontID::S10);
  oled->drawString(Display.XCenter, 49, "Cancel: Press Previous");

  oled->display();
  
  autoCancelTime = millis() + 60 * 1000L; // If nothing has happened in a minute, cancel
}

void RebootScreen::processPeriodicActivity() {
  if (millis() >= autoCancelTime) ScreenMgr.displayHomeScreen();
}

void RebootScreen::setButtons(uint8_t confirmPin, uint8_t cancelPin) {
  nButtonMappings = 0;
  if (confirmPin != 255) confirmCancelMappings[nButtonMappings++] = {confirmPin, ConfirmationButton};
  if (cancelPin != 255) confirmCancelMappings[nButtonMappings++] = {cancelPin, CancelButton};
}

#endif

