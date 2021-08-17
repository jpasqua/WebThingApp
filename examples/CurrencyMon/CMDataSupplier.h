/*
 * CMDataSupplier:
 *    Supplies app-specific data to the WebThing DataBroker
 *
 * NOTES:
 * o Establishes the "Rates" namespace in the DataBroker using the prefix 'R'
 * o Provides information about the configured currencies
 * o Keys are of the form: $R.N.subkey, where
 *   + '$R' is the namespace prefix and is stripped away by the time the
 *     dataSupplier function is called.
 *   + 'N' is a digit between 0 & MaxCurrencies-1. It indicates which currency
 *     we're interested in
 *   + 'subkey' specifies the information of interest. It can be 'rate', 'id', or
 *     'nick' which provide the exchange rate, currency ID, or nickname respectively.
 *
 */

#ifndef CMDataSupplier_h
#define CMDataSupplier_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------


namespace CMDataSupplier {
  // CUSTOM: Choose a single character prefix to use for this data source
  static const char ERPrefix = 'R';
  
  extern void dataSupplier(const String& key, String& value);
}

#endif  // CMDataSupplier_h