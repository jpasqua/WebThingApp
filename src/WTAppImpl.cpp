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
#include <Output.h>
#include <WebThing.h>
#include <WebUI.h>
#include <DataBroker.h>
//                                  Local Includes
#include "WTAppImpl.h"
#include "gui/Theme.h"
#include "gui/ScreenMgr.h"
//--------------- End:    Includes ---------------------------------------------


constexpr const char* SettingsFileName = "/settings.json";

/*------------------------------------------------------------------------------
 *
 * Public Functions
 *
 *----------------------------------------------------------------------------*/


// ----- Utility functions for the rest of the app

void WTAppImpl::askToReboot() {
  ScreenMgr.display(screens.rebootScreen);
}

void WTAppImpl::saveSettings() {
  settings->write();
}

void WTAppImpl::configMayHaveChanged() {
  // TO DO: Implement me!!
}

void WTAppImpl::weatherConfigMayHaveChanged() {
  owmClient->setUnits(wtApp->settings->uiOptions.useMetric);
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

void WTAppImpl::begin(
    bool respectPowerSettings,
    Basics::Pin SDA, Basics::Pin SCL,
    bool startWithInfoScreen)
{
  WebThing::preSetup(SDA, SCL);                 // Must be first

  settings->init(SettingsFileName);
  settings->read();
  settings->logSettings();
  Output::setOptions(&(settings->uiOptions.useMetric), &(settings->uiOptions.use24Hour));

  app_configureHW();

  pluginMgr.loadAll("/plugins");
  DataBroker::registerMapper( ([this](const String& key, String& value){ weatherDataSupplier(key, value); }), 'W' );
  app_registerDataSuppliers();

  ScreenMgr.setup(&settings->uiOptions, &settings->displayOptions);
  screens.registerScreens();
  screens.splashScreen = app_registerScreens();

  ScreenMgr.display(screens.wifiScreen);

  prepWebThing(respectPowerSettings);
  app_initWebUI();

  if (screens.splashScreen) ScreenMgr.display(screens.splashScreen);

  app_initClients();
  prepWeather();

  conditionalUpdate(true);

  if (startWithInfoScreen) ScreenMgr.displayInfoScreen();
  else ScreenMgr.displayHomeScreen();

  WebThing::postSetup();                // Must be last

}

void WTAppImpl::loop() {
  app_loop();

  conditionalUpdate(false);

  WebThing::loop();
  ScreenMgr.loop();

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
  if (!settings->owmOptions.enabled) return;

  static uint32_t lastWeatherUpdate = 0;
  static uint32_t lastForecastUpdate = 0;
  uint32_t curMillis = millis();
  uint32_t threshold = Basics::minutesToMS(settings->owmOptions.refresh);
  uint32_t fcstThreshold = Basics::hoursToMS(settings->owmOptions.fcstRefresh);

  if (force || ((curMillis - lastWeatherUpdate) > threshold)) {
    ScreenMgr.showActivityIcon(Theme::Color_UpdatingWeather);
    owmClient->update();
    lastWeatherUpdate = curMillis;
  }
  if (force || ((curMillis - lastForecastUpdate) > fcstThreshold)) {
    ScreenMgr.showActivityIcon(Theme::Color_UpdatingWeather);
    owmClient->updateForecast(WebThing::getGMTOffset());
    lastForecastUpdate = curMillis;
  }
  ScreenMgr.hideActivityIcon();
}

void WTAppImpl::prepWeather() {
  if (settings->owmOptions.key.isEmpty()) { settings->owmOptions.enabled = false; }
  if (settings->owmOptions.enabled) {
    if (owmClient != nullptr) { /* TO DO: Do any necessary cleanup */ }
    owmClient = new OWMClient(
      settings->owmOptions.key, settings->owmOptions.cityID, settings->uiOptions.useMetric, settings->owmOptions.language);
  } else {
    owmClient = nullptr;
  }
}

void WTAppImpl::weatherDataSupplier(const String& key, String& value) {
  if (owmClient == nullptr) return;
  if (key.equalsIgnoreCase("temp")) value +=  owmClient->weather.readings.temp;
  else if (key.equalsIgnoreCase("desc")) value += owmClient->weather.description.basic;
  else if (key.equalsIgnoreCase("ldesc")) value += owmClient->weather.description.longer;
  else if (key.equalsIgnoreCase("wind")) {
      value += (int)(owmClient->weather.readings.windSpeed);
      value += owmClient->dirFromDeg(owmClient->weather.readings.windDeg);
  } else if (key.equalsIgnoreCase("pressure")) {
    value += owmClient->weather.readings.pressure;
  } else if (key.equalsIgnoreCase("humidity")) {
    value += owmClient->weather.readings.humidity;
  } else if (key.equalsIgnoreCase("city")) {
    if (settings->owmOptions.nickname.isEmpty()) { value += owmClient->weather.location.city; }
    else { value += settings->owmOptions.nickname; }
  }
}


// ----- WebThing Functions
void WTAppImpl::setTitle() {
  if (WebThing::settings.hostname.isEmpty()) {
    WebUI::setTitle(appName);
  } else {
    WebUI::setTitle(appName + " (" + WebThing::settings.hostname + ")");
  }
}

void WTAppImpl::baseConfigChange() {
  setTitle();
}

void WTAppImpl::configModeCallback(const String &ssid, const String&) {
  screens.configScreen->setSSID(ssid);
  ScreenMgr.display(screens.configScreen);
}

void WTAppImpl::prepWebThing(bool respectPowerSettings) {
  if (!respectPowerSettings) {
    WebThing::settings.hasVoltageSensing = false;
    WebThing::settings.sleepOverridePin = Basics::UnusedPin;
    WebThing::displayPowerOptions(false);               // Don't let the user fool with this.    
  }

  WebThing::replaceEmptyHostname(appPrefix.c_str());
  WebThing::notifyOnConfigMode([this](const String &ssid, const String &ip) { configModeCallback(ssid, ip); });
  WebThing::notifyConfigChange([this]() { baseConfigChange(); } );
  WebThing::setup();
  WebThing::setDisplayedVersion(appPrefix + appVersion);
  setTitle();
}

