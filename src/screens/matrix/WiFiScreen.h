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

    if (w >= 64) {
      drawFauxStrengthIcon(0, h-1);
      Display.drawStringInRegion(
        "WiFi", Display.BuiltInFont_ID, Display.MC_Align,
        0, h, w, h, w/2, h/2, 
        Theme::Color_WHITE, Theme::Color_BLACK);
    } else {
      Display.drawStringInRegion(
        "WiFi", Display.BuiltInFont_ID, Display.ML_Align,
        0, h, w, h, 0, h/2, 
        Theme::Color_WHITE, Theme::Color_BLACK);
    }
    drawFauxStrengthIcon(w-8, h-1);
    Display.mtx->write();
  }

  virtual void processPeriodicActivity() { }

  void drawFauxStrengthIcon(uint16_t x, uint16_t y) {
    for (int bar = 1; bar < 5; bar++) {
      int barSize = bar*2;
      Display.mtx->drawFastVLine(x++, y-barSize, barSize, 1);
      x++; // Skip a column
    }
  }
};

#endif