/*
 * OLEDTestApp:
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
#include <WebThing.h>
#include <WebUI.h>
#include <DataBroker.h>
#include <gui/Display.h>
#include <gui/ScreenMgr.h>
#include <plugins/PluginMgr.h>
#include <plugins/common/GenericPlugin.h>
#include <plugins/common/BlynkPlugin.h>
#include <plugins/common/CryptoPlugin.h>
//                                  Local Includes
#include "src/hardware/HWConfig.h"
#include "OLEDTestApp.h"
#include "OTSettings.h"
#include "OTWebUI.h"
#include "OTDataSupplier.h"
#include "src/screens/AppTheme.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constants
 *
 *----------------------------------------------------------------------------*/

// CUSTOM: Update these strings for your app:
static const char* VersionString = "0.0.5";
static const char* AppName = "OLEDTest";
static const char* AppPrefix = "OT-";


/*------------------------------------------------------------------------------
 *
 * Private Utility Functions
 *
 *----------------------------------------------------------------------------*/

Plugin* pluginFactory(const String& type) {
  Plugin *p = NULL;

  // CUSTOM: Choose which plugins you'd like to load
  if      (type.equalsIgnoreCase("generic")) { p = new GenericPlugin(); }
  else if (type.equalsIgnoreCase("crypto"))  { p = new CryptoPlugin();  }
  

  if (p == NULL) {
    Log.warning("Unrecognized plugin type: %s", type.c_str());
  }
  return p;
}

/*------------------------------------------------------------------------------
 *
 * Class function to create and start the OLEDTestApp singleton
 *
 *----------------------------------------------------------------------------*/

static OTSettings theSettings;  // Allocate storage for the app settings

void OLEDTestApp::create() {
  // CUSTOM: Perform any low level initialization that may be required, this
  // is often related to initializing hardware
  
  // Set the display options before we fire up the display!
  Display.setDeviceOptions(&hwConfig.displayDeviceOptions);

  // Initialize the synthetic grounds
  for (int i = 0; i < hwConfig.nSyntheticGrounds; i++) {
    uint8_t pin = hwConfig.syntheticGrounds[i];
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  // BOILERPLATE
  PluginMgr::setFactory(pluginFactory);
  OLEDTestApp* app = new OLEDTestApp(&theSettings);

  app->begin();
}


/*------------------------------------------------------------------------------
 *
 * OLEDTestApp Public Functions
 *
 *----------------------------------------------------------------------------*/

OLEDTestApp::OLEDTestApp(OTSettings* settings) :
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

void OLEDTestApp::app_registerDataSuppliers() {
  // BOILERPLATE
  DataBroker::registerMapper(OTDataSupplier::dataSupplier, OTDataSupplier::ERPrefix);
}

void OLEDTestApp::app_initWebUI() {
  // BOILERPLATE
  OTWebUI::init();
}

void OLEDTestApp::app_loop() {
  // CUSTOM: Perform any app-specific periodic activity
  // Note that app_conditionalUpdate() is called for you automatically on a
  // periodic basis, so no need to do that here.

  // In this case, nothing app-specific is required
}

void OLEDTestApp::app_initClients() {
  // CUSTOM: If your app has any app-specific clients, initilize them now
  // In this example, we have one: RateClient

  ScreenMgr.showActivityIcon(AppTheme::Color_UpdatingRates);

  for (int i = 0; i < OTSettings::MaxCurrencies; i++) {
    currencies[i].id = otSettings->currencies[i].id;
    currencies[i].exchangeRate = 1.0;
  }

  rateClient = new RateClient(
      otSettings->rateApiKey, &(currencies[0]), OTSettings::MaxCurrencies);

  ScreenMgr.hideActivityIcon();
}

void OLEDTestApp::app_conditionalUpdate(bool force) {
  // CUSTOM: Update any app-specific clients

  static uint32_t nextTimeForStatus = 0;
  if (millis() > nextTimeForStatus || force) {
    ScreenMgr.showActivityIcon(AppTheme::Color_UpdatingRates);

    rateClient->updateRates();

    ScreenMgr.hideActivityIcon();
    nextTimeForStatus = millis() + otSettings->refreshInterval * 60 * 60 * 1000L;
  }      
}

Screen* OLEDTestApp::app_registerScreens() {
  // CUSTOM: Register any app-specific Screen objects

  splashScreen = new SplashScreen();
  homeScreen = new HomeScreen();

  ScreenMgr.registerScreen("Splash", splashScreen);
  ScreenMgr.registerScreen("Home", homeScreen);

  ScreenMgr.setAsHomeScreen(homeScreen);

  return splashScreen;
}

/*------------------------------------------------------------------------------
 *
 * Optional WTAppImpl virtual functions
 *
 *----------------------------------------------------------------------------*/

void OLEDTestApp::app_registerButtons() {
  // CUSTOM: Register any physical buttons that are connected
  for (int i = 0; i < hwConfig.nPhysicalButtons; i++) {
    uint8_t pin = hwConfig.physicalButtons[i];
    WebThing::buttonMgr.addButton(pin);
  }
}
