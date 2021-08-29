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
//                                  WebThing Includes
#include <DataBroker.h>
#include <WebThing.h>
#include <WebUI.h>
//                                  Local Includes
#include "WTAppImpl.h"
#include "gui/Theme.h"
#include "gui/Display.h"
#include "gui/ScreenMgr.h"
//--------------- End:    Includes ---------------------------------------------


// ----- BEGIN: WebUIHelper namespace
namespace WebUIHelper {
  static const char UpdatingSymbol = 'w';
  static const String   checkedOrNot[2] = {"", "checked='checked'"};
  ESPTemplateProcessor  *templateHandler;

  void showUpdatingIcon() {
    ScreenMgr::showUpdatingIcon(Theme::Color_WebRequest, UpdatingSymbol);
  }

  void hideUpdatingIcon() { ScreenMgr::hideUpdatingIcon(); }


  void wrapWebAction(const char* actionName, std::function<void(void)> action) {
    Log.trace(F("Handling %s"), actionName);
    if (!WebUI::authenticationOK()) { return; }

    showUpdatingIcon();
    action();
    hideUpdatingIcon();
  }

  void wrapWebPage(
      const char* pageName, const char* htmlTemplate,
      ESPTemplateProcessor::ProcessorCallback mapper)
  {
    Log.trace(F("Handling %s"), pageName);
    if (!WebUI::authenticationOK()) { return; }

    showUpdatingIcon();
    WebUI::startPage();
    templateHandler->send(htmlTemplate, mapper);
    WebUI::finishPage();
    hideUpdatingIcon();
  }

  // ----- BEGIN: WebUIHelper::Internal
  namespace Internal {
    const char CORE_MENU_ITEMS[] PROGMEM =
      "<a class='w3-bar-item w3-button' href='/updateStatus'>"
      "<i class='fa fa-recycle'></i> Update Status</a>"
      "<a class='w3-bar-item w3-button' href='/presentDisplayConfig'>"
      "<i class='fa fa-desktop'></i> Configure Display</a>"
      "<a class='w3-bar-item w3-button' href='/presentWeatherConfig'>"
      "<i class='fa fa-thermometer-three-quarters'></i> Configure Weather</a>"
      "<a class='w3-bar-item w3-button' href='/presentPluginConfig'>"
      "<i class='fa fa-plug'></i> Configure Plugins</a>";

    const char DEV_MENU_ITEMS[] PROGMEM =
      "<a class='w3-bar-item w3-button' href='/dev'>"
      "<i class='fa fa-gears'></i> Dev Settings</a>";
  }

  // ----- END: WebUIHelper::Internal


  // ----- BEGIN: WebUIHelper::Endpoints
  namespace Endpoints {
    void setBrightness() {
      auto action = []() {
        showUpdatingIcon();
        uint8_t b = WebUI::arg(F("brightness")).toInt();
        if (b <= 0 || b > 100) {  // NOTE: 0 is not an allowed value!
          Log.warning(F("/setBrightness: %d is an unallowed brightness setting"), b);
          WebUI::closeConnection(400, "Invalid Brightness: " + WebUI::arg(F("brightness")));
        } else {
          Display::setBrightness(b);
          WebUI::closeConnection(200, F("Brightness Set"));
        }
      };

      wrapWebAction("/setBrightness", action);
    }

    void updateStatus() {
      // NOTE: Don't display the updating icon. It will get done by the updating actions
      if (!WebUI::authenticationOK()) { return; }
      wtAppImpl->updateAllData();
      WebUI::redirectHome();
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

      wrapWebAction("/updateWeatherConfig", action);
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

      wrapWebAction("/updateDisplayConfig", action);
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
        wtApp->settings->displayOptions.invertDisplay = WebUI::hasArg(F("invDisp"));

        wtApp->settings->write();
        //wtApp->settings->logSettings();

        WebUI::redirectHome();
      };
      
      wrapWebAction("/updateDisplayConfig", action);
    }
  }   // ----- END: WebUIHelper::Endpoints

  // ----- BEGIN: WebUIHelper::Dev
  namespace Dev {
    void reboot() {
      auto action = []() {
        wtAppImpl->askToReboot();
        WebUI::redirectHome();
      };

      wrapWebAction("/dev/reboot", action);
    }

    void forceScreen() {
      auto action = []() {
        String screen = WebUI::arg(F("screen"));
        Log.trace(F("/dev/forceScreen?screen=%s"), screen.c_str());
        if (!screen.isEmpty()) {
          if (screen.equalsIgnoreCase("home")) ScreenMgr::displayHomeScreen();
          else ScreenMgr::display(screen);
        }
        WebUI::redirectHome();
      };

      wrapWebAction("/dev/forceScreen", action);
    }

    void yieldSettings() {
      auto action = []() {
        DynamicJsonDocument *doc = (WebUI::hasArg("wt")) ? WebThing::settings.asJSON() :
                                                           wtApp->settings->asJSON();
        WebUI::sendJSONContent(doc);
        doc->clear();
        delete doc;
      };
      wrapWebAction("/dev/settings", action);
    }

    void yieldScreenShot() {
      auto action = []() {
        WebUI::sendArbitraryContent(
            "image/bmp",
            Display::getSizeOfScreenShotAsBMP(),
            Display::streamScreenShotAsBMP);
      };
      wrapWebAction("/dev/screenShot", action);
    }

    void enableDevMenu() {
      auto action = []() {
        wtApp->settings->uiOptions.showDevMenu = WebUI::hasArg("showDevMenu");
        wtApp->settings->write();
        WebUI::redirectHome();
      };
      wrapWebAction("/dev/enableDevMenu", action);
    }

    void getDataBrokerValue() {
      auto action = []() {
        String key = "$" + WebUI::arg(F("key"));
        String value;
        DataBroker::map(key, value);
        String result = key + ": " + value;
        WebUI::sendStringContent("text", result);
      };

      wrapWebAction("/dev/data", action);
    }
  }   // ----- END: WebUIHelper::Dev


  // ----- BEGIN: WebUIHelper::Default
  namespace Default {
    void devPage() {
      auto action = []() {
        auto mapper =[](String &key) -> String {
          if (key.equals(F("HEAP"))) {
            String heapStats;
            heapStats.reserve(32);
            DataBroker::map("$S.heap", heapStats);
            return heapStats;
          }
          if (key.equals("SHOW_DEV_MENU")) return checkedOrNot[wtApp->settings->uiOptions.showDevMenu];
          return WTBasics::EmptyString;
        };

        WebUI::startPage();
        templateHandler->send("/wta/ConfigDev.html", mapper);
        WebUI::finishPage();
      };

      wrapWebAction("/dev", action);
    }

    void updateDevConfig() {
      auto action = []() {
        wtApp->settings->uiOptions.showDevMenu = WebUI::hasArg("showDevMenu");
        wtApp->settings->write();
        WebUI::redirectHome();
      };

      wrapWebAction("/updateDevConfig", action);
    }

    void homePage() {
      auto action = []() {
        auto mapper =[](String &key) -> String {
          if (key.equals(F("CITYID"))) {
            if (wtApp->settings->owmOptions.enabled) return String(wtApp->settings->owmOptions.cityID);
            else return String("5380748");  // Palo Alto, CA, USA
          }
          if (key.equals(F("WEATHER_KEY"))) return wtApp->settings->owmOptions.key;
          if (key.equals(F("UNITS"))) return wtApp->settings->uiOptions.useMetric ? "metric" : "imperial";
          if (key.equals(F("BRIGHT"))) return String(Display::getBrightness());
          return WTBasics::EmptyString;
        };

        WebUI::startPage();
        templateHandler->send("/wta/HomePage.html", mapper);
        WebUI::finishPage();
      };

      wrapWebAction("/", action);
    }
  }  // ----- END: WebUIHelper::Default

  namespace Pages {
    void presentWeatherConfig() {
      String langTarget = "SL" + wtApp->settings->owmOptions.language;

      auto mapper =[&langTarget](String &key) -> String {
        if (key.equals(F("WEATHER_KEY"))) return wtApp->settings->owmOptions.key;
        if (key.equals(F("CITY_NAME")) && wtApp->settings->owmOptions.enabled) {
          return wtApp->owmClient == NULL ? WTBasics::EmptyString : wtApp->owmClient->weather.location.city;
        }
        if (key.equals(F("NICKNAME"))) return String(wtApp->settings->owmOptions.nickname);
        if (key.equals(F("CITY"))) return String(wtApp->settings->owmOptions.cityID);
        if (key.equals(F("USE_METRIC"))) return checkedOrNot[wtApp->settings->uiOptions.useMetric];
        if (key == langTarget) return "selected";
        if (key.equals(F("USE_OWM")))  return checkedOrNot[wtApp->settings->owmOptions.enabled];
        return WTBasics::EmptyString;
      };

      wrapWebPage("/presentWeatherConfig", "/wta/ConfigWeather.html", mapper);
    }


    void presentPluginConfig() {
      uint8_t count = wtAppImpl->pluginMgr.getPluginCount();
      Plugin** plugins = wtAppImpl->pluginMgr.getPlugins();

      auto mapper =[count, plugins](String &key) -> String {
        if (key.startsWith("_P")) {
          int pluginIndex = (key.charAt(2) - '0') - 1;
          if (pluginIndex < count) {
            Plugin* p = plugins[pluginIndex];

            key.remove(0, 4); // Get rid of the prefix; e.g. _P1_
            if (key.equals(F("IDX"))) return String(pluginIndex+1);
            if (key.equals(F("NAME"))) { return p->getName(); }
            if (key.equals(F("FORM"))) { String v; p->getForm(v); return v; }
            if (key.equals(F("VALS"))) { String v; p->getSettings(v); return v; }
          }
        }
        return WTBasics::EmptyString;
      };

      wrapWebPage("/presentPluginConfig", "/wta/ConfigPlugins.html", mapper);
    }

    void presentDisplayConfig() {
      auto mapper =[](String &key) -> String {
        if (key.equals(F("SCHED_ENABLED"))) return checkedOrNot[wtApp->settings->uiOptions.schedule.active];
        if (key.equals(F("MORN"))) return WebThing::formattedInterval(
          wtApp->settings->uiOptions.schedule.morning.hr, wtApp->settings->uiOptions.schedule.morning.min, 0, true, false);
        if (key.equals(F("EVE"))) return WebThing::formattedInterval(
          wtApp->settings->uiOptions.schedule.evening.hr, wtApp->settings->uiOptions.schedule.evening.min, 0, true, false);
        if (key.equals(F("M_BRIGHT"))) return String(wtApp->settings->uiOptions.schedule.morning.brightness);
        if (key.equals(F("E_BRIGHT"))) return String(wtApp->settings->uiOptions.schedule.evening.brightness);

        if (key.equals(F("USE_24HOUR"))) return checkedOrNot[wtApp->settings->uiOptions.use24Hour];
        if (key.equals(F("INVERT_DISPLAY"))) return checkedOrNot[wtApp->settings->displayOptions.invertDisplay];
        return WTBasics::EmptyString;
      };

      wrapWebPage("/presentPluginConfig", "/wta/ConfigDisplay.html", mapper);
    }
  }   // ----- END: WebUIHelper::Pages


  void init(PGM_P appMenuItems) {
    WebUI::addCoreMenuItems(Internal::CORE_MENU_ITEMS);
    WebUI::addAppMenuItems(appMenuItems);
    if (wtApp->settings->uiOptions.showDevMenu) {
      WebUI::addDevMenuItems(Internal::DEV_MENU_ITEMS);
    }

    WebUI::registerHandler("/presentWeatherConfig",   Pages::presentWeatherConfig);
    WebUI::registerHandler("/presentDisplayConfig",   Pages::presentDisplayConfig);
    WebUI::registerHandler("/presentPluginConfig",    Pages::presentPluginConfig);

    WebUI::registerHandler("/updateStatus",           Endpoints::updateStatus);
    WebUI::registerHandler("/updateWeatherConfig",    Endpoints::updateWeatherConfig);
    WebUI::registerHandler("/updateDisplayConfig",    Endpoints::updateDisplayConfig);
    WebUI::registerHandler("/updatePluginConfig",     Endpoints::updatePluginConfig);
    WebUI::registerHandler("/setBrightness",          Endpoints::setBrightness);

    WebUI::registerHandler("/dev/reboot",             Dev::reboot);
    WebUI::registerHandler("/dev/settings",           Dev::yieldSettings);
    WebUI::registerHandler("/dev/screenShot",         Dev::yieldScreenShot);
    WebUI::registerHandler("/dev/forceScreen",        Dev::forceScreen);
    WebUI::registerHandler("/dev/enableDevMenu",      Dev::enableDevMenu);
    WebUI::registerHandler("/dev/data",               Dev::getDataBrokerValue);

    templateHandler = WebUI::getTemplateHandler();
  }

}
// ----- END: WebUIHelper