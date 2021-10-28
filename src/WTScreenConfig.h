/*
 * WTScreenConfig
 *     A class that contains a pointer to each of the standard screens that may be used
 *     by any app. The specific set of standard screens varies a bit by Display Device type.
 *
 */


#ifndef WTScreenConfig_h
#define WTScreenConfig_h

#include "gui/ScreenMgr.h"

// ----- BEGIN: DEVICE_TYPE_TOUCH
#if DEVICE_TYPE == DEVICE_TYPE_TOUCH

#include "screens/touch/CalibrationScreen.h"
#include "screens/touch/ConfigScreen.h"
#include "screens/touch/ForecastScreen.h"
#include "screens/touch/RebootScreen.h"
#include "screens/touch/WeatherScreen.h"
#include "screens/touch/WiFiScreen.h"
#include "screens/touch/UtilityScreen.h"

class AppScreens {
public:
  CalibrationScreen*  calibrationScreen;
  ConfigScreen*       configScreen;
  ForecastScreen*     forecastScreen;
  RebootScreen*       rebootScreen;
  WeatherScreen*      weatherScreen;
  WiFiScreen*         wifiScreen;
  UtilityScreen*      utilityScreen;

  Screen*             splashScreen;       // Optional. Supplied by the app.

	void registerScreens() {
	  // CUSTOM: To avoid loading a screen, comment out the corresponding pair of lines below.

	  calibrationScreen = new CalibrationScreen();
	  ScreenMgr.registerScreen("Calibration", calibrationScreen);

	  configScreen = new ConfigScreen();
	  ScreenMgr.registerScreen("Config", configScreen);

	  forecastScreen = new ForecastScreen();
	  ScreenMgr.registerScreen("Forecast", forecastScreen);

	  rebootScreen = new RebootScreen();
	  ScreenMgr.registerScreen("Reboot", rebootScreen);

	  weatherScreen = new WeatherScreen();
	  ScreenMgr.registerScreen("Weather", weatherScreen);

	  wifiScreen = new WiFiScreen();
	  ScreenMgr.registerScreen("WiFi", wifiScreen);

	  utilityScreen = new UtilityScreen();
	  ScreenMgr.registerScreen("Utility", utilityScreen);
	}
};


// ----- BEGIN: DEVICE_TYPE_OLED
#elif DEVICE_TYPE == DEVICE_TYPE_OLED

#include "screens/oled/ConfigScreen.h"
#include "screens/oled/ForecastScreen.h"
#include "screens/oled/InfoScreen.h"
#include "screens/oled/RebootScreen.h"
#include "screens/oled/WeatherScreen.h"
#include "screens/oled/WiFiScreen.h"

class AppScreens {
public:
  ConfigScreen*       configScreen;
  ForecastLast2*      forecastLast2;
  ForecastFirst3*     forecastFirst3;
  InfoScreen*         infoScreen;
  RebootScreen*       rebootScreen;
  WeatherScreen*      weatherScreen;
  WiFiScreen*         wifiScreen;
  Screen*             splashScreen;       // Optional. Supplied by the app.

	void registerScreens() {
	  // CUSTOM: To avoid loading a screen, comment out the corresponding pair of lines below.

	  configScreen = new ConfigScreen();
	  ScreenMgr.registerScreen("Config", configScreen);

	  forecastFirst3 = new ForecastFirst3();
	  ScreenMgr.registerScreen("Forecast-3", forecastFirst3);

	  forecastLast2 = new ForecastLast2();
	  ScreenMgr.registerScreen("Forecast-2", forecastLast2);

	  infoScreen = new InfoScreen();
	  ScreenMgr.registerScreen("Info", infoScreen);

	  rebootScreen = new RebootScreen();
	  ScreenMgr.registerScreen("Reboot", rebootScreen);

	  weatherScreen = new WeatherScreen();
	  ScreenMgr.registerScreen("Weather", weatherScreen);

	  wifiScreen = new WiFiScreen();
	  ScreenMgr.registerScreen("WiFi", wifiScreen);	  
	}
};
#endif

#endif	// WTScreenConfig_h