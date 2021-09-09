//#include <ESP8266WiFi.h>
#include <ArduinoLog.h>
#include <JSONService.h>
#include "BlynkClient.h"

static constexpr char BlynkServer[] = "blynk-cloud.com";
static constexpr uint16_t BlynkPort = 80;
static constexpr uint8_t MaxFailures = 10;

static JSONService blynkService(ServiceDetails(BlynkServer, BlynkPort));
static uint8_t nFailures = 0;

bool BlynkClient::readPin(const String& blynkAppID, const String& pin, String& value) {
    if (blynkAppID.isEmpty()) return false;
    if (nFailures > MaxFailures) return false;

    String endpoint;
    endpoint.reserve(64);
    endpoint = "/";
    endpoint += blynkAppID;
    endpoint += "/get/";
    endpoint += pin;

    DynamicJsonDocument *root = blynkService.issueGET(endpoint, 256);
    if (!root) {
      Log.warning(F("BlynkClient::readPin(): issueGet failed"));
      nFailures++;
      return false;
    }
    //serializeJsonPretty(*root, Serial); Serial.println();

    value = (*root)[0].as<String>();
    delete root;

    return true;
}
