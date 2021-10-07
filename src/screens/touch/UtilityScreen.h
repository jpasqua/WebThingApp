/*
 * UtilityScreen:
 *    Display info about the GrillMon including things like the server name,
 *    wifi address, heap stats, etc. Also allow brightness adjustment 
 *
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_TOUCH
#ifndef UtilityScreen_h
#define UtilityScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../gui/Screen.h"
//--------------- End:    Includes ---------------------------------------------


class UtilityScreen : public Screen {
public:
  using RefreshCallback = std::function<void(bool)>;

  UtilityScreen();
  void display(bool activating = false);
  virtual void processPeriodicActivity();

  void setSub(const String& heading, const String& content);

private:
  PluginMgr* pluginMgr;
  String _subHeading;
  String _subContent;
  RefreshCallback _updateAllData;

  // (x, y) represents the bottom left corner of the wifi strength bars
  void drawWifiStrength(uint16_t x, uint16_t y, uint32_t color);

  void drawLabel(String label, int i, uint16_t textColor, bool clear);

};

#endif  // #define UtilityScreen_h
#endif