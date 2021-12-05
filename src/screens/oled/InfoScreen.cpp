#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
//                                  Local Includes
#include "../../WTApp.h"
#include "InfoScreen.h"
#include "../../gui/Display.h"
#include "../../gui/Theme.h"
#include "../../gui/ScreenMgr.h"
//--------------- End:    Includes ---------------------------------------------


void InfoScreen::display(bool) {
  auto& oled = Display.oled;
  uint8_t BufSize = 32;
  char fmtBuf[BufSize];

  oled->clear();
  Display.setFont(Display.FontID::SB12);
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  snprintf(fmtBuf, BufSize, "%s", wtApp->appName.c_str());
  oled->drawString(Display.XCenter, 4, fmtBuf);

  Display.setFont(Display.FontID::M5);
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  snprintf(fmtBuf, BufSize, "v%s", wtApp->appVersion.c_str());
  oled->drawString(Display.XCenter, 21, fmtBuf);

  Display.setFont(Display.FontID::S16);
  snprintf(fmtBuf, BufSize, "%s", WebThing::settings.hostname.c_str());
  oled->drawString(Display.XCenter, 34, fmtBuf);

  Display.setFont(Display.FontID::S10);
  snprintf(fmtBuf, BufSize, "%s", WebThing::ipAddrAsString().c_str());
  oled->drawString(Display.XCenter, 49, fmtBuf);

  drawRssi(112, 62);

  oled->display();
}

void InfoScreen::drawRssi(uint16_t x, uint16_t y) {
  int8_t quality = WebThing::wifiQualityAsPct();
  for (int8_t i = 0; i < 4; i++) {
    for (int8_t j = 0; j < 3 * (i + 2); j++) {
      if (quality > i * 25 || j == 0) {
        Display.oled->setPixel(x + 4 * i, y - j);
      }
    }
  }
}  

#endif