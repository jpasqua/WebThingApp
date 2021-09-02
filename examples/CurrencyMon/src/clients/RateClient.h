#ifndef RateClient_h
#define RateClient_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <JSONService.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------


class Currency {
public:
	String id;
	float exchangeRate;
  bool inactive() { return (id.isEmpty()); }
  bool active() { return !inactive(); }
};

class RateClient {
public:
	static constexpr uint8_t MaxCurrencies = 4;

  uint32_t      timeOfLastUpdate = 0;

  RateClient(String& apiKey, Currency* currencies, uint8_t nCurrencies);

  // Update the exchange ratea
  void updateRates();

  // Convert from one currency to another
  float convert(Currency* from, Currency* to, float amount);

private:
	String _apiKey;
	Currency* _currencies;
	uint8_t _nCurrencies;

  ServiceDetails  details;
  JSONService     *service = NULL;

  String _endpoint;

  void mockUpdate();
};

#endif  // RateClient_h
