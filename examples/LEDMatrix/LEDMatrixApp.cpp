/*
 * LEDMatrixApp:
 *    A simple example showing how to use the Matrix display class.
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
#include <plugins/common/AIOPlugin.h>
#include <plugins/common/CryptoPlugin.h>
//                                  Local Includes
#include "src/hardware/HWConfig.h"
#include "LEDMatrixApp.h"
#include "LMSettings.h"
#include "LMWebUI.h"
#include "LMDataSupplier.h"
#include "src/screens/AppTheme.h"
//--------------- End:    Includes ---------------------------------------------


/*------------------------------------------------------------------------------
 *
 * Constants
 *
 *----------------------------------------------------------------------------*/

// CUSTOM: Update these strings for your app:
static const char* VersionString = "0.0.6";
static const char* AppName = "LEDMatrix";
static const char* AppPrefix = "LM-";


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
  else if (type.equalsIgnoreCase("aio"))  { p = new AIOPlugin();  }
  

  if (p == NULL) {
    Log.warning("Unrecognized plugin type: %s", type.c_str());
  }
  return p;
}

/*------------------------------------------------------------------------------
 *
 * Class function to create and start the LEDMatrixApp singleton
 *
 *----------------------------------------------------------------------------*/

static LMSettings theSettings;  // Allocate storage for the app settings

void LEDMatrixApp::create() {
  // CUSTOM: Perform any low level initialization that may be required, this
  // is often related to initializing hardware
  
  // Set the display options before we fire up the display!
  Display.setDeviceOptions(&hwConfig.displayDeviceOptions);

  // BOILERPLATE
  PluginMgr::setFactory(pluginFactory);
  LEDMatrixApp* app = new LEDMatrixApp(&theSettings);

  app->begin();
}


/*------------------------------------------------------------------------------
 *
 * LEDMatrixApp Public Functions
 *
 *----------------------------------------------------------------------------*/

LEDMatrixApp::LEDMatrixApp(LMSettings* settings) :
    WTAppImpl(AppName, AppPrefix, VersionString, settings)
{
  // CUSTOM: Perform any object initialization here
  // In this case, nothing app-specific is required
}

void LEDMatrixApp::printerWasActivated(int index) {
  if (lmSettings->printMonitorEnabled) {
  // TO DO: May need to cache the IP!!!
    printerGroup->activatePrinter(index);
  }
}


/*------------------------------------------------------------------------------
 *
 * Mandatory WTAppImpl virtual functions
 *
 *----------------------------------------------------------------------------*/

void LEDMatrixApp::app_registerDataSuppliers() {
  // BOILERPLATE
  DataBroker::registerMapper(LMDataSupplier::dataSupplier, LMDataSupplier::ERPrefix);
  if (lmSettings->printMonitorEnabled) {
    DataBroker::registerMapper(
        [this](const String& key,String& val) { this->printerGroup->dataSupplier(key, val); },
        PrinterGroup::DataProviderPrefix);
  }
}

void LEDMatrixApp::app_initWebUI() {
  // BOILERPLATE
  LMWebUI::init();
}

void LEDMatrixApp::app_loop() {
  // CUSTOM: Perform any app-specific periodic activity
  // Note that app_conditionalUpdate() is called for you automatically on a
  // periodic basis, so no need to do that here.

  // In this case, nothing app-specific is required
}

void LEDMatrixApp::app_initClients() {
  // CUSTOM: If your app has any app-specific clients, initilize them now

  fireUpPrintMonitor();

  // ScreenMgr.showActivityIcon(Theme::Color_WHITE);
  // Perform potentially long running actions here...
  // ScreenMgr.hideActivityIcon();
}

void LEDMatrixApp::app_conditionalUpdate(bool force) {
  // CUSTOM: Update any app-specific clients

  if (lmSettings->printMonitorEnabled) {
    printerGroup->refreshPrinterData(force);
  }
}

Screen* LEDMatrixApp::app_registerScreens() {
  // CUSTOM: Register any app-specific Screen objects
  splashScreen = new SplashScreen();
  homeScreen = new HomeScreen();
  nextPrinterScreen = new NextPrinterScreen();
  allPrinterScreen = new AllPrinterScreen();
  
  ScreenMgr.registerScreen("Splash", splashScreen, true);
  ScreenMgr.registerScreen("Home", homeScreen);
  ScreenMgr.registerScreen("NextPrint", nextPrinterScreen);
  ScreenMgr.registerScreen("AllPrints", allPrinterScreen);
  ScreenMgr.setAsHomeScreen(homeScreen);

  // CUSTOM: Associate a confirm/cancel buttons with the reboot screen
  screens.rebootScreen->setButtons(hwConfig.advanceButton, hwConfig.previousButton);

  // CUSTOM: Add a sequence of screens that the user can cycle through
  auto& sequence = ScreenMgr.sequence;
  sequence.push_back(homeScreen);
  sequence.push_back(wtAppImpl->screens.weatherScreen);
  sequence.push_back(wtAppImpl->screens.forecastScreen);
  // Add any plugins to the sequence
  uint8_t nPlugins = pluginMgr.getPluginCount();
  for (int i = 0; i < nPlugins; i++) {
    Plugin* p = pluginMgr.getPlugin(i);
    sequence.push_back(p->getFlexScreen());
  }
  sequence.push_back(wtAppImpl->screens.infoScreen);

  ScreenMgr.reconcileScreenSequence(lmSettings->screenSettings);

  return splashScreen;
}

/*------------------------------------------------------------------------------
 *
 * Optional WTAppImpl virtual functions
 *
 *----------------------------------------------------------------------------*/

void LEDMatrixApp::app_configureHW() {
  // CUSTOM: Register any physical buttons that are connected

  // Initialize the synthetic grounds
  for (int i = 0; i < hwConfig.nSyntheticGrounds; i++) {
    Basics::Pin pin = hwConfig.syntheticGrounds[i];
    if (pin != Basics::UnusedPin) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);      
    }
  }

  // Initialize the buttons
  for (int i = 0; i < hwConfig.nPhysicalButtons; i++) {
    Basics::Pin pin = hwConfig.physicalButtons[i];
    if (pin != Basics::UnusedPin) {
      WebThing::buttonMgr.addButton(pin);
    }
  }

  ScreenMgr.setSequenceButtons(hwConfig.advanceButton, hwConfig.previousButton);
}

/*------------------------------------------------------------------------------
 *
 * LEDMatrixApp Private Functions
 *
 *----------------------------------------------------------------------------*/

void LEDMatrixApp::showPrinterActivity(bool busy) {
 if (busy) ScreenMgr.showActivityIcon(Theme::Color_WHITE);
 else ScreenMgr.hideActivityIcon();
}

void LEDMatrixApp::fireUpPrintMonitor() {
  if (lmSettings->printMonitorEnabled) {
    printerGroup = new PrinterGroup(
      MaxPrinters, lmSettings->printer,
      lmSettings->printerRefreshInterval,
      [this](bool busy){this->showPrinterActivity(busy);});
    for (int i = 0; i < MaxPrinters; i++) {
      printerGroup->activatePrinter(i);
    }
  }
}
