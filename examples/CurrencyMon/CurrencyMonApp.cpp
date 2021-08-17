/*
 * CurrencyMonApp:
 *    A simple example of a WebThingApp that display times and currency
 *    exchange rates.
 *
 * CUSTOMIZATION:
 * o To create your own app based on this sample, search for 'CUSTOM'
 *   to find areas that should be changed/customized
 * o Areas labeled 'BOILERPLATE' may require mechanical changes such as updating
 *   the names you have used for classes and variables
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <WebUI.h>
#include <DataBroker.h>
#include <gui/ScreenMgr.h>
#include <plugins/PluginMgr.h>
#include <plugins/common/GenericPlugin.h>
#include <plugins/common/BlynkPlugin.h>
#include <plugins/common/CryptoPlugin.h>
//                                  Local Includes
#include "CurrencyMonApp.h"
#include "CMSettings.h"
#include "CMWebUI.h"
#include "CMDataSupplier.h"
#include "src/screens/AppTheme.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constants
 *
 *----------------------------------------------------------------------------*/

// CUSTOM: Update these strings for your app:
static const String VersionString = "0.1.0";
static const String AppName = "CurrencyMon";
static const String AppPrefix = "CM-";


/*------------------------------------------------------------------------------
 *
 * Private Utility Functions
 *
 *----------------------------------------------------------------------------*/

Plugin* pluginFactory(String& type) {
  Plugin *p = NULL;

  // CUSTOM: Choose which plugins you'd like to load
  if      (type.equalsIgnoreCase("generic")) { p = new GenericPlugin(); }
  else if (type.equalsIgnoreCase("blynk"))   { p = new BlynkPlugin();   }
  // else if (type.equalsIgnoreCase("crypto"))  { p = new CryptoPlugin();  }
  

  if (p == NULL) {
    Log.warning("Unrecognized plugin type: %s", type.c_str());
  }
  return p;
}

/*------------------------------------------------------------------------------
 *
 * Class function to create and start the CurrencyMonApp singleton
 *
 *----------------------------------------------------------------------------*/

static CMSettings theSettings;  // Allocate storage for the app settings

void CurrencyMonApp::create() {
  // BOILERPLATE
  PluginMgr::setFactory(pluginFactory);
  CurrencyMonApp* app = new CurrencyMonApp(&theSettings);

  app->begin();

  // CUSTOM: Perform any app-specific initialization here
  // In this case, nothing app-specific is required
}


/*------------------------------------------------------------------------------
 *
 * CurrencyMonApp Public Functions
 *
 *----------------------------------------------------------------------------*/

CurrencyMonApp::CurrencyMonApp(CMSettings* settings) :
    WTAppImpl(AppName, AppPrefix, VersionString, settings)
{
  // CUSTOM: Perform any object initialization here
  // In this case, nothing app-specific is required
}


/*------------------------------------------------------------------------------
 *
 * Mandatory WTAppImpl virtual functions
 *
 *----------------------------------------------------------------------------*/

void CurrencyMonApp::app_registerDataSuppliers() {
  // BOILERPLATE
  DataBroker::registerMapper(CMDataSupplier::dataSupplier, CMDataSupplier::ERPrefix);
}

void CurrencyMonApp::app_initWebUI() {
  // BOILERPLATE
  CMWebUI::init();
}

void CurrencyMonApp::app_loop() {
  // CUSTOM: Perform any app-specific periodic activity
  // Note that app_conditionalUpdate() is called for you automatically on a
  // periodic basis, so no need to do that here.

  // In this case, nothing app-specific is required
}

void CurrencyMonApp::app_initClients() {
  // CUSTOM: If your app has any app-specific clients, initilize them now
  // In this example, we have one: RateClient

  ScreenMgr::showUpdatingIcon(AppTheme::Color_UpdatingRates);

  for (int i = 0; i < CMSettings::MaxCurrencies; i++) {
    currencies[i].id = cmSettings->currencies[i].id;
    currencies[i].exchangeRate = 1.0;
  }

  rateClient = new RateClient(
      cmSettings->rateApiKey, &(currencies[0]), CMSettings::MaxCurrencies);

  ScreenMgr::hideUpdatingIcon();
}

void CurrencyMonApp::app_conditionalUpdate(bool force) {
  // CUSTOM: Update any app-specific clients

  static uint32_t nextTimeForStatus = 0;
  if (millis() > nextTimeForStatus || force) {
    ScreenMgr::showUpdatingIcon(AppTheme::Color_UpdatingRates);

    rateClient->updateRates();

    ScreenMgr::hideUpdatingIcon();
    nextTimeForStatus = millis() + cmSettings->refreshInterval * 60 * 60 * 1000L;
  }      
}

Screen* CurrencyMonApp::app_registerScreens() {
  // CUSTOM: Register any app-specific Screen objects

	numpad = new EnterNumberScreen();
  splashScreen = new SplashScreen();
  timeScreen = new TimeScreen();

  ScreenMgr::registerScreen("Splash", splashScreen);
  ScreenMgr::registerScreen("Time", timeScreen);

  ScreenMgr::setAsHomeScreen(timeScreen);

  return splashScreen;
}
