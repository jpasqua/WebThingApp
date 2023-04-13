/*
 * WeatherScreen:
 *    Display weather info for the selected city and a running clock. 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef WeatherScreen_h
#define WeatherScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThingApp Includes
#include "../../gui/Screen.h"
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class WeatherScreen : public ScrollScreen {
public:
  static String getTextForIcon(String& icon);

  WeatherScreen();
  virtual void innerActivation() override;


private:
  uint32_t lastDT = UINT32_MAX;
};

#endif  // WeatherScreen_h
#endif