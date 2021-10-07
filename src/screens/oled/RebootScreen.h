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
  void display(bool activating = false);
  virtual void processPeriodicActivity();

private:
  uint32_t autoCancelTime = UINT32_MAX;
};

#endif // RebootScreen_h
#endif