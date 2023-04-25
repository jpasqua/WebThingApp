/*
 * LMSettings.h
 *    Setting for the LMCurrencyApp.
 *
 */

#ifndef LMSettings_h
#define LMSettings_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <BPA_PrinterSettings.h>
//                                  WebThing Includes
#include <WTAppSettings.h>
//                                  Local Includes
#include "LEDMatrixApp.h"
#include "src/clients/RateClient.h"
//--------------- End:    Includes ---------------------------------------------


class LMSettings: public WTAppSettings {
public:
  // ----- Constructors and methods
  LMSettings();
  void fromJSON(const JsonDocument &doc) override;
  void toJSON(JsonDocument &doc);
  void logSettings();

  struct {
    String username;
    String key;
    String groupName;
  } aio;

  uint16_t scrollDelay;


  static constexpr uint8_t MaxPrinters = 4;
  PrinterSettings printer[MaxPrinters];
  uint32_t printerRefreshInterval = 10;

private:
  // ----- Constants -----
  static constexpr uint32_t CurrentVersion = 0x0001;
};
#endif // LMSettings_h