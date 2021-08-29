/*
 * CoinbaseClient:
 *     Very simple client to read crypto buy prices using Coinbase api
 *
 */

#ifndef CoinbaseClient_h
#define CoinbaseClient_h

#include <Arduino.h>

// If you are in a low memory situation & can't afford the program or heap space
// requirements of SSL, you can mock this service by #define-ing MockCoinbase
// This is defined by defaul on ESP8266
#if defined(ESP8266)
  #define MockCoinbase
#endif

class CoinbaseClient {
public:
  static bool getPrice(String coinID, String& currency, String& price);
};

#endif  // CoinbaseClient_h