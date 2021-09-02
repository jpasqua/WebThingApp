/*
 * CMSettings.h
 *    Setting for the CMCurrencyApp.
 *
 */

#ifndef CMSettings_h
#define CMSettings_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
//                                  WebThing Includes
#include <WTAppSettings.h>
//                                  Local Includes
#include "CurrencyMonApp.h"
#include "src/clients/RateClient.h"
//--------------- End:    Includes ---------------------------------------------


class CMSettings: public WTAppSettings {
public:
  static constexpr uint8_t MaxCurrencies = 3;
  static constexpr uint8_t MinRefreshInterval = 4;

  // ----- Constructors and methods
  CMSettings();
  void fromJSON(JsonDocument &doc) override;
  void toJSON(JsonDocument &doc);
  void logSettings();

  uint16_t refreshInterval = 4;   // Time in hours between refreshing exchange rates
  struct {
    String id;
    String nickname;
  } currencies[MaxCurrencies];
  
  String rateApiKey;              // api key for exchange rate service

private:
  // ----- Constants -----
  static constexpr uint32_t CurrentVersion = 0x0001;
};
#endif // CMSettings_h