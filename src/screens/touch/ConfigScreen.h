#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_TOUCH

/*
 * ConfigScreen:
 *    Displays a screen with instructions when the device is going through
 *    an initil WiFi setup.
 *                    
 */

#ifndef ConfigScreen_h
#define ConfigScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include "../../gui/Screen.h"
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class ConfigScreen : public Screen {
public:
  ConfigScreen();
  void display(bool activating = false);
  virtual void processPeriodicActivity();

  void setSSID(const String& ssid);

private:
  String _ssid = "app-NNNNN";
};

#endif  // ConfigScreen_h
#endif