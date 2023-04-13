#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
//                                  Local Includes
#include "../../WTApp.h"
#include "InfoScreen.h"
//--------------- End:    Includes ---------------------------------------------


InfoScreen::InfoScreen() {
  init();
}

void InfoScreen::innerActivation() {
  String info;

  info += wtApp->appName;
  info += ' ';
  info += wtApp->appVersion;
  info += ' ';
  info += WebThing::settings.hostname;
  info += " (";
  info += WebThing::ipAddrAsString();
  info += ") ";
  info += "WiFi ";
  info += WebThing::wifiQualityAsPct();
  info += '%';

  setText(info);
}

#endif