/*
 * ConfigScreen:
 *    Displays a screen with instructions when the device is going through
 *    an initil WiFi setup.
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../gui/Display.h"
#include "../../gui/Theme.h"
#include "ConfigScreen.h"
#include "images/GearBitmap.h"
//--------------- End:    Includes ---------------------------------------------


ConfigScreen::ConfigScreen() {
  nLabels = 0;
  labels = NULL;
}

void ConfigScreen::display(bool) {
  auto& oled = Display.oled;
  
  oled->clear();
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  Display.setFont(Display.FontID::S16);
  oled->drawString(64, 0, "Connect to WIFi:");
  oled->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  Display.setFont(Display.FontID::S24);
  // TEXT_ALIGN_CENTER_BOTH centers on the overall font height. For this, I want to
  // center on the baseline, hence I use 36 as the center instead of 32 (=64/2+4)
  oled->drawString(64, 36, _ssid);
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  Display.setFont(Display.FontID::S10);
  oled->drawString(64, 52, "then choose your network");
  oled->display();
}

void ConfigScreen::processPeriodicActivity() {
  // Nothing to do here, we're displaying a static screen
}

void ConfigScreen::setSSID(const String& ssid) { _ssid = ssid; }

#endif