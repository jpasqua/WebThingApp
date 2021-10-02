/*
 * ConfigScreen:
 *    Displays a screen with instructions when the device is going through
 *    an initil WiFi setup.
 *                    
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../gui/Display.h"
#include "../gui/Theme.h"
#include "ConfigScreen.h"
#include "images/Gears160x240.h"
//--------------- End:    Includes ---------------------------------------------


ConfigScreen::ConfigScreen() {
  nLabels = 0;
  labels = NULL;
}

void ConfigScreen::display(bool) {
  auto& tft = Display.tft;
  
  tft.fillScreen(Theme::Color_Background);
  tft.drawBitmap(0, 0, Gears160x240, 160, 240, Theme::Color_AlertGood);

  Display.fonts.setUsingID(Display.fonts.FontID::S12, tft);
  tft.setTextSize(1);
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(Theme::Color_NormalText);
  tft.drawString(F("Initial"),    160+(160/2), 50);
  tft.drawString(F("Setup:"),     160+(160/2), 72);
  tft.setTextColor(Theme::Color_AlertGood);
  tft.drawString(F("Connect to"), 160+(160/2), 100);
  tft.drawString(F("WiFi named"), 160+(160/2), 122);
  tft.setTextColor(Theme::Color_NormalText);
  tft.drawString(_ssid,        160+(160/2), 156);
  tft.setTextDatum(TL_DATUM);
}

void ConfigScreen::processPeriodicActivity() {
  // Nothing to do here, we're displaying a static screen
}

void ConfigScreen::setSSID(const String& ssid) { _ssid = ssid; }
