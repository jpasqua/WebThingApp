/*
 * EnterNumberScreen:
 *    Allow the user to enter a numeric value using a numberpad
 *                    
 * NOTES:
 * o The value of the numberpad is always provided as a float even if
 *   decimalsAllowed is false (see below)
 * o The numberpad can be configured using parameters to the init function:
 *   + decimalsAllowed: If true, users will be allowed to enter floats, otherwise
 *     they will only be able to enter whole numbers
 *   + minAllowed: The minimum acceptable value. If this is less than 0, a '+/-'
 *     button will be provided to change the sign of the entered value
 *   + maxAllowed: The maximum acceptable value
 */

#ifndef EnterNumberScreen_h
#define EnterNumberScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <float.h>
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../gui/Screen.h"
//--------------- End:    Includes ---------------------------------------------


class EnterNumberScreen : public Screen {
public:
  EnterNumberScreen();
  void display(bool activating = false);
  virtual void processPeriodicActivity();

  void init(
      const String &theTitle, float initialValue, WTBasics::FloatValCB cb, bool decimalsAllowed = false,
      float minAllowed = 0.0, float maxAllowed = FLT_MAX);

private:
  String title;
  float minVal, maxVal;
  float _initialValue;
  bool allowDecimals;
  String formattedValue;
  WTBasics::FloatValCB newValueCB;
};

#endif  // EnterNumberScreen_h







