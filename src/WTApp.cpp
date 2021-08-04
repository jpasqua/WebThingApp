/*
 * WTApp
 *    Generic App Framework for WebThings
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "WTApp.h"
//--------------- End:    Includes ---------------------------------------------


WTApp* wtApp;

WTApp::WTApp(
    const String& name, const String& prefix, const String& version, WTAppSettings* appSettings) :
    appName(name), appPrefix(prefix), appVersion(version), settings(appSettings)
{
  wtApp = this;
}
