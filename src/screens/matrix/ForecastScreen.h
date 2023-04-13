/*
 * ForecastScreen:
 *    Display the forecast for the selected city 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef ForecastScreen_h
#define ForecastScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
//                                  Local Includes
#include "ScrollScreen.h"
//--------------- End:    Includes ---------------------------------------------

class ForecastScreen : public ScrollScreen {
public:
  ForecastScreen();
  virtual void innerActivation();

private:
  String _forecastText;
  uint32_t _timeOfLastForecast = 0;

  void appendForecastForDay(Forecast* forecast, String& appendTo);

};

#endif  // ForecastScreen_h
#endif