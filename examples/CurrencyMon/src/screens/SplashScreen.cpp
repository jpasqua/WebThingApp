//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <gui/Display.h>
#include <gui/Theme.h>
//                                  Local Includes
#include "SplashScreen.h"
#include "AppTheme.h"
#include "images/CMLogoMono.h"
//--------------- End:    Includes ---------------------------------------------


SplashScreen::SplashScreen() {
  nButtons = 0;
  buttons = NULL;
}

void SplashScreen::display(bool) {
  auto& tft = Display.tft;

  tft.fillScreen(AppTheme::Color_SplashBkg);

  constexpr uint16_t x = Display.XCenter - (CMLogoMono_Width/2);
  constexpr uint16_t y = 25;

  tft.drawBitmap(x, y, CMLogoMono, CMLogoMono_Width, CMLogoMono_Height, AppTheme::Color_SplashFG);

  Display.fonts.setUsingID(Display.fonts.FontID::SBO24, tft);
  tft.setTextColor(AppTheme::Color_SplashText);
  tft.setTextDatum(BC_DATUM);
  tft.drawString(F("CurrencyMon"), Display.XCenter, Display.Height-1);
}

void SplashScreen::processPeriodicActivity() {
  // Nothing to do here, we're displaying a static screen
}
