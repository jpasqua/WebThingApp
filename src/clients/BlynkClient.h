#ifndef BlynkClient_h
#define BlynkClient_h

#include <Arduino.h>

class BlynkClient {
public:
  static bool readPin(const String& blynkAppID, const String& pin, String& value);
};

#endif  // BlynkClient_h
