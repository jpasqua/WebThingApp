/*
 * WeatherScreen:
 *    Display weather info for the selected city and a running clock. 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED
#ifndef WeatherScreen_h
#define WeatherScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include "../../gui/Screen.h"
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class WeatherScreen : public Screen {
public:
  static String getGlyphForIcon(String& icon);

  WeatherScreen();
  void display(bool force = false);
  virtual void processPeriodicActivity();


private:
  uint32_t lastDT = UINT32_MAX;
};

#endif  // WeatherScreen_h
#endif