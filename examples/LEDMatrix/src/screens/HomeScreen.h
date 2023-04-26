#ifndef HomeScreen_h
#define HomeScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <TimeLib.h>
//                                  WebThingApp
#include <gui/Display.h>
//                                  Local Includes
#include "../../LEDMatrixApp.h"
//--------------- End:    Includes ---------------------------------------------


class HomeScreen : public Screen {
public:
  HomeScreen();
  virtual void display(bool) override;
  virtual void processPeriodicActivity() override;

private:
  uint32_t _colonLastToggledAt = 0;
  uint32_t _displayStartedAt = 0;
  uint16_t _compositeTime = 0;
  bool _colonVisible = true;
  uint32_t _homeScreenDisplayTime = 10*1000L; // Should be a setting

  void toggleColon();
  void drawProgressBar();
};

#endif  // HomeScreen_h
