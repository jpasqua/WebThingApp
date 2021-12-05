/*
 * UtilityScreen:
 *    Display info about the GrillMon including things like the server name,
 *    wifi address, heap stats, etc. Also allow brightness adjustment 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_TOUCH

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <FS.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <TimeLib.h>
//                                  WebThing Includes
#include <WebThing.h>
//                                  Local Includes
#include "../../WTAppImpl.h"
#include "../../plugins/PluginMgr.h"
#include "../../gui/Theme.h"
#include "../../gui/ScreenMgr.h"
#include "../../gui/Display.h"
#include "UtilityScreen.h"
//--------------- End:    Includes ---------------------------------------------


static const char *pis_label[6] = {"Dim", "Medium", "Bright", "Refresh", "Calibrate", "Home"};
static const uint16_t pis_colors[6] = {
  Theme::Color_Inactive,   Theme::Color_DimText,    Theme::Color_NormalText,
  Theme::Color_NormalText, Theme::Color_NormalText, Theme::Color_AlertGood};

static constexpr int16_t SubFont = 2; // A small 5x7 font

static constexpr auto HeaderFont = Display.FontID::SB12;
static constexpr uint16_t HeaderFontHeight = 29;

static constexpr auto LabelFont = Display.FontID::SB9;
static constexpr uint16_t LabelFontHeight = 22;

static constexpr uint16_t LabelFrameSize = 2;
static constexpr uint16_t LabelHeight = 40;
static constexpr uint16_t LabelInset = 1;
static constexpr uint16_t HalfWidth = (Display.Width-(2*LabelInset))/2;
static constexpr uint16_t ThirdWidth = (Display.Width-(2*LabelInset))/3;

static constexpr uint16_t PI_YOrigin = 60;

static constexpr uint8_t FirstPluginLabel = 0;
static constexpr uint8_t NPluginSlots = 4;
static constexpr uint8_t DimLabel = FirstPluginLabel + NPluginSlots;
static constexpr uint8_t MediumLabel = 5;
static constexpr uint8_t BrightLabel = 6;
static constexpr uint8_t RefreshLabel = 7;
static constexpr uint8_t CalLabel = 8;
static constexpr uint8_t HomeLabel = 9;
static constexpr uint8_t N_Labels = 10;

static constexpr uint16_t WifiBarsWidth = 13;
static constexpr uint16_t WifiBarsHeight = 16;

UtilityScreen::UtilityScreen() {

  buttonHandler =[&](int id, PressType type) -> void {
    Log.verbose(F("In UtilityScreen Label Handler, id = %d, type = %d"), id, type);

    if (id < NPluginSlots) {
      wtAppImpl->pluginMgr.displayPlugin(id);
      return;
    }

    if (id <= BrightLabel) {
      Display.setBrightness(id == DimLabel ? 20 : (id == MediumLabel ? 50 : 90));
      return;
    }

    if (id == RefreshLabel) { wtAppImpl->updateAllData(); return;} 
    if (id == CalLabel) { ScreenMgr.display(wtAppImpl->screens.calibrationScreen); return; }
    if (id == HomeLabel) { ScreenMgr.displayHomeScreen(); return; }
  };


  labels = new Label[(nLabels = N_Labels)];
  for (int i = 0; i < NPluginSlots; i++) {
    labels[i].init(
      LabelInset + ((i%2) * HalfWidth), PI_YOrigin + (i/2) * LabelHeight, HalfWidth, LabelHeight, i);        
  }

  int x = LabelInset;
  labels[DimLabel].init(
      x, 160, ThirdWidth, LabelHeight, DimLabel);     x+= ThirdWidth;
  labels[MediumLabel].init(
      x, 160, ThirdWidth, LabelHeight, MediumLabel);  x+= ThirdWidth;
  labels[BrightLabel].init(
      x, 160, ThirdWidth, LabelHeight, BrightLabel);
  x = LabelInset;
  labels[RefreshLabel].init(
      x, 200, ThirdWidth, LabelHeight, RefreshLabel); x+= ThirdWidth;
  labels[CalLabel].init(
      x, 200, ThirdWidth, LabelHeight, CalLabel);     x+= ThirdWidth;
  labels[HomeLabel].init(
      x, 200, ThirdWidth, LabelHeight, HomeLabel);
}

void UtilityScreen::display(bool activating) {
  auto& tft = Display.tft;
  
  if (activating) tft.fillScreen(Theme::Color_Background);

  int y = 0;
  tft.setTextColor(Theme::Color_AlertGood);
  Display.setFont(HeaderFont);
  tft.setTextDatum(TC_DATUM);
  String appInfo = wtApp->appName + " v" + wtApp->appVersion;
  tft.drawString(appInfo, Display.XCenter, 0);
  drawWifiStrength(Display.Width-WifiBarsWidth-3, LabelHeight-12, Theme::Color_NormalText);
  y += HeaderFontHeight;

  Display.setFont(LabelFont);
  tft.setTextColor(Theme::Color_NormalText);
  tft.setTextDatum(TC_DATUM);
  String address = WebThing::settings.hostname + " (" + WebThing::ipAddrAsString() + ")";
  tft.drawString(address, Display.XCenter, y);

  tft.setTextColor(Theme::Color_NormalText);
  tft.setTextDatum(BC_DATUM);
  tft.drawString(_subHeading + _subContent, Display.XCenter, PI_YOrigin-1, SubFont);

  String name;
  uint16_t textColor = Theme::Color_NormalText;
  uint8_t nPlugins = min(wtAppImpl->pluginMgr.getPluginCount(), NPluginSlots);

  for (int i = 0; i < N_Labels; i++) {
    if (i < nPlugins) {
      Plugin *p = wtAppImpl->pluginMgr.getPlugin(i);
      if (!p->enabled()) textColor = Theme::Color_Inactive;
      name = p->getName();
      textColor = Theme::Color_WiFiBlue;
    } else if (i < NPluginSlots) {
      name = "Unused";
      textColor = Theme::Color_Inactive;
    } else {
      name = pis_label[i-NPluginSlots];
      textColor = pis_colors[i-NPluginSlots];
    }
    drawLabel(name, i, textColor, activating);
  }
}

void UtilityScreen::processPeriodicActivity() {  }

void UtilityScreen::drawLabel(String label, int i, uint16_t textColor, bool clear) {
  if (clear) labels[i].clear(Theme::Color_Background);
  labels[i].drawSimple(
      label, LabelFont, LabelFrameSize,
      textColor, Theme::Color_Border, Theme::Color_Background);
}

void UtilityScreen::setSub(const String& heading, const String& content) {
  _subHeading = heading;
  _subContent = content;
}

// (x, y) represents the bottom left corner of the wifi strength bars
void UtilityScreen::drawWifiStrength(uint16_t x, uint16_t y, uint32_t color) {
  int8_t quality = WebThing::wifiQualityAsPct();
  for (int i = 0; i < 4; i++) {
    int h = (quality > (25 * i)) ? 4*(i+1) : 1;
    Display.tft.drawRect(x+(i*4), y-h+1, 1, h, color);
  }
}
#endif