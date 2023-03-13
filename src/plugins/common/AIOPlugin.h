#ifndef AIOPlugin_h
#define AIOPlugin_h

/*
 * AIOPlugin
 *    A plugin that interacts with AdafruitIO
 *
 */


//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <AdafruitIO_Group.h>
//                                  WebThing Libraries
#include <BaseSettings.h>
//                                  Local Includes
#include "../Plugin.h"
//--------------- End:    Includes ---------------------------------------------

class AIOSettings : public BaseSettings {
public:
  // ----- Constructors and methods
  AIOSettings();
  void fromJSON(const JsonDocument &doc);
  void fromJSON(const String& settings);
  void toJSON(JsonDocument &doc);
  void toJSON(String& serialized);
  void logSettings();

  // ----- Settings
  bool      enabled;    // Is the Plugin enabled?
  String    username;   // AIO Username
  String    key;        // AIO key
  uint8_t   nGroups;    // The number of AIO groups we will read/write
  String*   groupNames; // The names of the groups (technically, the key of the group)
  String*   nicknames;  // Nicknames for the groups which may be used in the UI
  uint8_t   nFeeds;     // Total number of feeds across all groups
  String*   feeds;      // feed names of each feed. This is actually a composite of:
                        //    groupName.feedName
  uint32_t  refreshInterval;  // How often to refresh the data
  uint32_t  riScale;    // NOT a user-visible setting!
};

class AIOPlugin : public Plugin {
public:
  AIOSettings settings;

  ~AIOPlugin();
  bool typeSpecificInit();
  void typeSpecificMapper(const String& key, String& value);
  void refresh(bool force = false);
  void getSettings(String& serializedSettings);
  void newSettings(const String& serializedSettings);
  uint32_t getUIRefreshInterval();

private:
  const char* decompressFeedName(int feedIndex);
  void requestNextPendingValue();

  String*             _feedVals = NULL;
  uint32_t            _nextRefresh = 0;
};

#endif  // AIOPlugin_h