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
#include <Output.h>
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
      "<a class='w3-bar-item w3-button' href='/presentDisplayConfig'>"
      "<i class='fa fa-desktop'></i> Configure Display</a>"
      "<a class='w3-bar-item w3-button' href='/presentWeatherConfig'>"
      "<i class='fa fa-thermometer-three-quarters'></i> Configure Weather</a>"
      "<a class='w3-bar-item w3-button' href='/presentPluginConfig'>"
      "<i class='fa fa-plug'></i> Configure Plugins</a>"
      "<a class='w3-bar-item w3-button' href='/updateStatus'>"
      "<i class='fa fa-refresh'></i> Update Status</a>");
  }

  // ----- END: WebUIHelper::Internal


  // ----- BEGIN: WebUIHelper::Endpoints
  namespace Endpoints {
    void setBrightness() {
      auto action = []() {
        String bString = WebUI::arg(F("value"));
        bString.trim();
        if (!isDigit(bString[0])) {
          Log.warning(F("/setBrightness: %s isn't valid"), bString.c_str());
          WebUI::closeConnection(400, "Invalid Brightness: " + bString);
          return;
        }
        uint8_t b = bString.toInt();
          // We know there is at least one leading digit, so a zero result
          // really means zero, not an error parsing
        if (b > 100) { 
          Log.warning(F("/setBrightness: %d is an unallowed brightness setting"), b);
          WebUI::closeConnection(400, "Invalid Brightness: " + WebUI::arg(F("brightness")));
        } else {
          ScreenMgr.setBrightness(b); // Honors screen blanking
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

    void updateWthrSettings() {
      auto action = [&]() {
        // We are handling an HTTP POST with a JSON payload. There isn't a specific function
        // to get the payload from the request, instead ask for the arg named "plain"
        String newSettings = WebUI::arg("plain");
        if (newSettings.isEmpty()) {
          WebUI::sendStringContent("text/plain", "Settings are empty", "400 Bad Request");
          return;
        }
        wtApp->settings->owmOptions.fromJSON(newSettings);
        wtApp->settings->write();
        WebUI::sendStringContent("text/plain", "New weather settings were saved");
      };

      WebUI::wrapWebAction("/updateWthrSettings", action);
    }

    void updatePluginConfig() {
      auto action = []() {
        String responseType = "text/plain";
        if (WebUI::hasArg(F("pID"))) {
          uint8_t pID = WebUI::arg(F("pID")).toInt()-1;
          Plugin *p = wtAppImpl->pluginMgr.getUnifiedPlugin(pID);

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
        bool curUseMetric = wtApp->settings->uiOptions.useMetric;

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

        wtApp->settings->uiOptions.screenBlankMinutes = WebUI::arg(F("blank")).toInt();
        if (wtApp->settings->uiOptions.schedule.active && 
            wtApp->settings->uiOptions.screenBlankMinutes != 0) {
          // We don't want schedules and screen blanking on at the same time. The UI will
          // avoid this, but if for some reason we get incompatible settings, choose the schedule
          wtApp->settings->uiOptions.screenBlankMinutes = 0;
        }

        wtApp->settings->uiOptions.use24Hour = WebUI::hasArg(F("is24hour"));
        wtApp->settings->uiOptions.useMetric = WebUI::hasArg(F("metric"));
        wtApp->settings->displayOptions.invertDisplay = WebUI::hasArg(F("invDisp"));

        wtApp->settings->write();
        //wtApp->settings->logSettings();

        if (curUseMetric != wtApp->settings->uiOptions.useMetric) {
          wtAppImpl->weatherConfigMayHaveChanged();
        }

        WebUI::redirectHome();
      };
      
      WebUI::wrapWebAction("/updateDisplayConfig", action);
      Output::setOptions(
        &wtApp->settings->uiOptions.useMetric,
        &wtApp->settings->uiOptions.use24Hour);
    }

    void updateScreenSelection() {
      auto action = []() {
        // We are handling an HTTP POST with a JSON payload. There isn't a specific function
        // to get the payload from the request, instead ask for the arg named "plain"
        String newSequence = WebUI::arg("plain");
        if (newSequence.isEmpty()) {
         WebUI::sendStringContent("text/plain", "No screens were selected", "400 Bad Request");
          return;
        }
        wtAppImpl->settings->screenSettings.fromJSON(newSequence);
        ScreenMgr.reconcileScreenSequence(wtAppImpl->settings->screenSettings);
        wtApp->settings->write();
        WebUI::sendStringContent("text/plain", "New screen sequence was saved");
      };
      
      WebUI::wrapWebAction("/updateScreenSelection", action);
      ScreenMgr.beginSequence();
        // Necessary to ensure we're in a sensible state after setting a new 
        // sequence when we may be in the middle of the previous sequence
        // Do this after wrapWebAction so the newly displayed screen doesn't end
        // up having the ActivityIcon restored from the previous screen.
    }
  }   // ----- END: WebUIHelper::Endpoints

  // ----- BEGIN: WebUIHelper::Dev
  namespace Dev {
    void reboot() {
      auto action = []() {
        WebUI::redirectHome();
          // Make sure this gets done first or the reboot request
          // may be repeated over and over!
        wtAppImpl->askToReboot();
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
            "image/bmp", Display.screenShotSize(), 
            [](Stream& s) { Display.streamScreenShotAsBMP(s); });
      };
      WebUI::wrapWebAction("/dev/screenShot", action, false);
    }
  }   // ----- END: WebUIHelper::Dev


  // ----- BEGIN: WebUIHelper::Default
  namespace Default {
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
        else if (key == langTarget) val = "selected";
        else if (key.equals(F("USE_OWM")))  val = checkedOrNot[wtApp->settings->owmOptions.enabled];
        else if (key.equals(F("WTHR_SETTINGS"))) wtApp->settings->owmOptions.toJSON(val);
      };

      WebUI::wrapWebPage("/presentWeatherConfig", "/wta/ConfigWeather.html", mapper);
    }


    void presentPluginConfig() {
      auto mapper = [](const String& key, String& val) -> void {
        if (key.startsWith("_P")) {
          int pluginIndex = (key.charAt(2) - '0') - 1;
          Plugin* p = wtAppImpl->pluginMgr.getUnifiedPlugin(pluginIndex);

          if (p != nullptr) {
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
      String blankingTarget("SL");
      blankingTarget += uiOptions->screenBlankMinutes;

      auto mapper =[uiOptions, &blankingTarget](const String& key, String& val) -> void {
        if (key.equals(F("SCHED_ENABLED"))) val = checkedOrNot[uiOptions->schedule.active];
        else if (key.equals(F("MORN"))) val = WebThing::formattedInterval(
          uiOptions->schedule.morning.hr, uiOptions->schedule.morning.min, 0, true, false);
        else if (key.equals(F("EVE"))) val = WebThing::formattedInterval(
          uiOptions->schedule.evening.hr, uiOptions->schedule.evening.min, 0, true, false);
        else if (key.equals(F("M_BRIGHT"))) val.concat(uiOptions->schedule.morning.brightness);
        else if (key.equals(F("E_BRIGHT"))) val.concat(uiOptions->schedule.evening.brightness);

        else if (key.equals(F("BRIGHT"))) val.concat(Display.getBrightness());

        else if (key.equals(F("USE_24HOUR"))) val = checkedOrNot[uiOptions->use24Hour];
        else if (key.equals(F("USE_METRIC"))) val = checkedOrNot[wtApp->settings->uiOptions.useMetric];
        else if (key == blankingTarget) val = "selected";
        else if (key.equals(F("INVERT_DISPLAY"))) val = checkedOrNot[wtApp->settings->displayOptions.invertDisplay];
      };

      WebUI::wrapWebPage("/presentDisplayConfig", "/wta/ConfigDisplay.html", mapper);
    }

    void presentScreenConfig() {
      UIOptions* uiOptions = &(wtApp->settings->uiOptions);

      auto mapper =[uiOptions](const String& key, String& val) -> void {
        if (key.equals(F("ORIG_SEQ"))) wtAppImpl->settings->screenSettings.toJSON(val);
      };

      WebUI::wrapWebPage("/presentScreenConfig", "/wta/ConfigScreens.html", mapper);
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

    WebUI::Dev::addButton({"Take a screen shot", "/dev/screenShot", nullptr, nullptr});

    WebUI::registerHandler("/presentWeatherConfig",   Pages::presentWeatherConfig);
    WebUI::registerHandler("/presentDisplayConfig",   Pages::presentDisplayConfig);
    WebUI::registerHandler("/presentPluginConfig",    Pages::presentPluginConfig);
    WebUI::registerHandler("/presentScreenConfig",    Pages::presentScreenConfig);

    WebUI::registerHandler("/updateStatus",           Endpoints::updateStatus);
    WebUI::registerHandler("/updateWthrSettings",     Endpoints::updateWthrSettings);
    WebUI::registerHandler("/updateDisplayConfig",    Endpoints::updateDisplayConfig);
    WebUI::registerHandler("/updatePluginConfig",     Endpoints::updatePluginConfig);
    WebUI::registerHandler("/updateScreenSelection",  Endpoints::updateScreenSelection);
    WebUI::registerHandler("/setBrightness",          Endpoints::setBrightness);

    WebUI::registerHandler("/dev/reboot",             Dev::reboot); // Override the default from WebUI
    WebUI::registerHandler("/dev/screenShot",         Dev::yieldScreenShot);
    WebUI::registerHandler("/dev/forceScreen",        Dev::forceScreen);
  }

}
// ----- END: WebUIHelper