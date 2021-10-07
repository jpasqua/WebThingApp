/*
 * OTWebUI:
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
#include "OLEDTestApp.h"
#include "OTWebUI.h"
//--------------- End:    Includes ---------------------------------------------


// ----- BEGIN: OTWebUI namespace
namespace OTWebUI {

  namespace Internal {
    const __FlashStringHelper* APP_MENU_ITEMS = FPSTR(
      "<a class='w3-bar-item w3-button' href='/presentCurrencyConfig'>"
      "<i class='fa fa-cog'></i> Set Currencies</a>");
  } // ----- END: OTWebUI::Internal

  // ----- BEGIN: OTWebUI::Pages
  namespace Pages {
    void currencyPage() {
      auto mapper =[](const String& key, String& val) -> void {
        if (key.startsWith("_C")) {
          // "key" is of the form: _CN_subkey, where N is a digit
          int i = (key.charAt(2) - '0');
          const char* subkey = &(key.c_str()[4]);
          if (strcmp(subkey, "ID") == 0) val = otSettings->currencies[i].id;
          else if (strcmp(subkey, "NICK") == 0) val = otSettings->currencies[i].nickname;
        }
        else if (key.equals(F("RFRSH"))) val.concat(otSettings->refreshInterval);
        else if (key.equals(F("ER_KEY"))) val = otSettings->rateApiKey;
      };

      WebUI::wrapWebPage("/presentCurrencyConfig", "/ConfigCurrency.html", mapper);
    }
  } // ----- END: OTWebUI::Pages


  // ----- BEGIN: OTWebUI::Endpoints
  namespace Endpoints {
    void updateCurrencyConfig() {
      auto action = []() {
        for (int i = 0; i < OTSettings::MaxCurrencies; i++) {
          String prefix = "_c" + String(i) + "_";
          otSettings->currencies[i].id = WebUI::arg(prefix + "id");
          otSettings->currencies[i].nickname = WebUI::arg(prefix + "nick");
        }

        uint32_t refreshInHours = WebUI::arg(F("refreshInterval")).toInt();
        otSettings->refreshInterval = max<uint32_t>(refreshInHours, OTSettings::MinRefreshInterval);
        otSettings->rateApiKey = WebUI::arg("erKey");

        wtApp->settings->write();

        // Act on changed settings...
        wtAppImpl->configMayHaveChanged();
        WebUI::redirectHome();
      };

      WebUI::wrapWebAction("/updateCurrencyConfig", action);
    }
  }   // ----- END: OTWebUI::Endpoints


  void init() {
    WebUIHelper::init(Internal::APP_MENU_ITEMS);

    WebUI::registerHandler("/",                       WebUIHelper::Default::homePage);
    WebUI::registerHandler("/presentCurrencyConfig",  Pages::currencyPage);
    WebUI::registerHandler("/updateCurrencyConfig",   Endpoints::updateCurrencyConfig);
  }

}
// ----- END: OTWebUI
