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
#include <WTBasics.h>
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
    const __FlashStringHelper* APP_MENU_ITEMS = FPSTR(
      "<a class='w3-bar-item w3-button' href='/presentCurrencyConfig'>"
      "<i class='fa fa-cog'></i> Set Currencies</a>");
  } // ----- END: CMWebUI::Internal

  // ----- BEGIN: CMWebUI::Pages
  namespace Pages {
    void currencyPage() {
      auto mapper =[](const String &key, String& val) -> void {
        if (key.startsWith("_C")) {
          // "key" is of the form: _CN_subkey, where N is a digit
          int i = (key.charAt(2) - '0');
          const char* subkey = &(key.c_str()[4]);
          if (strcmp(subkey, "ID") == 0) val = cmSettings->currencies[i].id;
          else if (strcmp(subkey, "NICK") == 0) val = cmSettings->currencies[i].nickname;
        }
        else if (key.equals(F("RFRSH"))) val.concat(cmSettings->refreshInterval);
        else if (key.equals(F("ER_KEY"))) val = cmSettings->rateApiKey;
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
    WebUIHelper::init(Internal::APP_MENU_ITEMS);

    // Standard pages and endpoints
    WebUI::registerHandler("/",                     WebUIHelper::Default::homePage);
    WebUI::registerHandler("/dev",                  WebUIHelper::Default::devPage);
    WebUI::registerHandler("/dev/updateSettings",   WebUIHelper::Default::updateDevConfig);

    // CUSTOM: App-specific pages and endpoints
    WebUI::registerHandler("/presentCurrencyConfig",Pages::currencyPage);
    WebUI::registerHandler("/updateCurrencyConfig", Endpoints::updateCurrencyConfig);
  }

}
// ----- END: CMWebUI
