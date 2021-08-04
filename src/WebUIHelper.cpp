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
#include "gui/Display.h"
#include "gui/ScreenMgr.h"
//--------------- End:    Includes ---------------------------------------------


// ----- BEGIN: WebUIHelper namespace
namespace WebUIHelper {
  static const String   checkedOrNot[2] = {"", "checked='checked'"};
  ESPTemplateProcessor  *templateHandler;

  // ----- BEGIN: WebUIHelper::Internal
  namespace Internal {
    String BASE_ACTIONS =
      "<a class='w3-bar-item w3-button' href='/updateStatus'>"
      "<i class='fa fa-recycle'></i> Update Status</a>"
      "<a class='w3-bar-item w3-button' href='/presentPrinterConfig'>"
      "<i class='fa fa-cog'></i> Configure Printers</a>"
      "<a class='w3-bar-item w3-button' href='/presentDisplayConfig'>"
      "<i class='fa fa-desktop'></i> Configure Display</a>"
      "<a class='w3-bar-item w3-button' href='/presentWeatherConfig'>"
      "<i class='fa fa-thermometer-three-quarters'></i> Configure Weather</a>"
      "<a class='w3-bar-item w3-button' href='/presentPluginConfig'>"
      "<i class='fa fa-plug'></i> Configure Plugins</a>";

    String DEV_ACTION =
      "<a class='w3-bar-item w3-button' href='/dev'>"
      "<i class='fa fa-gears'></i> Dev Settings</a>";
  }

  // ----- END: WebUIHelper::Internal


  // ----- BEGIN: WebUIHelper::Endpoints
  namespace Endpoints {
    void setBrightness() {
      if (!WebUI::authenticationOK()) { return; }
      Log.trace(F("Handling /setBrightness"));

      uint8_t b = WebUI::arg(F("brightness")).toInt();
      if (b <= 0 || b > 100) {  // NOTE: 0 is not an allowed value!
        Log.warning(F("/setBrightness: %d is an unallowed brightness setting"), b);
        WebUI::closeConnection(400, "Invalid Brightness: " + WebUI::arg(F("brightness")));
      } else {
        Display::setBrightness(b);
        WebUI::closeConnection(200, F("Brightness Set"));
      }
    }

    void updateStatus() {
      if (!WebUI::authenticationOK()) { return; }
      wtAppImpl->updateAllData();
      WebUI::redirectHome();
    }

    void updateWeatherConfig() {
      if (!WebUI::authenticationOK()) { return; }
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
    }

    void updatePluginConfig() {
      if (!WebUI::authenticationOK()) { return; }

      String responseType = "text/plain";

      if (!WebUI::hasArg(F("pID"))) {
        Log.warning(F("No plugin ID was supplied"));
        WebUI::sendStringContent(responseType, F("ERR: No plugin ID"));
        return;
      }

      uint8_t pID = WebUI::arg(F("pID")).toInt()-1;
      Plugin *p = wtAppImpl->pluginMgr.getPlugin(pID);
      if (p == NULL) {
        Log.warning(F("Invalid plugin ID: %d"), pID);
        WebUI::sendStringContent(responseType, F("ERR: Invalid plugin ID"));
        return;
      }

      if (!WebUI::hasArg(F("plain"))) {
        Log.warning(F("No payload supplied"));
        WebUI::sendStringContent(responseType, F("ERR: No payload"));
        return;
      }

      String settings = WebUI::arg("plain");
      p->newSettings(settings);        

      WebUI::sendStringContent(responseType, F("OK: settings updated"));
    }

    void updateDisplayConfig() {
      if (!WebUI::authenticationOK()) { return; }

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
    }
  }   // ----- END: WebUIHelper::Endpoints

  // ----- BEGIN: WebUIHelper::Dev
  namespace Dev {
    void reboot() {
      if (!WebUI::authenticationOK()) { return; }
      wtAppImpl->askToReboot();
      WebUI::redirectHome();
    }

    void forceScreen() {
      Log.trace(F("Web Request: /dev/forceScreen"));
      if (!WebUI::authenticationOK()) { return; }
      String screen = WebUI::arg(F("screen"));
      Log.trace(F("/dev/forceScreen?screen=%s"), screen.c_str());
      if (screen.isEmpty()) return;
      ScreenMgr::display(screen);
      WebUI::redirectHome();
    }

    void yieldSettings() {
      Log.trace(F("Web Request: /dev/settings"));
      if (!WebUI::authenticationOK()) { return; }

      DynamicJsonDocument *doc = (WebUI::hasArg("wt")) ? WebThing::settings.asJSON() :
                                                         wtApp->settings->asJSON();
      WebUI::sendJSONContent(doc);
      doc->clear();
      delete doc;
    }

    void yieldScreenShot() {
      Log.trace(F("Web Request: /dev/screenShot"));
      if (!WebUI::authenticationOK()) { return; }

      WebUI::sendArbitraryContent(
          "image/bmp",
          Display::getSizeOfScreenShotAsBMP(),
          Display::streamScreenShotAsBMP);
    }

    void enableDevMenu() {
      if (!WebUI::authenticationOK()) { return; }
      Log.trace("Web Request: /dev/enableDevMenu");

      wtApp->settings->uiOptions.showDevMenu = WebUI::hasArg("showDevMenu");
      wtApp->settings->write();

      WebUI::redirectHome();
    }

    void getDataBrokerValue() {
      Log.trace(F("Web Request: /dev/data"));
      if (!WebUI::authenticationOK()) { return; }
      String key = "$" + WebUI::arg(F("key"));
      String value;
      DataBroker::map(key, value);
      String result = key + ": " + value;
      WebUI::sendStringContent("text", result);
    }
  }   // ----- END: WebUIHelper::Dev


  // ----- BEGIN: WebUIHelper::Default
  namespace Default {
    void devPage() {
      Log.trace(F("Web Request: Handle Dev Configure"));
      if (!WebUI::authenticationOK()) { return; }

      auto mapper =[](String &key) -> String {
        ((void)key);
        return WTBasics::EmptyString;
      };

      WebUI::startPage();
      templateHandler->send("/wta/ConfigDev.html", mapper);
      WebUI::finishPage();
    }

    void updateDevConfig() {
      if (!WebUI::authenticationOK()) { return; }

      WebUI::redirectHome();
    }

    void homePage() {
      Log.trace(F("Web Request: Display Home Page"));
      if (!WebUI::authenticationOK()) { return; }

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
    }
  }  // ----- END: WebUIHelper::Default

  namespace Pages {
    void presentWeatherConfig() {
      Log.trace(F("Web Request: Handle Weather Configure"));
      if (!WebUI::authenticationOK()) { return; }

      String langTarget = "SL" + wtApp->settings->owmOptions.language;

      auto mapper =[langTarget](String &key) -> String {
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

      WebUI::startPage();
      templateHandler->send("/wta/ConfigWeather.html", mapper);
      WebUI::finishPage();
    }


    void presentPluginConfig() {
      Log.trace(F("Web Request: Handle Plugin Configure"));
      if (!WebUI::authenticationOK()) { return; }

      auto mapper =[](String &key) -> String {
        if (key.startsWith("_P")) {
          int i = (key.charAt(2) - '0');
          key.remove(0, 4); // Get rid of the prefix; e.g. _P1_
          Plugin *p = wtAppImpl->pluginMgr.getPlugin(i-1);
          if (p == NULL) return WTBasics::EmptyString;
          if (key.equals(F("IDX"))) return String(i);
          if (key.equals(F("NAME"))) { return p->getName(); }
          if (key.equals(F("FORM"))) { String v; p->getForm(v); return v; }
          if (key.equals(F("VALS"))) { String v; p->getSettings(v); return v; }
        }
        return WTBasics::EmptyString;
      };

      WebUI::startPage();
      templateHandler->send("/wta/ConfigPlugins.html", mapper);
      WebUI::finishPage();
    }

    void presentDisplayConfig() {
      Log.trace(F("Web Request: Handle Display Configure"));
      if (!WebUI::authenticationOK()) { return; }

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

      WebUI::startPage();
      templateHandler->send("/wta/ConfigDisplay.html", mapper);
      WebUI::finishPage();
    }
  }   // ----- END: WebUIHelper::Pages


  void init(String& customActions) {
    String actions = Internal::BASE_ACTIONS + customActions;
    if (wtApp->settings->uiOptions.showDevMenu) {
      actions += Internal::DEV_ACTION;
    }
    WebUI::addMenuItems(actions);

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