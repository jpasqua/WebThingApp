/*
 * UIOptions:
 *                    
 */

#ifndef UIOptions_h
#define UIOptions_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoJson.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class BrightnessSchedule {
public:
  bool active;                        // Is the scheduling system active?
  struct {
    uint8_t hr;                       // Hour that the period starts (24 hour time)
    uint8_t min;                      // Minute that the period starts
    uint8_t brightness;               // The brightness level during this period
  } morning, evening;

  BrightnessSchedule();
  void fromJSON(const JsonDocument &doc);
  void toJSON(JsonDocument &doc);
  void logSettings();
};

class UIOptions {
public:
  bool use24Hour;
  bool useMetric;
  bool showDevMenu;
  BrightnessSchedule schedule;

  UIOptions();
  void fromJSON(const JsonDocument &doc);
  void toJSON(JsonDocument &doc);
  void logSettings();
};


#endif  // UIOptions_h



