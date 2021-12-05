#ifndef ScreenSettings_h
#define ScreenSettings_h

#include <map>
#include <vector>
#include <ArduinoJson.h>
#include "Screen.h"

class ScreenSettings {
public:
  struct ScreenInfo {
    ScreenInfo() = default;
    ScreenInfo(const String& theID, bool on)
      : id(theID), enabled(on) { }
    String id;
    bool enabled;
  };

  ScreenSettings();

  void fromJSON(const JsonDocument &doc);
  void fromJSON(const String &json);
  void toJSON(JsonDocument &doc);
  void toJSON(Stream &s);
  void toJSON(String &serialString);
  void logSettings();

  std::vector<ScreenInfo> screenInfo;

private:
  static constexpr size_t MaxDocSize = 1024;
};

#endif  // ScreenSettings_h