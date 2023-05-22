/*
 * WeatherScreen:
 *    Display weather info for the selected city and a running clock. 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef WeatherScreen_h
#define WeatherScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
#include <vector>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <BaseSettings.h>
//                                  WebThingApp Includes
#include "../../gui/Screen.h"
//                                  Local Includes
#include "ScrollScreen.h"
#include "SettingsOwner.h"
//--------------- End:    Includes ---------------------------------------------

class WSSettings : public BaseSettings {
public:  
  struct Field {
    Field() = default;
    Field(const String& theID, bool on) : id(theID), enabled(on) { }
    String id;
    bool enabled;
  };

  WSSettings();

  // Ensures that other variants of fromJSON() / toJSON aren't hidden
  // See: https://isocpp.org/wiki/faq/strange-inheritance#hiding-rule
  using BaseSettings::fromJSON;
  using BaseSettings::toJSON;

  // Must override these functions of BaseSettings
  virtual void fromJSON(const JsonDocument& doc) override;
  virtual void toJSON(JsonDocument& doc) override;

  // May override these functions of BaseSettings
  virtual void logSettings() override;

  std::vector<Field> fields;
};

class WeatherScreen : public ScrollScreen, public SettingsOwner {
public:
  WeatherScreen();

  virtual void innerActivation() override;
  virtual void settingsHaveChanged() override;
  virtual BaseSettings* getSettings() { return &settings; }

  WSSettings settings;

private:
  uint32_t lastDT = UINT32_MAX;
};

#endif  // WeatherScreen_h
#endif