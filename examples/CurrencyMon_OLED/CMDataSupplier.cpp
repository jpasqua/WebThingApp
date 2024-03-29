/*
 * CMDataSupplier:
 *    Supplies app-specific data to the WebThing DataBroker
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
//                                  Local Includes
#include "CurrencyMonApp.h"
#include "CMDataSupplier.h"
//--------------- End:    Includes ---------------------------------------------


namespace CMDataSupplier {

  // CUSTOM: If your app has a custom data source, publish that data to
  // plugins by implementing a data supplier that maps keys to values.
  // In this case we publish the data from the RateClient
  void dataSupplier(const String& key, String& value) {
    // Handle "N.rate", "N.id", and "N.nick" which give the currency exchange rate,
    // id, and nickname repectively
    // of the Nth currency
    if (isDigit(key[0]) && key[1] == '.') {
      int index = (key[0] - '0') - 1;
      if (index >= CMSettings::MaxCurrencies) return;
      String subkey = key.substring(2);
      if (subkey.equals("rate")) value += cmApp->currencies[index].exchangeRate;
      else if (subkey.equals("id")) value += cmApp->currencies[index].id;
      else if (subkey.equals("nick")) value += cmSettings->currencies[index].nickname;
    }
  }

}

