/*
 * WeatherScreen:
 *    Display weather info for the selected city and a running clock. 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_TOUCH
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
  WeatherScreen();
  void display(bool force = false);
  virtual void processPeriodicActivity();

private:
  uint32_t lastDT = UINT32_MAX;
  uint32_t lastClockUpdate = 0;

  void displaySingleWeather(int index);
  void showTime();
};

#endif  // WeatherScreen_h
#endif