/*
 * RebootScreen:
 *    Confirmation screen to trigger a reboot 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <TimeLib.h>
//                                  WebThing Includes
//                                  Local Includes
#include "../../gui/Display.h"
#include "../../gui/Theme.h"
#include "../../gui/ScreenMgr.h"
#include "RebootScreen.h"
// #include "images/CancelBitmap.h"
// #include "images/RebootBitmap.h"
//--------------- End:    Includes ---------------------------------------------


static constexpr uint16_t IconInset = 10;
static constexpr uint8_t RebootLabel = 0;
static constexpr uint8_t CancelLabel = 1;

RebootScreen::RebootScreen() {
    // buttonHandler =[&](int id, PressType type) -> void {
    //   Log.verbose(F("In RebootScreen ButtonHandler, id = %d"), id);
    //   if (id == RebootLabel && type > PressType::Normal) { ESP.restart(); }
    //   if (id == CancelLabel) ScreenMgr.displayHomeScreen();
    // };

    // labels = new Label[(nLabels = 2)];
    // labels[0].init(0, 0,                Display.Width, Display.Height/2, RebootLabel);
    // labels[1].init(0, Display.Height/2, Display.Width, Display.Height/2, CancelLabel);
  }

void RebootScreen::display(bool) {
  auto& oled = Display.oled;

  oled->clear();
  oled->setColor(OLEDDISPLAY_COLOR::WHITE);
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  Display.setFont(Display.FontID::S24);
  oled->drawString(64, 0, "RebootScreen");
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  Display.setFont(Display.FontID::S16);
  oled->drawString(64, 36, "Implement Me!");
  oled->display();
  
  // tft.fillScreen(Theme::Color_Background);

  // Display.setFont(Display.FontID::SB12);
  // tft.setTextDatum(MC_DATUM);

  // uint16_t xc = (Display.Width + (IconInset+RebootIcon_Width))/2;
  // uint16_t yc = (Display.Height)/4;
  // tft.drawRect(
  //     labels[RebootLabel].region.x, labels[RebootLabel].region.y,
  //     labels[RebootLabel].region.w, labels[RebootLabel].region.h, Theme::Color_AlertError);
  // tft.drawBitmap(
  //     IconInset, (Display.Height/2-RebootIcon_Height)/2, RebootIcon,
  //     RebootIcon_Width, RebootIcon_Height,
  //     Theme::Color_AlertError);
  // tft.setTextColor(Theme::Color_AlertError);
  // tft.drawString(F("Reboot"), xc, yc);

  // xc = (Display.Width + (IconInset+CancelIcon_Width))/2;
  // yc = (Display.Height*3)/4;
  // tft.drawRect(
  //     labels[CancelLabel].region.x, labels[CancelLabel].region.y,
  //     labels[CancelLabel].region.w, labels[CancelLabel].region.h, Theme::Color_AlertGood);
  // tft.drawBitmap(
  //     IconInset, yc-(CancelIcon_Height/2), CancelIcon,
  //     CancelIcon_Width, CancelIcon_Height,
  //     Theme::Color_AlertGood);
  // tft.setTextColor(Theme::Color_AlertGood);
  // tft.drawString(F("Cancel"), xc, yc);
  // autoCancelTime = millis() + 60 * 1000L; // If nothing has happened in a minute, cancel
}

void RebootScreen::processPeriodicActivity() {
  // if (millis() >= autoCancelTime) ScreenMgr.displayHomeScreen();
}

#endif

