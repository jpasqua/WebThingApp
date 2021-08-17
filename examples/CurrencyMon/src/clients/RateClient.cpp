
//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoJson.h>
#include <ArduinoLog.h>
//                                  Local Includes
#include "RateClient.h"
//--------------- End:    Includes ---------------------------------------------


RateClient::RateClient(String& apiKey, Currency* currencies, uint8_t nCurrencies) {
	static const String ServerName = "api.exchangeratesapi.io";

	_nCurrencies = (nCurrencies > MaxCurrencies) ? MaxCurrencies : nCurrencies;
	_currencies = currencies;
	_apiKey = apiKey;
	_apiKey.trim();

  details.server = ServerName;
  details.port = 80;
  details.apiKey = "";
  details.apiKeyName = "";
  service = new JSONService(details);

  if (_apiKey.isEmpty() || _nCurrencies == 0) {
  	_endpoint = "";
  } else {
	  _endpoint = "/v1/latest?access_key=";
	  _endpoint += apiKey;
	  _endpoint += "&symbols=";
	  for (int i = _nCurrencies-1; i >= 0; i--) {
	  	_endpoint += _currencies[i].id;
	  	if (i) _endpoint += ',';
	  }
  }
  Log.verbose("n: %d, _endpoint=%s", _nCurrencies, _endpoint.c_str());
}

void RateClient::updateRates() {
  static const uint32_t ResultSize = 1000;

  if (_endpoint.isEmpty()) return;	// Nothing to do...

  DynamicJsonDocument *root = service->issueGET(_endpoint, ResultSize);
  if (!root) {
    Log.warning(F("issueGET failed for Rate Information"));
    return;
  }
  serializeJsonPretty(*root, Serial); Serial.println();

  for (int i = 0; i < _nCurrencies; i++) {
  	_currencies[i].exchangeRate = (*root)["rates"][_currencies[i].id]|1.0;
  }
  delete root;

  timeOfLastUpdate = millis();
}

// Convert from one currency to another
float RateClient::convert(Currency* from, Currency* to, float amount) {
	return (amount / from->exchangeRate) * to->exchangeRate;
}

