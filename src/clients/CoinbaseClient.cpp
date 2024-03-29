/*
 * CoinbaseClient:
 *     Very simple client to read crypto buy prices using Coinbase api
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <JSONService.h>
//                                  Local Includes
#include "CoinbaseClient.h"
//--------------- End:    Includes ---------------------------------------------


#if defined(MockCoinbase)
  bool CoinbaseClient::getPrice(const String&, String& currency, String& price) {
    currency = "USD";
    price = String(random(0, 500000)/10.0, 2);
    return true;
  }
#else   // MockCoinbase
  /*------------------------------------------------------------------------------
   *
   * Constants
   *
   *----------------------------------------------------------------------------*/

  static constexpr char SiteValidation[] = 
  #if defined(ESP8266)
    // Use a fingerprint for coinbase
    "11 7A 9E 53 1A 1A 84 1A 04 0A B8 9E A5 40 95 87 7A 3B 43 4D";
  #else // ESP32
    // Use a cert for the coinbase root certificate (Baltimore Trust)
    // Using the root makes the certicate more robust over time
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n" \
    "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n" \
    "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n" \
    "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n" \
    "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n" \
    "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n" \
    "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n" \
    "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n" \
    "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n" \
    "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n" \
    "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n" \
    "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n" \
    "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n" \
    "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n" \
    "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n" \
    "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n" \
    "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n" \
    "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n" \
    "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n" \
    "-----END CERTIFICATE-----\n";
  #endif
    

  /*------------------------------------------------------------------------------
   *
   * Class Variables
   *
   *----------------------------------------------------------------------------*/

  static JSONService CoinbaseService(ServiceDetails("api.coinbase.com", 443));


  /*------------------------------------------------------------------------------
   *
   * Public Methods
   *
   *----------------------------------------------------------------------------*/            

  bool CoinbaseClient::getPrice(const String& coinID, String& currency, String& price) {
    constexpr uint32_t ReplyJSONSize = 256;
    constexpr uint8_t MaxFailures = 10;
    static uint8_t nFailures = 0;

    if (coinID.isEmpty()) return false;
    if (nFailures > MaxFailures) return false;

    String endpoint;
    endpoint.reserve(28);
    endpoint = "/v2/prices/";
    endpoint += coinID;
    endpoint += "/buy";

    DynamicJsonDocument *root = CoinbaseService.issueGET(endpoint, ReplyJSONSize, NULL, SiteValidation);
    if (!root) {
      Log.warning(F("CoinbaseClient::getPrice(): issueGet failed"));
      nFailures++;
      return false;
    }
    serializeJsonPretty(*root, Serial); Serial.println();
    // Example interaction:
    //    $ curl https://api.coinbase.com/v2/prices/BTC-USD/buy
    //    {"data":{"base":"BTC","currency":"USD","amount":"34909.53"}}

    currency = (*root)["data"]["currency"].as<const char*>();
    price = (*root)["data"]["amount"].as<const char*>();

    delete root;

    return true;
  }
#endif  // MockCoinbase

