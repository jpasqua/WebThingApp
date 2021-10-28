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
#include "gui/ScreenSettings.h"
//--------------- End:    Includes ---------------------------------------------


class OWMOptions {
public:
  bool    enabled;      // Get weather data from OpenWeatherMap?
  String  key;          // Your API Key from http://openweathermap.org/
  int     cityID;       // OpenWeatherMap City ID
  String  language;     // See OpenWeatherMap for list of available languages
  String  nickname;     // If not empty, override the city name with this nickname (e.g. Home)

  OWMOptions();
  void fromJSON(const JsonDocument &doc);
  void toJSON(JsonDocument &doc);
  void logSettings();
};


class WTAppSettings: public BaseSettings {
public:
  // ----- Constructors and methods
  WTAppSettings();
  virtual void fromJSON(const JsonDocument &doc) override;
  virtual void toJSON(JsonDocument &doc) override;
  void logSettings();

  DisplayOptions displayOptions;
  UIOptions uiOptions;
  OWMOptions owmOptions;
  ScreenSettings screenSettings;

private:
};

#endif  // WTAppSettings_h