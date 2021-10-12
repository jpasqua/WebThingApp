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
#include "../../CurrencyMonApp.h"
//--------------- End:    Includes ---------------------------------------------


class HomeScreen : public Screen {
public:
  HomeScreen();
  virtual void display(bool) override;
  virtual void processPeriodicActivity() override;

private:
  void drawCurrencyNames();
  void drawCurrencies();

  uint16_t compositeTime = 0;
  Label *labels = nullptr;
};

#endif  // HomeScreen_h
