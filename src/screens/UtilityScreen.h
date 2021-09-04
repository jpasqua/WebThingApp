/*
 * UtilityScreen:
 *    Display info about the GrillMon including things like the server name,
 *    wifi address, heap stats, etc. Also allow brightness adjustment 
 *
 */

#ifndef UtilityScreen_h
#define UtilityScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../gui/Screen.h"
//--------------- End:    Includes ---------------------------------------------


class UtilityScreen : public Screen {
public:
  typedef std::function<void(bool)> RefreshCallback;

  UtilityScreen();
  void display(bool activating = false);
  virtual void processPeriodicActivity();

  void setSub(const String& heading, const String& content);

private:
  PluginMgr* pluginMgr;
  String _subHeading;
  String _subContent;
  RefreshCallback _updateAllData;

  void drawButton(String label, int i, uint16_t textColor, bool clear = false);

  // (x, y) represents the bottom left corner of the wifi strength bars
  void drawWifiStrength(uint16_t x, uint16_t y, uint32_t color);

};

#endif  // #define UtilityScreen_h
