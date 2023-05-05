/*
 * ConfigScreen:
 *    Displays a screen with instructions when the device is going through
 *    an initil WiFi setup.
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef ConfigScreen_h
#define ConfigScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "ScrollScreen.h"
//--------------- End:    Includes ---------------------------------------------

class ConfigScreen : public ScrollScreen {
public:
  ConfigScreen();
  virtual void innerActivation() { }

  void setSSID(const String& ssid);

private:
};

#endif  // ConfigScreen_h
#endif