#ifndef	WTAppSettings_h
#define WTAppSettings_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BaseSettings.h>
//                                  Local Includes
#include "gui/DisplayOptions.h"
#include "gui/UIOptions.h"
//--------------- End:    Includes ---------------------------------------------


class OWMOptions {
public:
  bool    enabled;      // Get weather data from OpenWeatherMap?
  String  key;          // Your API Key from http://openweathermap.org/
  int     cityID;       // OpenWeatherMap City ID
  String  language;     // See OpenWeatherMap for list of available languages
  String  nickname;     // If not empty, override the city name with this nickname (e.g. Home)

  OWMOptions();
  void fromJSON(JsonDocument &doc);
  void toJSON(JsonDocument &doc);
  void logSettings();
};


class WTAppSettings: public BaseSettings {
public:
  // ----- Constructors and methods
  WTAppSettings();
  void fromJSON(JsonDocument &doc) override;
  void toJSON(JsonDocument &doc);
  void logSettings();

  DisplayOptions displayOptions;
  UIOptions uiOptions;
  OWMOptions owmOptions;

private:
  // ----- Constants -----
  static const uint32_t CurrentVersion;
};

#endif  // WTAppSettings_h