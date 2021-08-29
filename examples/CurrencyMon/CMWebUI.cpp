/*
 * CMWebUI:
 *    Implements a simple WebUI that displays and updates settings
 *
 */


//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <WebThingBasics.h>
#include <WebThing.h>
#include <WebUI.h>
#include <WebUIHelper.h>
#include <gui/Display.h>
//                                  Local Includes
#include "CurrencyMonApp.h"
#include "CMWebUI.h"
//--------------- End:    Includes ---------------------------------------------


// ----- BEGIN: CMWebUI namespace
namespace CMWebUI {

  namespace Internal {
    const char APP_MENU_ITEMS[] PROGMEM =
      "<a class='w3-bar-item w3-button' href='/presentCurrencyConfig'>"
      "<i class='fa fa-cog'></i> Set Currencies</a>";
  } // ----- END: CMWebUI::Internal

  // ----- BEGIN: CMWebUI::Pages
  namespace Pages {
    void currencyPage() {
      auto mapper =[](String &key) -> String {
        if (key.startsWith("_C")) {
          int i = (key.charAt(2) - '0');
          key.remove(0, 4); // Get rid of the prefix; e.g. _C1_
          if (key.equals(F("ID"))) return  cmSettings->currencies[i].id;
          if (key.equals(F("NICK"))) return  cmSettings->currencies[i].nickname;
        }
        if (key.equals(F("RFRSH"))) return String(cmSettings->refreshInterval);
        if (key.equals(F("ER_KEY"))) return String(cmSettings->rateApiKey);
        return WTBasics::EmptyString;
      };

      WebUIHelper::wrapWebPage("/presentCurrencyConfig", "/ConfigCurrency.html", mapper);
    }
  } // ----- END: CMWebUI::Pages


  // ----- BEGIN: CMWebUI::Endpoints
  namespace Endpoints {
    void updateCurrencyConfig() {
      auto action = []() {
        for (int i = 0; i < CMSettings::MaxCurrencies; i++) {
          String prefix = "_c" + String(i) + "_";
          cmSettings->currencies[i].id = WebUI::arg(prefix + "id");
          cmSettings->currencies[i].nickname = WebUI::arg(prefix + "nick");
        }

        uint32_t refreshInHours = WebUI::arg(F("refreshInterval")).toInt();
        cmSettings->refreshInterval = max<uint32_t>(refreshInHours, CMSettings::MinRefreshInterval);
        cmSettings->rateApiKey = WebUI::arg("erKey");

        wtApp->settings->write();

        // Act on changed settings...
        wtAppImpl->configMayHaveChanged();
        WebUI::redirectHome();
      };

      WebUIHelper::wrapWebAction("/updateCurrencyConfig", action);
    }
  }   // ----- END: CMWebUI::Endpoints


  void init() {
WebThing::genHeapStatsRow("before WebUIHelper::init");
    WebUIHelper::init(Internal::APP_MENU_ITEMS);

WebThing::genHeapStatsRow("before registering endpoints");
    // Standard pages and endpoints
    WebUI::registerHandler("/",                     WebUIHelper::Default::homePage);
    WebUI::registerHandler("/dev",                  WebUIHelper::Default::devPage);
    WebUI::registerHandler("/dev/updateSettings",   WebUIHelper::Default::updateDevConfig);

    // CUSTOM: App-specific pages and endpoints
    WebUI::registerHandler("/presentCurrencyConfig",Pages::currencyPage);
    WebUI::registerHandler("/updateCurrencyConfig", Endpoints::updateCurrencyConfig);
WebThing::genHeapStatsRow("Exiting CMWebUI::init");
  }

}
// ----- END: CMWebUI
