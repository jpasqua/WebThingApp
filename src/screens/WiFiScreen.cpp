/*
 * CalibrationScreen:
 *    Display a notification that the device is connecting to WiFi
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../gui/Display.h"
#include "../gui/Theme.h"
#include "WiFiScreen.h"
#include "images/WiFiLogo.h"
//--------------- End:    Includes ---------------------------------------------


auto& tft = Display.tft;

WiFiScreen::WiFiScreen() {
    nLabels = 0;
    labels = nullptr;
  }

void WiFiScreen::display(bool) {
  tft.fillScreen(Theme::Color_Background);
  uint16_t x = (Display.Width-WiFiLogo_Width)/2;
  uint16_t y = 30;  // A little space from the top of the screen
#ifdef WiFiLogo_UseMono
  tft.drawBitmap(x, y, WiFiLogoMono, WiFiLogo_Width, WiFiLogo_Height, Theme::Color_WiFiBlue);
#else   // WiFiLogo_UseMono
  tft.pushImage(x, y, WiFiLogo_Width, WiFiLogo_Height, WiFiLogo, WiFiLogo_Transparent);
#endif  // WiFiLogo_UseMono  
  y += WiFiLogo_Height;

  Display.setFont(Display.FontID::SBO12);
  tft.setTextColor(Theme::Color_WiFiBlue);
  tft.setTextDatum(MC_DATUM);
  x = Display.XCenter;
  y += (Display.Height - y)/2;
  tft.drawString(F("Connecting..."), x, y);
}

void WiFiScreen::processPeriodicActivity() {
  // Nothing to do here, we're displaying a static screen
}
