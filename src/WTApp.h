/*
 * WTApp
 *    A data class that holds basic app information
 *
 */

#ifndef WTApp_h
#define WTApp_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <WebThingBasics.h>
//                                  Local Includes
#include "WTAppSettings.h"
#include "clients/OWMClient.h"
//--------------- End:    Includes ---------------------------------------------


class WTApp {
public:
  const String appName;
  const String appPrefix;
  const String appVersion;

  OWMClient* owmClient;
  WTAppSettings* settings;

  WTApp(const String& name, const String& prefix, const String& version, WTAppSettings* appSettings);

};

extern WTApp* wtApp;

#endif  // WTApp_h
