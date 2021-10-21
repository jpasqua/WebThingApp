/*
 * MMWebUI:
 *    Implements the WebUI for MultiMon
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <FS.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <BPABasics.h>
//                                  WebThing Includes
#include <DataBroker.h>
#include <WebThing.h>
#include <WebUI.h>
//                                  Local Includes
#include "WebUIHelper.h"
#include "WTAppImpl.h"
#include "gui/Theme.h"
#include "gui/Display.h"
#include "gui/ScreenMgr.h"
//--------------- End:    Includes ---------------------------------------------


// ----- BEGIN: WebUIHelper namespace
namespace WebUIHelper {
  constexpr char UpdatingSymbol = 'w';
  
  // ----- BEGIN: WebUIHelper::Internal
  namespace Internal {
    const __FlashStringHelper* CORE_MENU_ITEMS = FPSTR(
      "<a class='w3-bar-item w3-button' href='/updateStatus'>"
      "<i class='fa fa-recycle'></i> Update Status</a>"
      "<a class='w3-bar-item w3-button' href='/presentDisplayConfig'>"
      "<i class='fa fa-desktop'></i> Configure Display</a>"
      "<a class='w3-bar-item w3-button' href='/presentWeatherConfig'>"
      "<i class='fa fa-thermometer-three-quarters'></i> Configure Weather</a>"
      "<a class='w3-bar-item w3-button' href='/presentPluginConfig'>"
      "<i class='fa fa-plug'></i> Configure Plugins</a>");
  }

  // ----- END: WebUIHelper::Internal


  // ----- BEGIN: WebUIHelper::Endpoints
  namespace Endpoints {
    void setBrightness() {
      auto action = []() {
        uint8_t b = WebUI::arg(F("value")).toInt();
        if (b <= 0 || b > 100) {  // NOTE: 0 is not an allowed value!
          Log.warning(F("/setBrightness: %d is an unallowed brightness setting"), b);
          WebUI::closeConnection(400, "Invalid Brightness: " + WebUI::arg(F("brightness")));
        } else {
          Display.setBrightness(b);
          WebUI::closeConnection(200, F("Brightness Set"));
        }
      };

      WebUI::wrapWebAction("/setBrightness", action);
    }

    void updateStatus() {
      auto action = []() {
        wtAppImpl->updateAllData();
        WebUI::redirectHome();
      };

      WebUI::wrapWebAction("/updateStatus", action, false);
    }

    void updateWeatherConfig() {
      auto action = []() {
        wtApp->settings->owmOptions.enabled = WebUI::hasArg(F("useOWM"));
        wtApp->settings->uiOptions.useMetric = WebUI::hasArg(F("metric"));
        wtApp->settings->owmOptions.key = WebUI::arg(F("openWeatherMapApiKey"));
        wtApp->settings->owmOptions.cityID = WebUI::arg(F("cityID")).toInt();
        wtApp->settings->owmOptions.language = WebUI::arg(F("language"));
        wtApp->settings->owmOptions.nickname = WebUI::arg(F("nickname"));

        wtApp->settings->write();
        // wtApp->settings->logSettings();
        
        wtAppImpl->weatherConfigMayHaveChanged();

        WebUI::redirectHome();
      };

      WebUI::wrapWebAction("/updateWeatherConfig", action);
    }

    void updatePluginConfig() {
      auto action = []() {
        String responseType = "text/plain";
        if (!WebUI::hasArg(F("pID"))) {
          uint8_t pID = WebUI::arg(F("pID")).toInt()-1;
          Plugin *p = wtAppImpl->pluginMgr.getPlugin(pID);

          if (p) {
            if (WebUI::hasArg(F("plain"))) {
              String settings = WebUI::arg("plain");
              p->newSettings(settings);        
              WebUI::sendStringContent(responseType, F("OK: settings updated"));
            } else {
              Log.warning(F("No payload supplied"));
              WebUI::sendStringContent(responseType, F("ERR: No payload"));
            }
          } else {
            Log.warning(F("Invalid plugin ID: %d"), pID);
            WebUI::sendStringContent(responseType, F("ERR: Invalid plugin ID"));
          }
        } else {
          Log.warning(F("No plugin ID was supplied"));
          WebUI::sendStringContent(responseType, F("ERR: No plugin ID"));
        }
      };

      WebUI::wrapWebAction("/updatePluginConfig", action);
    }

    void updateDisplayConfig() {
      auto action = []() {
        wtApp->settings->uiOptions.schedule.active = WebUI::hasArg(F("scheduleEnabled"));
        String t = WebUI::arg(F("morning"));
        int separator = t.indexOf(":");
        wtApp->settings->uiOptions.schedule.morning.hr = t.substring(0, separator).toInt();
        wtApp->settings->uiOptions.schedule.morning.min = t.substring(separator+1).toInt();
        wtApp->settings->uiOptions.schedule.morning.brightness = WebUI::arg(F("mBright")).toInt();

        t = WebUI::arg(F("evening"));
        separator = t.indexOf(":");
        wtApp->settings->uiOptions.schedule.evening.hr = t.substring(0, separator).toInt();
        wtApp->settings->uiOptions.schedule.evening.min = t.substring(separator+1).toInt();
        wtApp->settings->uiOptions.schedule.evening.brightness = WebUI::arg(F("eBright")).toInt();

        wtApp->settings->uiOptions.use24Hour = WebUI::hasArg(F("is24hour"));
        wtApp->settings->uiOptions.screenBlankMinutes = WebUI::arg(F("blank")).toInt();
        wtApp->settings->displayOptions.invertDisplay = WebUI::hasArg(F("invDisp"));

        wtApp->settings->write();
        //wtApp->settings->logSettings();

        WebUI::redirectHome();
      };
      
      WebUI::wrapWebAction("/updateDisplayConfig", action);
    }
  }   // ----- END: WebUIHelper::Endpoints

  // ----- BEGIN: WebUIHelper::Dev
  namespace Dev {
    void reboot() {
      auto action = []() {
        wtAppImpl->askToReboot();
        WebUI::redirectHome();
      };

      WebUI::wrapWebAction("/dev/reboot", action, false);
    }

    void forceScreen() {
      auto action = []() {
        String screen = WebUI::arg(F("screen"));
        Log.trace(F("/dev/forceScreen?screen=%s"), screen.c_str());
        if (!screen.isEmpty()) {
          if (screen.equalsIgnoreCase("home")) ScreenMgr.displayHomeScreen();
          else ScreenMgr.display(screen);
        }
        WebUI::redirectHome();
      };

      WebUI::wrapWebAction("/dev/forceScreen", action, false);
    }

    void yieldScreenShot() {
      auto action = []() {
        WebUI::sendArbitraryContent(
            "image/bmp", Display.ScreenShotSize, 
            [](Stream& s) { Display.streamScreenShotAsBMP(s); });
      };
      WebUI::wrapWebAction("/dev/screenShot", action, false);
    }
  }   // ----- END: WebUIHelper::Dev


  // ----- BEGIN: WebUIHelper::Default
  namespace Default {
    void updateDevConfig() {
      auto action = []() {
        wtApp->settings->uiOptions.showDevMenu = WebUI::hasArg("showDevMenu");
        wtApp->settings->write();
        WebUI::redirectHome();
      };

      WebUI::wrapWebAction("/updateDevConfig", action);
    }

    void homePage() {
      auto mapper =[](const String& key, String &val) -> void {
        if (key.equals(F("CITYID"))) {
          if (wtApp->settings->owmOptions.enabled) val.concat(wtApp->settings->owmOptions.cityID);
          else val.concat("5380748");  // Palo Alto, CA, USA
        }
        else if (key.equals(F("WEATHER_KEY"))) val = wtApp->settings->owmOptions.key;
        else if (key.equals(F("UNITS"))) val.concat(wtApp->settings->uiOptions.useMetric ? "metric" : "imperial");
        else if (key.equals(F("BRIGHT"))) val.concat(Display.getBrightness());
      };

      WebUI::wrapWebPage("/", "/wta/HomePage.html", mapper);
    }
  }  // ----- END: WebUIHelper::Default

  namespace Pages {
    void presentWeatherConfig() {
      String langTarget = "SL" + wtApp->settings->owmOptions.language;

      auto mapper =[&langTarget](const String& key, String& val) -> void {
        if (key.equals(F("WEATHER_KEY"))) val = wtApp->settings->owmOptions.key;
        else if (key.equals(F("CITY_NAME")) && wtApp->settings->owmOptions.enabled) {
          if (wtApp->owmClient) val = wtApp->owmClient->weather.location.city;
        }
        else if (key.equals(F("NICKNAME"))) val = wtApp->settings->owmOptions.nickname;
        else if (key.equals(F("CITY"))) val.concat(wtApp->settings->owmOptions.cityID);
        else if (key.equals(F("USE_METRIC"))) val = checkedOrNot[wtApp->settings->uiOptions.useMetric];
        else if (key == langTarget) val = "selected";
        else if (key.equals(F("USE_OWM")))  val = checkedOrNot[wtApp->settings->owmOptions.enabled];
      };

      WebUI::wrapWebPage("/presentWeatherConfig", "/wta/ConfigWeather.html", mapper);
    }


    void presentPluginConfig() {
      uint8_t count = wtAppImpl->pluginMgr.getPluginCount();
      Plugin** plugins = wtAppImpl->pluginMgr.getPlugins();

      auto mapper =[count, plugins](const String& key, String& val) -> void {
        if (key.startsWith("_P")) {
          int pluginIndex = (key.charAt(2) - '0') - 1;
          if (pluginIndex < count) {
            Plugin* p = plugins[pluginIndex];

            const char* subkey = &(key.c_str()[4]);
            if (strcmp(subkey, "IDX") == 0) val.concat(pluginIndex+1);
            if (strcmp(subkey, "NAME") == 0) val = p->getName();
            if (strcmp(subkey, "FORM") == 0) p->getForm(val);
            if (strcmp(subkey, "VALS") == 0) p->getSettings(val);
          }
        }
      };

      WebUI::wrapWebPage("/presentPluginConfig", "/wta/ConfigPlugins.html", mapper);
    }

    void presentDisplayConfig() {
      UIOptions* uiOptions = &(wtApp->settings->uiOptions);

      auto mapper =[uiOptions](const String& key, String& val) -> void {
        if (key.equals(F("SCHED_ENABLED"))) val = checkedOrNot[uiOptions->schedule.active];
        else if (key.equals(F("MORN"))) val = WebThing::formattedInterval(
          uiOptions->schedule.morning.hr, uiOptions->schedule.morning.min, 0, true, false);
        else if (key.equals(F("EVE"))) val = WebThing::formattedInterval(
          uiOptions->schedule.evening.hr, uiOptions->schedule.evening.min, 0, true, false);
        else if (key.equals(F("M_BRIGHT"))) val.concat(uiOptions->schedule.morning.brightness);
        else if (key.equals(F("E_BRIGHT"))) val.concat(uiOptions->schedule.evening.brightness);

        else if (key.equals(F("USE_24HOUR"))) val = checkedOrNot[uiOptions->use24Hour];
        else if (key.equals(F("BLANK")))  val.concat(uiOptions->screenBlankMinutes);
        else if (key.equals(F("INVERT_DISPLAY"))) val = checkedOrNot[wtApp->settings->displayOptions.invertDisplay];
      };

      WebUI::wrapWebPage("/presentPluginConfig", "/wta/ConfigDisplay.html", mapper);
    }
  }   // ----- END: WebUIHelper::Pages

  void showBusyStatus(bool busy) {
    if (busy) ScreenMgr.showActivityIcon(Theme::Color_WebRequest, UpdatingSymbol);
    else ScreenMgr.hideActivityIcon();
  }

  void init(const __FlashStringHelper* appMenuItems) {
    WebUI::registerBusyCallback(showBusyStatus);

    WebUI::addCoreMenuItems(Internal::CORE_MENU_ITEMS);
    WebUI::addAppMenuItems(appMenuItems);
    WebUI::Dev::init(&wtApp->settings->uiOptions.showDevMenu, wtApp->settings);

    WebUI::Dev::addButton({"Take a screen shot", "/dev/screenShot", nullptr, nullptr});

    WebUI::registerHandler("/presentWeatherConfig",   Pages::presentWeatherConfig);
    WebUI::registerHandler("/presentDisplayConfig",   Pages::presentDisplayConfig);
    WebUI::registerHandler("/presentPluginConfig",    Pages::presentPluginConfig);

    WebUI::registerHandler("/updateStatus",           Endpoints::updateStatus);
    WebUI::registerHandler("/updateWeatherConfig",    Endpoints::updateWeatherConfig);
    WebUI::registerHandler("/updateDisplayConfig",    Endpoints::updateDisplayConfig);
    WebUI::registerHandler("/updatePluginConfig",     Endpoints::updatePluginConfig);
    WebUI::registerHandler("/setBrightness",          Endpoints::setBrightness);

    WebUI::registerHandler("/dev/reboot",             Dev::reboot); // Override the default from WebUI
    WebUI::registerHandler("/dev/screenShot",         Dev::yieldScreenShot);
    WebUI::registerHandler("/dev/forceScreen",        Dev::forceScreen);
  }

}
// ----- END: WebUIHelper