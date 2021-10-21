/*
 * ForecastScreen:
 *    Display forecast info for the selected city
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED
#ifndef ForecastScreen_h
#define ForecastScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include "../../gui/Screen.h"
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class ForecastScreen : public Screen {
public:
  ForecastScreen(uint8_t start) : startingDay(start) {};

  void display(bool force = false) override;
  virtual void processPeriodicActivity() override;

protected:
  uint8_t startingDay = 0;    // 0 == today

private:
  void drawForecastDetails(Forecast* forecast, int xOff);
};

class ForecastFirst3 : public ForecastScreen {
public:
  ForecastFirst3() : ForecastScreen(0) {}
};

class ForecastLast2 : public ForecastScreen {
public:
  ForecastLast2() : ForecastScreen(3) {}
};

#endif  // ForecastScreen_h
#endif