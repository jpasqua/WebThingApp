/*
 * CurrencyMonApp:
 *    This is the core of the MultiMon functionality. It is invoked
 *    by the primary sketch ".ino" file.
 *
 * NOTES:
 * o CurrencyMonApp is derived from WTAppImpl, which is itself derived
 *   from WTApp.
 * o This is a singleton and is made available to the whole app via
 *   an extern declaration in WTApp.h:
 *       WTApp* wtApp;
 * o This must be cast back to CurrencyMonApp* in order to get to app-specific
 *   data and functions. Technically this should be a dynamic_cast, but for
 *   efficiency, we perform a "C-style" cast.
 * o Macros are provided to easily get the app and settings in their
 *   specialized forms.
 *
 * Customization:
 * o To add a new screen to the app, declare it here and instantiate it
 *   in the associated .cpp file.
 */


#ifndef CurrencyMonApp_h
#define CurrencyMonApp_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <GrillClient.h>
//                                  WebThing Includes
#include <WTAppImpl.h>
#include <screens/EnterNumberScreen.h>
//                                  Local Includes
#include "CMSettings.h"
#include "src/screens/SplashScreen.h"
#include "src/screens/HomeScreen.h"
#include "src/clients/RateClient.h"
//--------------- End:    Includes ---------------------------------------------


// A few convenience macros:
// cmApp simply yields the wtApp singleton cast as a CurrencyMonApp*
// cmSettings yields the settings object from wtApp, cast as a CMSettings*
#define cmApp ((CurrencyMonApp*)wtApp)
#define cmSettings ((CMSettings*)cmApp->settings)


class CurrencyMonApp : public WTAppImpl {
public:

  static void create();

  // CUSTOM: Screens implemented by this app
  SplashScreen*   splashScreen;
  HomeScreen*     homeScreen;
  // CUSTOM: Include Screen implemented by WebThingApp
  EnterNumberScreen* numpad;

  // CUSTOM: Data defined by this app which is available to the whole app
  RateClient* rateClient;
  Currency currencies[CMSettings::MaxCurrencies];

  // ----- Functions that *must* be provided by subclasses
  virtual void app_registerDataSuppliers() override;
  virtual Screen* app_registerScreens() override;
  virtual void app_initWebUI() override;
  virtual void app_initClients() override;
  virtual void app_conditionalUpdate(bool force = false) override;
  virtual void app_loop() override;

  // ----- Functions that *may* be provided by subclasses
  virtual void app_registerButtons() override;

  // ----- Public functions
  CurrencyMonApp(CMSettings* settings);

};

#endif	// CurrencyMonApp_h
