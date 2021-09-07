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
#include "WebUIHelper.h"
#include "WTAppImpl.h"
#include "gui/Theme.h"
#include "gui/Display.h"
#include "gui/ScreenMgr.h"
//--------------- End:    Includes ---------------------------------------------


// ----- BEGIN: WebUIHelper namespace
namespace WebUIHelper {
  constexpr char UpdatingSymbol = 'w';
  
  const String   checkedOrNot[2] = {"", "checked='checked'"};
  ESPTemplateProcessor  *templateHandler;

  void showUpdatingIcon() {
    ScreenMgr::showUpdatingIcon(Theme::Color_WebRequest, UpdatingSymbol);
  }

  void hideUpdatingIcon() { ScreenMgr::hideUpdatingIcon(); }

  void wrapWebAction(const char* actionName, std::function<void(void)> action, bool showIcon) {
    Log.trace(F("Handling %s"), actionName);
    if (!WebUI::authenticationOK()) { return; }

    if (showIcon) showUpdatingIcon();
    action();
    if (showIcon) hideUpdatingIcon();
  }

  void wrapWebPage(
      const char* pageName, const char* htmlTemplate,
      ESPTemplateProcessor::Mapper mapper,
      bool showIcon)
  {
    Log.trace(F("Handling %s"), pageName);
    if (!WebUI::authenticationOK()) { return; }

    if (showIcon) showUpdatingIcon();
    WebUI::startPage();
    templateHandler->send(htmlTemplate, mapper);
    WebUI::finishPage();
    if (showIcon) hideUpdatingIcon();
  }

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

    const __FlashStringHelper* DEV_MENU_ITEMS = FPSTR(
      "<a class='w3-bar-item w3-button' href='/dev'>"
      "<i class='fa fa-gears'></i> Dev Settings</a>");
  }

  // ----- END: WebUIHelper::Internal


  // ----- BEGIN: WebUIHelper::Endpoints
  namespace Endpoints {
    void setBrightness() {
      auto action = []() {
        showUpdatingIcon();
        uint8_t b = WebUI::arg(F("value")).toInt();
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
      auto action = []() {
        wtAppImpl->updateAllData();
        WebUI::redirectHome();
      };

      wrapWebAction("/updateStatus", action, false);
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

      wrapWebAction("/updatePluginConfig", action);
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

      wrapWebAction("/dev/reboot", action, false);
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

      wrapWebAction("/dev/forceScreen", action, false);
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
      wrapWebAction("/dev/screenShot", action, false);
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
        auto mapper =[](const String& key, String& val) -> void {
          if (key.equals(F("HEAP"))) { DataBroker::map("$S.heap", val); }
          else if (key.equals("SHOW_DEV_MENU")) {
            val = checkedOrNot[wtApp->settings->uiOptions.showDevMenu];
          }
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
        auto mapper =[](const String &key, String &val) -> void {
          if (key.equals(F("CITYID"))) {
            if (wtApp->settings->owmOptions.enabled) val.concat(wtApp->settings->owmOptions.cityID);
            else val.concat("5380748");  // Palo Alto, CA, USA
          }
          else if (key.equals(F("WEATHER_KEY"))) val = wtApp->settings->owmOptions.key;
          else if (key.equals(F("UNITS"))) val.concat(wtApp->settings->uiOptions.useMetric ? "metric" : "imperial");
          else if (key.equals(F("BRIGHT"))) val.concat(Display::getBrightness());
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

      auto mapper =[&langTarget](const String &key, String& val) -> void {
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

      wrapWebPage("/presentWeatherConfig", "/wta/ConfigWeather.html", mapper);
    }


    void presentPluginConfig() {
      uint8_t count = wtAppImpl->pluginMgr.getPluginCount();
      Plugin** plugins = wtAppImpl->pluginMgr.getPlugins();

      auto mapper =[count, plugins](const String &key, String& val) -> void {
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

      wrapWebPage("/presentPluginConfig", "/wta/ConfigPlugins.html", mapper);
    }

    void presentDisplayConfig() {
      UIOptions* uiOptions = &(wtApp->settings->uiOptions);

      auto mapper =[uiOptions](const String &key, String& val) -> void {
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

      wrapWebPage("/presentPluginConfig", "/wta/ConfigDisplay.html", mapper);
    }
  }   // ----- END: WebUIHelper::Pages


  void init(const __FlashStringHelper* appMenuItems) {
    templateHandler = WebUI::getTemplateHandler();

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

    WebUI::registerBusyCallback([](bool busy) -> void { 
        if (busy) showUpdatingIcon();
        else hideUpdatingIcon();
      }
    );
  }

}
// ----- END: WebUIHelper