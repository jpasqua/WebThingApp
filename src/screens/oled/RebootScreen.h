/*
 * RebootScreen:
 *    Confirmation screen to trigger a reboot 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED
#ifndef RebootScreen_h
#define RebootScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../gui/Screen.h"
//--------------- End:    Includes ---------------------------------------------

class RebootScreen : public Screen {
public:
  RebootScreen();
  virtual void display(bool activating = false) override;
  virtual void processPeriodicActivity() override;

  void setButtons(uint8_t confirmPin, uint8_t cancelPin = 255);

private:
  uint32_t autoCancelTime = UINT32_MAX;
  WTButton::Mapping confirmCancelMappings[2];

};

#endif // RebootScreen_h
#endif