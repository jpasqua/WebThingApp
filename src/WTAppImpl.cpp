/*
 * WTAppImpl
 *    Generic App Framework for WebThings
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <GenericESP.h>
#include <WebThing.h>
#include <WebUI.h>
#include <DataBroker.h>
//                                  Local Includes
#include "WTAppImpl.h"
#include "gui/Theme.h"
#include "gui/ScreenMgr.h"
//--------------- End:    Includes ---------------------------------------------


const String WTAppImpl::SettingsFileName = "/settings.json";

/*------------------------------------------------------------------------------
 *
 * Public Functions
 *
 *----------------------------------------------------------------------------*/


// ----- Utility functions for the rest of the app

void WTAppImpl::askToReboot() {
  ScreenMgr::display(rebootScreen);
}

void WTAppImpl::saveSettings() {
  settings->write();
}

void WTAppImpl::configMayHaveChanged() {
  // TO DO: Implement me!!
}

void WTAppImpl::weatherConfigMayHaveChanged() {
  // TO DO: Implement me!!
}

void WTAppImpl::conditionalUpdate(bool force) {
  app_conditionalUpdate(force);
  refreshWeatherData(force);
  pluginMgr.refreshAll(force);
}

void WTAppImpl::updateAllData() {
  conditionalUpdate(true);
}

WTAppImpl::WTAppImpl(const String& name, const String& prefix, const String& version, WTAppSettings* appSettings) :
  	WTApp(name, prefix, version, appSettings)
{
  // Nothing to do here...
}

void WTAppImpl::begin() {
WebThing::genHeapStatsRow("Entering WTAppImpl::begin");
  WebThing::preSetup();                 // Must be first

WebThing::genHeapStatsRow("before reading settings");
  settings->init(SettingsFileName);
  settings->read();
  settings->logSettings();

WebThing::genHeapStatsRow("before DataBroker");
  DataBroker::begin();
  DataBroker::registerMapper( ([this](const String& key, String& value){ weatherDataSupplier(key, value); }), 'W' );
  app_registerDataSuppliers();

WebThing::genHeapStatsRow("before ScreenMgr setup");
  ScreenMgr::setup(&settings->uiOptions, &settings->displayOptions);
WebThing::genHeapStatsRow("before registerScreens");
  registerScreens();
WebThing::genHeapStatsRow("before display wifiScreen");
  ScreenMgr::display(wifiScreen);

WebThing::genHeapStatsRow("before prepWebThing");
  prepWebThing();
WebThing::genHeapStatsRow("before app_initWebUI");
  app_initWebUI();

WebThing::genHeapStatsRow("before splashscreen");
  if (splashScreen) ScreenMgr::display(splashScreen);

WebThing::genHeapStatsRow("before initclients");
  app_initClients();
  prepWeather();

WebThing::genHeapStatsRow("before loading plugins");
  pluginMgr.loadAll("/plugins");

WebThing::genHeapStatsRow("before conditionalUpdate");
  conditionalUpdate(true);

WebThing::genHeapStatsRow("before HomeScreen");
  ScreenMgr::displayHomeScreen();

WebThing::genHeapStatsRow("before postSetup");
  WebThing::postSetup();                // Must be last

WebThing::genHeapStatsRow("Exiting WTAppImpl::begin");
}

void WTAppImpl::loop() {
  app_loop();

  conditionalUpdate(false);

  WebThing::loop();
  ScreenMgr::loop();

  static uint32_t nextStats = 0;
  if (millis() > nextStats) {
    Log.verbose(F("Heap: free=%d, frag=%d%%"), ESP.getFreeHeap(), GenericESP::getHeapFragmentation());
    nextStats = millis() + 60 * 1000L;
  }
}


/*------------------------------------------------------------------------------
 *
 * Protected Functions
 *
 *----------------------------------------------------------------------------*/

// ----- Weather Functions
void WTAppImpl::refreshWeatherData(bool force) {
  static const uint32_t ForecastInterval = 8 * 60 * 60 * 1000L; // 8 Hours

  if (!settings->owmOptions.enabled) return;

  static uint32_t lastWeatherUpdate = 0;
  static uint32_t lastForecastUpdate = 0;
  uint32_t curMillis = millis();
  uint32_t threshold = (WebThing::settings.processingInterval * 60 * 1000L);

  if (force || ((curMillis - lastWeatherUpdate) > threshold)) {
    ScreenMgr::showUpdatingIcon(Theme::Color_UpdatingWeather);
    owmClient->update();
    lastWeatherUpdate = curMillis;
  }
  if (force || ((curMillis - lastForecastUpdate) > ForecastInterval)) {
    ScreenMgr::showUpdatingIcon(Theme::Color_UpdatingWeather);
    owmClient->updateForecast(WebThing::getGMTOffset());
    lastForecastUpdate = curMillis;
  }
  ScreenMgr::hideUpdatingIcon();
}

void WTAppImpl::prepWeather() {
  if (settings->owmOptions.enabled) {
    if (settings->owmOptions.key.isEmpty()) { settings->owmOptions.enabled = false; }
    else {
      if (owmClient != NULL) { /* TO DO: Do any necessary cleanup */ }
      owmClient = new OWMClient(
        settings->owmOptions.key, settings->owmOptions.cityID, settings->uiOptions.useMetric, settings->owmOptions.language);
    }
  } else {
    owmClient = NULL;
  }
}

void WTAppImpl::weatherDataSupplier(const String& key, String& value) {
  if (owmClient == NULL) return;
  if (key.equalsIgnoreCase("temp")) value +=  owmClient->weather.readings.temp;
  else if (key.equalsIgnoreCase("city")) value += owmClient->weather.location.city;
  else if (key.equalsIgnoreCase("desc")) value += owmClient->weather.description.basic;
  else if (key.equalsIgnoreCase("ldesc")) value += owmClient->weather.description.longer;
}


// ----- WebThing Functions
void WTAppImpl::setTitle() {
  if (WebThing::settings.hostname.isEmpty() || WebThing::settings.hostname.startsWith(appPrefix))
    WebUI::setTitle("MultiMon (" + WebThing::settings.hostname + ")");
  else
    WebUI::setTitle(WebThing::settings.hostname);
}

void WTAppImpl::baseConfigChange() {
  setTitle();
}

void WTAppImpl::configModeCallback(String &ssid, String &ip) {
  (void)ip; // We don't use this parameter - avoid a warning
  configScreen->setSSID(ssid);
  ScreenMgr::display(configScreen);
}

void WTAppImpl::prepWebThing() {
  // Some of the MultiMonitor hardware configuration is fixed, so the corresponding settings
  // should have specific values. Rather than make the user configure them, set them to the
  // right values here.
  WebThing::settings.hasVoltageSensing = false;
  WebThing::settings.sleepOverridePin = WebThingSettings::NoPinAssigned;
  WebThing::displayPowerOptions(false);               // Don't let the user fool with this.
  WebThing::replaceEmptyHostname(appPrefix.c_str());

  WebThing::notifyOnConfigMode([this](String &ssid, String &ip) { configModeCallback(ssid, ip); });
  WebThing::notifyConfigChange([this]() { baseConfigChange(); } );

WebThing::genHeapStatsRow("before WebThing::setup");
  WebThing::setup();

WebThing::genHeapStatsRow("before WebThing::setDisplayedVersion");
  WebThing::setDisplayedVersion(appPrefix + appVersion);
WebThing::genHeapStatsRow("before setTitle");
  setTitle();
}


/*------------------------------------------------------------------------------
 *
 * Private Functions
 *
 *----------------------------------------------------------------------------*/

void WTAppImpl::registerScreens() {
  // CUSTOM: To avoid loading a screen, comment out the corresponding pair of lines below.

  calibrationScreen = new CalibrationScreen();
  ScreenMgr::registerScreen("Calibration", calibrationScreen);

  configScreen = new ConfigScreen();
  ScreenMgr::registerScreen("Config", configScreen);

  forecastScreen = new ForecastScreen();
  ScreenMgr::registerScreen("Forecast", forecastScreen);

  rebootScreen = new RebootScreen();
  ScreenMgr::registerScreen("Reboot", rebootScreen);

  weatherScreen = new WeatherScreen();
  ScreenMgr::registerScreen("Weather", weatherScreen);

  wifiScreen = new WiFiScreen();
  ScreenMgr::registerScreen("WiFi", wifiScreen);

  utilityScreen = new UtilityScreen();
  ScreenMgr::registerScreen("Utility", utilityScreen);

  splashScreen = app_registerScreens();
}