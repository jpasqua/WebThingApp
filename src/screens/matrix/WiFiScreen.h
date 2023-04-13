#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
//                                  WebThingApp Includes
#include "../../gui/Screen.h"
//--------------- End:    Includes ---------------------------------------------


class WiFiScreen : public Screen {
public:

  virtual void display(bool) override {
    Log.verbose("WiFiScreen::display");
    uint16_t w = Display.mtx->width();
    uint16_t h = Display.mtx->height();

    Display.drawStringInRegion(
      "Wifi", Display.BuiltInFont_ID, Display.MC_Align,
      0, h, w, h, w/2, h/2, 
      Theme::Color_WHITE, Theme::Color_BLACK);
  }

  virtual void processPeriodicActivity() { }
};

#endif