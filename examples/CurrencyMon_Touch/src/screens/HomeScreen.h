#ifndef HomeScreen_h
#define HomeScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <screens/touch/EnterNumberScreen.h>
#include <gui/Screen.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class HomeScreen : public Screen {
public:
  HomeScreen();

  void display(bool activating = false);

  virtual void processPeriodicActivity();

private:
  static constexpr uint8_t NumCurrencyButtons = 3;
  
  uint8_t activeCurrency = 0;
  uint32_t nextUpdateTime = UINT32_MAX;
  float displayedValues[NumCurrencyButtons];

  void drawClock(bool force = false);
  void drawWeather(bool force = false);
  void drawCurrencyButtons(bool force = false);
  void drawCurrencyLabels(bool force = false);
  void updateAmounts(float newBase);

  EnterNumberScreen* _numpad;
};

#endif  // HomeScreen_h