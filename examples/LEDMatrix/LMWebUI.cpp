/*
 * LMWebUI:
 *    Implements a simple WebUI that displays and updates settings
 *
 */


//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <WebThing.h>
#include <WebUI.h>
#include <WebUIHelper.h>
#include <gui/Display.h>
//                                  Local Includes
#include "LEDMatrixApp.h"
#include "LMWebUI.h"
//--------------- End:    Includes ---------------------------------------------


// ----- BEGIN: LMWebUI namespace
namespace LMWebUI {

  namespace Internal {
    const __FlashStringHelper* APP_MENU_ITEMS = FPSTR(
      "<a class='w3-bar-item w3-button' href='/presentLMConfig'>"
      "<i class='fa fa-cog'></i> Configure LEDMatrix</a>"
      "<a class='w3-bar-item w3-button' href='/presentScreenConfig'>"
      "<i class='fa fa-window-restore'></i> Configure Screens</a>");
  } // ----- END: LMWebUI::Internal

  // ----- BEGIN: LMWebUI::Pages
  namespace Pages {
    void presentLMconfig() {
      auto mapper =[&](const String &key, String& val) -> void {
        if (key == "SCROLL_DELAY")    val = lmSettings->scrollDelay;
        else if (key == "AIO_KEY")    val = lmSettings->aio.key;
        else if (key == "AIO_USER")   val = lmSettings->aio.username;
        else if (key == "AIO_GROUP")  val = lmSettings->aio.groupName;
        else if (key == "USE_METRIC") val = WebUI::checkedOrNot[lmSettings->uiOptions.useMetric];
        else if (key == "USE_24HOUR") val = WebUI::checkedOrNot[lmSettings->uiOptions.use24Hour];
        else if (key == "LAT")        val = WebThing::settings.latAsString();
        else if (key == "LNG")        val = WebThing::settings.lngAsString();
        else if (key == "GMAPS_KEY")  val = WebThing::settings.googleMapsKey;
        else if (key.equals(F("CITYID"))) {
          if (wtApp->settings->owmOptions.enabled) val = wtApp->settings->owmOptions.cityID;
          else val.concat("5380748");  // Palo Alto, CA, USA
        }
        else if (key.equals(F("WEATHER_KEY"))) val = wtApp->settings->owmOptions.key;
        else if (key.equals(F("UNITS"))) val = wtApp->settings->uiOptions.useMetric ? "metric" : "imperial";
        else if (key == "VLTG") {
          float voltage = WebThing::measureVoltage();
          if (voltage == -1) val = "N/A";
          else val = (String(voltage, 2) + "V");
        }
      };

      WebUI::wrapWebPage("/presentLMconfig", "/ConfigForm.html", mapper);
    }
  } // ----- END: LMWebUI::Pages


  // ----- BEGIN: LMWebUI::Endpoints
  namespace Endpoints {
    // Handler for the "/updatePHConfig" endpoint. This is invoked as the target
    // of the form presented by "/displayLMConfig". It updates the values of the
    // corresponding settings and writes the settings to EEPROM.
    //
    // TO DO: Call a function to let the main app know that settings may have changed
    //        so that it can take any appropriate actions
    //
    // Form:
    //    GET /updatePHConfig?description=DESC&iBright=INT&...
    //
    void updateLMConfig() {
      auto action = []() {
        lmSettings->aio.key = WebUI::arg("aioKey");
        lmSettings->aio.username = WebUI::arg("aioUsername");
        lmSettings->aio.groupName = WebUI::arg("aioGroup");
        lmSettings->uiOptions.useMetric = WebUI::hasArg(F("metric"));
        lmSettings->uiOptions.use24Hour = WebUI::hasArg(F("is24hour"));
        lmSettings->scrollDelay = WebUI::arg("scrollDelay").toInt();
        lmSettings->write();

        WebUI::redirectHome();
      };

      WebUI::wrapWebAction("/updateLMConfig", action, false);
    }
  }   // ----- END: LMWebUI::Endpoints


  void init() {
    WebUIHelper::init(Internal::APP_MENU_ITEMS);

    WebUI::registerHandler("/",                       WebUIHelper::Default::homePage);
    WebUI::registerHandler("/presentLMConfig",        Pages::presentLMconfig);

    WebUI::registerHandler("/updateLMConfig",         Endpoints::updateLMConfig);

  }

}
// ----- END: LMWebUI
