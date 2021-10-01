/*
 * RebootScreen:
 *    Confirmation screen to trigger a reboot 
 *                    
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <TimeLib.h>
//                                  WebThing Includes
//                                  Local Includes
#include "../gui/Display.h"
#include "../gui/Theme.h"
#include "../gui/ScreenMgr.h"
#include "RebootScreen.h"
#include "images/CancelBitmap.h"
#include "images/RebootBitmap.h"
//--------------- End:    Includes ---------------------------------------------


static constexpr uint16_t IconInset = 10;
static constexpr uint8_t RebootButtonID = 0;
static constexpr uint8_t CancelButtonID = 1;

RebootScreen::RebootScreen() {
    auto buttonHandler =[&](int id, Label::PressType type) -> void {
      Log.verbose(F("In RebootScreen ButtonHandler, id = %d"), id);
      if (id == RebootButtonID && type > Label::PressType::NormalPress) { ESP.restart(); }
      if (id == CancelButtonID) ScreenMgr.displayHomeScreen();
    };

    buttons = new Label[(nButtons = 2)];
    buttons[0].init(0, 0,                Display.Width, Display.Height/2, buttonHandler, RebootButtonID);
    buttons[1].init(0, Display.Height/2, Display.Width, Display.Height/2, buttonHandler, CancelButtonID);
  }

void RebootScreen::display(bool) {
  auto& tft = Display.tft;
  
  tft.fillScreen(Theme::Color_Background);

  Display.fonts.setUsingID(Display.fonts.FontID::SB12, tft);
  tft.setTextDatum(MC_DATUM);

  uint16_t xc = (Display.Width + (IconInset+RebootIcon_Width))/2;
  uint16_t yc = (Display.Height)/4;
  tft.drawRect(
      buttons[RebootButtonID].region.x, buttons[RebootButtonID].region.y,
      buttons[RebootButtonID].region.w, buttons[RebootButtonID].region.h, Theme::Color_AlertError);
  tft.drawBitmap(
      IconInset, (Display.Height/2-RebootIcon_Height)/2, RebootIcon,
      RebootIcon_Width, RebootIcon_Height,
      Theme::Color_AlertError);
  tft.setTextColor(Theme::Color_AlertError);
  tft.drawString(F("Reboot"), xc, yc);

  xc = (Display.Width + (IconInset+CancelIcon_Width))/2;
  yc = (Display.Height*3)/4;
  tft.drawRect(
      buttons[CancelButtonID].region.x, buttons[CancelButtonID].region.y,
      buttons[CancelButtonID].region.w, buttons[CancelButtonID].region.h, Theme::Color_AlertGood);
  tft.drawBitmap(
      IconInset, yc-(CancelIcon_Height/2), CancelIcon,
      CancelIcon_Width, CancelIcon_Height,
      Theme::Color_AlertGood);
  tft.setTextColor(Theme::Color_AlertGood);
  tft.drawString(F("Cancel"), xc, yc);
  autoCancelTime = millis() + 60 * 1000L; // If nothing has happened in a minute, cancel
}

void RebootScreen::processPeriodicActivity() {
  if (millis() >= autoCancelTime) ScreenMgr.displayHomeScreen();
}



