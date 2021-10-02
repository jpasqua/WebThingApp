/*
 * WTAppImpl
 *    Generic App Framework for WebThings
 *
 */

#ifndef WTAppImpl_h
#define WTAppImpl_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "WTApp.h"
#include "plugins/PluginMgr.h"
#include "screens/CalibrationScreen.h"
#include "screens/ConfigScreen.h"
#include "screens/ForecastScreen.h"
#include "screens/RebootScreen.h"
#include "screens/WeatherScreen.h"
#include "screens/WiFiScreen.h"
#include "screens/UtilityScreen.h"
//--------------- End:    Includes ---------------------------------------------


#define wtAppImpl ((WTAppImpl*)wtApp)

class WTAppImpl : public WTApp {
public:
  PluginMgr pluginMgr;

  CalibrationScreen*  calibrationScreen;
  ConfigScreen*       configScreen;
  ForecastScreen*     forecastScreen;
  RebootScreen*       rebootScreen;
  WeatherScreen*      weatherScreen;
  WiFiScreen*         wifiScreen;
  UtilityScreen*      utilityScreen;
  Screen*             splashScreen;       // Optional. Supplied by the app.

  WTAppImpl(const String& name, const String& prefix, const String& version, WTAppSettings* appSettings);
  
  // ----- Utility functions for the rest of the app
  void askToReboot();
  void saveSettings();
  void configMayHaveChanged();
  void weatherConfigMayHaveChanged();
  void conditionalUpdate(bool force);
  void updateAllData();

  // ----- Functions that aren't provided by subclasses
  void begin();
  void loop();

  // ----- Functions that *must* be provided by subclasses
  virtual void app_registerDataSuppliers() = 0;
  virtual Screen* app_registerScreens() = 0;
  virtual void app_initWebUI() = 0;
  virtual void app_initClients() = 0;
  virtual void app_conditionalUpdate(bool force = false) = 0;
  virtual void app_loop() = 0;

  // ----- Functions that *may* be provided by subclasses
  virtual void app_registerButtons() {  };

protected:
  // ----- Weather Functions
  void refreshWeatherData(bool force = false);
  void prepWeather();
  void weatherDataSupplier(const String& key, String& value);

  // ----- WebThing Functions
  void setTitle();
  void baseConfigChange();
  void configModeCallback(const String &ssid, const String &ip);
  void prepWebThing();

private:
  void registerScreens();
};

#endif  // WTAppImpl_h
