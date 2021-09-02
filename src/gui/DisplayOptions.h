/*
 * DisplayOptions:
 *                    
 */

#ifndef DisplayOptions_h
#define	DisplayOptions_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoJson.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------


class CalibrationData {
public:
  static constexpr int nCalReadings = 5;
  uint16_t readings[nCalReadings];

  CalibrationData();
  void fromJSON(JsonDocument &doc);
  void toJSON(JsonDocument &doc);
  void logSettings();
};

class DisplayOptions {
public:
  bool invertDisplay;
  CalibrationData calibrationData;

  DisplayOptions();
  void fromJSON(JsonDocument &doc);
  void toJSON(JsonDocument &doc);
  void logSettings();
};


#endif	// DisplayOptions_h