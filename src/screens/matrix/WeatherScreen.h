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
//                                  WebThingApp Includes
#include "../../gui/Screen.h"
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class WSSettings {
public:
  static constexpr char const* settingsFilePath = "/WSFields.json";
  
  struct Field {
    Field() = default;
    Field(const String& theID, bool on) : id(theID), enabled(on) { }
    String id;
    bool enabled;
  };

  WSSettings();

  bool fromJSON(const char* filePath);
  void fromJSON(const JsonDocument& doc);
  void fromJSON(const String& json);
  void toJSON(JsonDocument& doc);
  void toJSON(Stream& s);
  void toJSON(String& serialString);
  void toJSON(const char* filePath);
  void logSettings();

  std::vector<Field> fields;

private:
  static constexpr size_t MaxDocSize = 512;
};

class WeatherScreen : public ScrollScreen {
public:
  static String getTextForIcon(String& icon);
  WSSettings settings;

  WeatherScreen();
  virtual void innerActivation() override;
  void fieldsHaveBeenUpdated() { lastDT = UINT32_MAX; }

private:
  uint32_t lastDT = UINT32_MAX;
};

#endif  // WeatherScreen_h
#endif