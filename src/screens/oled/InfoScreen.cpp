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
  Display.setFont(Display.FontID::S16);
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  snprintf(fmtBuf, BufSize, "%s v%s", wtApp->appName.c_str(), wtApp->appVersion.c_str());
  oled->drawString(Display.XCenter, 5, fmtBuf);

  snprintf(fmtBuf, BufSize, "%s", WebThing::settings.hostname.c_str());
  oled->drawString(Display.XCenter, 29, fmtBuf);

  snprintf(fmtBuf, BufSize, "%s", WebThing::ipAddrAsString().c_str());
  oled->drawString(Display.XCenter, 42, fmtBuf);

  // snprintf(fmtBuf, BufSize, "Something else: %s", someString);
  // oled->drawString(0, 50, fmtBuf);
  
  drawRssi();

  oled->display();
}

void InfoScreen::drawRssi() {
  int8_t quality = WebThing::wifiQualityAsPct();
  for (int8_t i = 0; i < 4; i++) {
    for (int8_t j = 0; j < 3 * (i + 2); j++) {
      if (quality > i * 25 || j == 0) {
        Display.oled->setPixel(114 + 4 * i, 63 - j);
      }
    }
  }
}  

#endif