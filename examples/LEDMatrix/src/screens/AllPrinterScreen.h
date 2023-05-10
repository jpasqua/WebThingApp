/*
 * AllPrinterScreen:
 *    Display the print status of the next printer that will complete 
 *                    
 */

#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef AllPrinterScreen_h
#define AllPrinterScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThingApp Includes
#include <screens/matrix/ScrollScreen.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class AllPrinterScreen : public ScrollScreen {
public:
  AllPrinterScreen();
  virtual void innerActivation() override;
  virtual bool innerPeriodic() override;

private:
  static constexpr uint32_t UpdateInterval = 30 * 1000L;
  String    _statusText;
  uint32_t  _nextTimeToUpdate = 0;

  void updateText();

};

#endif  // AllPrinterScreen_h
#endif