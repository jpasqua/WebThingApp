/*
 * OLEDTestApp:
 *    This is the core of the MultiMon functionality. It is invoked
 *    by the primary sketch ".ino" file.
 *
 * NOTES:
 * o OLEDTestApp is derived from WTAppImpl, which is itself derived
 *   from WTApp.
 * o This is a singleton and is made available to the whole app via
 *   an extern declaration in WTApp.h:
 *       WTApp* wtApp;
 * o This must be cast back to OLEDTestApp* in order to get to app-specific
 *   data and functions. Technically this should be a dynamic_cast, but for
 *   efficiency, we perform a "C-style" cast.
 * o Macros are provided to easily get the app and settings in their
 *   specialized forms.
 *
 * Customization:
 * o To add a new screen to the app, declare it here and instantiate it
 *   in the associated .cpp file.
 */


#ifndef OLEDTestApp_h
#define OLEDTestApp_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <WTAppImpl.h>
#include <screens/touch/EnterNumberScreen.h>
//                                  Local Includes
#include "OTSettings.h"
#include "src/screens/SplashScreen.h"
#include "src/screens/HomeScreen.h"
#include "src/clients/RateClient.h"
//--------------- End:    Includes ---------------------------------------------


// A few convenience macros:
// otApp simply yields the wtApp singleton cast as a OLEDTestApp*
// otSettings yields the settings object from wtApp, cast as a OTSettings*
#define otApp ((OLEDTestApp*)wtApp)
#define otSettings ((OTSettings*)otApp->settings)


class OLEDTestApp : public WTAppImpl {
public:

  static void create();

  // CUSTOM: Screens implemented by this app
  SplashScreen*   splashScreen;
  HomeScreen*     homeScreen;

  // CUSTOM: Data defined by this app which is available to the whole app
  RateClient* rateClient;
  Currency currencies[OTSettings::MaxCurrencies];

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
  OLEDTestApp(OTSettings* settings);

};

#endif	// OLEDTestApp_h
