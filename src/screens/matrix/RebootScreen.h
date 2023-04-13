/*
 * RebootScreen:
 *    Confirmation screen to trigger a reboot 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef RebootScreen_h
#define RebootScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <BPABasics.h>
//                                  WebThing Includes
//                                  Local Includes
#include "ScrollScreen.h"
//--------------- End:    Includes ---------------------------------------------

class RebootScreen : public ScrollScreen {
public:
  RebootScreen();
  virtual void innerPeriodic() override;
  virtual void innerActivation() override;

  void setButtons(Basics::Pin confirmPin, Basics::Pin cancelPin = Basics::UnusedPin);

private:
  uint32_t autoCancelTime = UINT32_MAX;
  WTButton::Mapping confirmCancelMappings[2];

};

#endif // RebootScreen_h
#endif