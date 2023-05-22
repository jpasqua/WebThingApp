/*
 * SettingsOwner:
 *    Protocol for object's that own BaseSettings
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef SettingsOwner_h
#define SettingsOwner_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BaseSettings.h>
//                                  WebThingApp Includes
#include "../../gui/Screen.h"
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class SettingsOwner {
public:
  virtual void settingsHaveChanged() = 0;
  virtual BaseSettings* getSettings() = 0;
};

#endif  // SettingsOwner_h
#endif