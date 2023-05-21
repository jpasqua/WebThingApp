/*
 * ForecastScreen:
 *    Display the forecast for the selected city 
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef ForecastScreen_h
#define ForecastScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <BaseSettings.h>
//                                  Local Includes
#include "ScrollScreen.h"
//--------------- End:    Includes ---------------------------------------------

class FSSettings : public BaseSettings {
public:
  struct Field {
    Field() = default;
    Field(const String& theID, bool on) : id(theID), enabled(on) { }
    String id;
    bool enabled;
  };

  FSSettings();

  // Ensures that other variants of fromJSON() / toJSON aren't hidden
  // See: https://isocpp.org/wiki/faq/strange-inheritance#hiding-rule
  using BaseSettings::fromJSON;
  using BaseSettings::toJSON;

  // Must override these functions of BaseSettings
  virtual void fromJSON(const JsonDocument& doc) override;
  virtual void toJSON(JsonDocument& doc) override;

  // May override these functions of BaseSettings
  virtual void logSettings() override;

  std::vector<Field> heading;
  String label;
  std::vector<Field> fields;
};

class ForecastScreen : public ScrollScreen {
public:
  ForecastScreen();
  virtual void innerActivation();
  void settingsHaveChanged();

  FSSettings settings;

private:
  String _forecastText;
  uint32_t _timeOfLastForecast = 0;

  void appendForecastForDay(Forecast* forecast, String& appendTo);
};

#endif  // ForecastScreen_h
#endif