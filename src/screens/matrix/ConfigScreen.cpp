/*
 * ConfigScreen:
 *    Displays a screen with instructions when the device is going through
 *    an initil WiFi setup.
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../gui/Display.h"
#include "../../gui/Theme.h"
#include "ConfigScreen.h"
//--------------- End:    Includes ---------------------------------------------


ConfigScreen::ConfigScreen() {
  nLabels = 0;
  labels = NULL;

  init(false, 20, 2);
}

void ConfigScreen::setSSID(const String& ssid) {
  String text = "WiFi? Connect to: ";
  text += ssid;
  text += ", then choose your network";
  setText(text, Display.BuiltInFont_ID);
}

#endif