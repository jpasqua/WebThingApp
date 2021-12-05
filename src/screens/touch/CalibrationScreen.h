#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_TOUCH

/*
 * CalibrationScreen:
 *    Provides a way to calibrate the touch sensor on the screen
 *
 */

#ifndef CalibrationScreen_h
#define CalibrationScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../gui/Screen.h"
//--------------- End:    Includes ---------------------------------------------

class CalibrationScreen : public Screen {
public:
  CalibrationScreen();
  void display(bool activating = false) ;
  virtual void processPeriodicActivity();

private:
  enum {pre, post, complete} state;
  CalibrationData newCalibrationData;

  void init();
};

#endif  // CalibrationScreen_h
#endif