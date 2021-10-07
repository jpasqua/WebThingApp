/*
 * CalibrationScreen:
 *    Display a notification that the device is connecting to WiFi
 *
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_TOUCH
#ifndef WiFiScreen_h
#define WiFiScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../gui/Screen.h"
//--------------- End:    Includes ---------------------------------------------

class WiFiScreen : public Screen {
public:
  WiFiScreen();
  void display(bool activating = false);
  virtual void processPeriodicActivity();
};

#endif  // WiFiScreen_h
#endif