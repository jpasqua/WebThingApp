/*
 * UtilityScreen:
 *    Display info about the GrillMon including things like the server name,
 *    wifi address, heap stats, etc. Also allow brightness adjustment 
 *                    
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <FS.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <TimeLib.h>
//                                  WebThing Includes
#include <WebThing.h>
//                                  Local Includes
#include "../WTAppImpl.h"
#include "../plugins/PluginMgr.h"
#include "../gui/Theme.h"
#include "../gui/ScreenMgr.h"
#include "../gui/Display.h"
#include "UtilityScreen.h"
//--------------- End:    Includes ---------------------------------------------

using Display::tft;

static const char *pis_label[6] = {"Dim", "Medium", "Bright", "Refresh", "Calibrate", "Home"};
static const uint16_t pis_colors[6] = {
  Theme::Color_Inactive,   Theme::Color_DimText,    Theme::Color_NormalText,
  Theme::Color_NormalText, Theme::Color_NormalText, Theme::Color_AlertGood};

static constexpr int16_t SubFont = 2; // A small 5x7 font

static constexpr auto HeaderFont = Display::Font::FontID::SB12;
static constexpr uint16_t HeaderFontHeight = 29;

static constexpr auto ButtonFont = Display::Font::FontID::SB9;
static constexpr uint16_t ButtonFontHeight = 22;

static constexpr uint16_t ButtonFrameSize = 2;
static constexpr uint16_t ButtonHeight = 40;
static constexpr uint16_t ButtonInset = 1;
static constexpr uint16_t HalfWidth = (Display::Width-(2*ButtonInset))/2;
static constexpr uint16_t ThirdWidth = (Display::Width-(2*ButtonInset))/3;

static constexpr uint16_t PI_YOrigin = 60;

static constexpr uint8_t FirstPluginIndex = 0;
static constexpr uint8_t MaxPlugins = 4;
static constexpr uint8_t DimButtonIndex = 4;
static constexpr uint8_t MediumButtonIndex = 5;
static constexpr uint8_t BrightButtonIndex = 6;
static constexpr uint8_t RefreshButtonIndex = 7;
static constexpr uint8_t CalButtonIndex = 8;
static constexpr uint8_t HomeButtonIndex = 9;
static constexpr uint8_t TotalButtons = 10;

static constexpr uint16_t WifiBarsWidth = 13;
static constexpr uint16_t WifiBarsHeight = 16;

UtilityScreen::UtilityScreen() {

  auto buttonHandler =[&](int id, Button::PressType type) -> void {
    Log.verbose(F("In UtilityScreen Button Handler, id = %d, type = %d"), id, type);

    if (id < MaxPlugins) {
      wtAppImpl->pluginMgr.displayPlugin(id);
      return;
    }

    if (id <= BrightButtonIndex) {
      Display::setBrightness(id == DimButtonIndex ? 20 : (id == MediumButtonIndex ? 50 : 90));
      return;
    }

    if (id == RefreshButtonIndex) { wtAppImpl->updateAllData(); return;} 
    if (id == CalButtonIndex) { ScreenMgr.display(wtAppImpl->calibrationScreen); return; }
    if (id == HomeButtonIndex) { ScreenMgr.displayHomeScreen(); return; }
  };


  buttons = new Button[(nButtons = TotalButtons)];
  for (int i = 0; i < MaxPlugins; i++) {
    buttons[i].init(
      ButtonInset + ((i%2) * HalfWidth), PI_YOrigin + (i/2) * ButtonHeight, HalfWidth, ButtonHeight, buttonHandler, i);        
  }

  int x = ButtonInset;
  buttons[DimButtonIndex].init(
      x, 160, ThirdWidth, ButtonHeight, buttonHandler, DimButtonIndex);     x+= ThirdWidth;
  buttons[MediumButtonIndex].init(
      x, 160, ThirdWidth, ButtonHeight, buttonHandler, MediumButtonIndex);  x+= ThirdWidth;
  buttons[BrightButtonIndex].init(
      x, 160, ThirdWidth, ButtonHeight, buttonHandler, BrightButtonIndex);
  x = ButtonInset;
  buttons[RefreshButtonIndex].init(
      x, 200, ThirdWidth, ButtonHeight, buttonHandler, RefreshButtonIndex); x+= ThirdWidth;
  buttons[CalButtonIndex].init(
      x, 200, ThirdWidth, ButtonHeight, buttonHandler, CalButtonIndex);     x+= ThirdWidth;
  buttons[HomeButtonIndex].init(
      x, 200, ThirdWidth, ButtonHeight, buttonHandler, HomeButtonIndex);
}

void UtilityScreen::display(bool activating) {
  if (activating) tft.fillScreen(Theme::Color_Background);

  int y = 0;
  tft.setTextColor(Theme::Color_AlertGood);
  Display::Font::setUsingID(HeaderFont, tft);
  tft.setTextDatum(TC_DATUM);
  String appInfo = wtApp->appName + " v" + wtApp->appVersion;
  tft.drawString(appInfo, Display::XCenter, 0);
  drawWifiStrength(Display::Width-WifiBarsWidth-3, ButtonHeight-12, Theme::Color_NormalText);
  y += HeaderFontHeight;

  Display::Font::setUsingID(ButtonFont, tft);
  tft.setTextColor(Theme::Color_NormalText);
  tft.setTextDatum(TC_DATUM);
  String address = WebThing::settings.hostname + " (" + WebThing::ipAddrAsString() + ")";
  tft.drawString(address, Display::XCenter, y);

  tft.setTextColor(Theme::Color_NormalText);
  tft.setTextDatum(BC_DATUM);
  tft.drawString(_subHeading + _subContent, Display::XCenter, PI_YOrigin-1, SubFont);

  String name;
  uint16_t textColor = Theme::Color_NormalText;
  uint8_t nPlugins = min(wtAppImpl->pluginMgr.getPluginCount(), MaxPlugins);

  for (int i = 0; i < TotalButtons; i++) {
    if (i < nPlugins) {
      Plugin *p = wtAppImpl->pluginMgr.getPlugin(i);
      if (!p->enabled()) textColor = Theme::Color_Inactive;
      name = p->getName();
      textColor = Theme::Color_WiFiBlue;
    } else if (i < MaxPlugins) {
      name = "Unused";
      textColor = Theme::Color_Inactive;
    } else {
      name = pis_label[i-MaxPlugins];
      textColor = pis_colors[i-MaxPlugins];
    }
    drawButton(name, i, textColor, activating);
  }
}

void UtilityScreen::processPeriodicActivity() {  }

void UtilityScreen::drawButton(String label, int i, uint16_t textColor, bool clear) {
  if (clear) buttons[i].clear(Theme::Color_Background);
  buttons[i].drawSimple(
      label, ButtonFont, ButtonFrameSize,
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
    tft.drawRect(x+(i*4), y-h+1, 1, h, color);
  }
}