/*
 * AIOPlugin
 *    A plugin that gets and displays content from AIO
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <ArduinoLog.h>
#include <BPABasics.h>
//                                  WebThing Libraries
#include <clients/AIOMgr.h>
//                                  Local Includes
#include "../../gui/Theme.h"
#include "../../gui/ScreenMgr.h"
#include "AIOPlugin.h"
//--------------- End:    Includes ---------------------------------------------



/*------------------------------------------------------------------------------
 *
 * AIOSettings Implementation
 *
 *----------------------------------------------------------------------------*/

AIOSettings::AIOSettings() {
  version = 1;
  maxFileSize = 2048;

  enabled = true;
  nGroups = 0;
  groupNames = NULL;
  nicknames = NULL;
  nFeeds = 0;
  feeds = NULL;
  riScale = 60 * 1000L;   // Operate in minutes
  refreshInterval = 10;   // 10 Minutes
}

void AIOSettings::fromJSON(const JsonDocument &doc) {
  JsonArrayConst groupNameArray = doc[F("groups")];
  JsonArrayConst nameArray = doc[F("nicknames")];
  nGroups = groupNameArray.size();  
  groupNames = new String[nGroups];
  nicknames = new String[nGroups];

  for (int i = 0; i < nGroups; i++) {
    groupNames[i] = groupNameArray[i].as<String>();
    nicknames[i] = nameArray[i].as<String>();
  }

  JsonArrayConst feedArray = doc[F("feeds")];
  nFeeds = feedArray.size();  
  feeds = new String[nFeeds];

  for (int i = 0; i < nFeeds; i++) {
    feeds[i] = feedArray[i].as<String>();
  }

  enabled = doc[F("enabled")];
  username = doc[F("username")]|"";
  key = doc[F("key")]|"";
  refreshInterval = doc[F("refreshInterval")];
  riScale = doc[F("riScale")];
}

void AIOSettings::fromJSON(const String& settings) {
  DynamicJsonDocument doc(maxFileSize);
  auto error = deserializeJson(doc, settings);
  if (error) {
    Log.warning(F("AIOSettings::fromJSON, failed to parse new settings: %s"), error.c_str());
    return;
  }
  fromJSON(doc);
}

void AIOSettings::toJSON(JsonDocument &doc) {
  JsonArray groupArray = doc.createNestedArray(F("groupNames"));
  JsonArray nnArray = doc.createNestedArray(F("nicknames"));
  for (int i = 0; i < nGroups; i++) {
    groupArray.add(groupNames[i]);
    nnArray.add(nicknames[i]);
  }

  JsonArray feedArray = doc.createNestedArray(F("feeds"));
  for (int i = 0; i < nFeeds; i++) {
    feedArray.add(feeds[i]);
  }

  doc[F("refreshInterval")] = refreshInterval;
  doc[F("username")] = username;
  doc[F("key")] = key;
  doc[F("riScale")] = riScale;
  doc[F("enabled")] = enabled;
}

void AIOSettings::toJSON(String& serialized) {
  DynamicJsonDocument doc(maxFileSize);
  toJSON(doc);
  serializeJson(doc, serialized);
}

void AIOSettings::logSettings() {
  Log.verbose(F("----- AIOSettings"));
  Log.verbose(F("enabled: %T"), enabled);
  Log.verbose(F("AIO username: %s, key: %s"), username.c_str(), key.c_str());
  Log.verbose(F("Groups:"));
  for (int i = 0; i < nGroups; i++) {
    Log.verbose(F("  %s (%s)"), nicknames[i].c_str(), groupNames[i].c_str());
  }
  Log.verbose(F("Feeds:"));
  for (int i = 0; i < nFeeds; i++) {
    Log.verbose(F("  %d: %s"), i, feeds[i].c_str());
  }
  Log.verbose(F("refreshInterval: %d"), refreshInterval);
  Log.verbose(F("riScale: %d"), riScale);
}


/*------------------------------------------------------------------------------
 *
 * AIOPlugin Implementation
 *
 *----------------------------------------------------------------------------*/

AIOPlugin::~AIOPlugin() {
  // TO DO: free the settings object
}

void AIOPlugin::getSettings(String& serializedSettings) {
  settings.toJSON(serializedSettings);
}

void AIOPlugin::newSettings(const String& serializedSettings) {
  settings.fromJSON(serializedSettings);
  settings.write();
}

uint32_t AIOPlugin::getUIRefreshInterval() { return settings.refreshInterval * settings.riScale; }

bool AIOPlugin::typeSpecificInit() {
  settings.init(_pluginDir + "/settings.json");
  settings.read();
  settings.logSettings();

  _enabled = settings.enabled;
  _feedVals = new String[settings.nFeeds];

  // It's ok to call this even if another component has already done so.
  // AIOMgr is resilient to that.
  AIOMgr::init(settings.username, settings.key);

  return true;
}

void AIOPlugin::typeSpecificMapper(const String& key, String& value) {
  for (int i = 0; i < settings.nFeeds; i++) {
    if (key == settings.feeds[i]) {
      value = _feedVals[i];
      return;
    }
  }
  if (key.startsWith("NN")) {
    int whichNN = key.substring(2).toInt();
    if (whichNN > 0 && whichNN <= settings.nGroups) { value = settings.nicknames[whichNN-1]; }
  }
}

void AIOPlugin::refresh(bool force) {
    if (force || (_nextRefresh <= millis())) {
      ScreenMgr.showActivityIcon(Theme::Color_UpdatingPlugins);
      for (int feedIndex = 0; feedIndex < settings.nFeeds; feedIndex++) {
        int separator = settings.feeds[feedIndex].indexOf('.');
        int groupIndex = settings.feeds[feedIndex].substring(0, separator).toInt();
        String fqFeedName = settings.groupNames[groupIndex];
        fqFeedName.concat(settings.feeds[feedIndex].substring(separator)); // Includes the '.'
        AIOMgr::aio->get(fqFeedName.c_str(), _feedVals[feedIndex]);
        // Log.verbose("Updated feed %d to %s", feedIndex, _feedVals[feedIndex].c_str());
      }
      _nextRefresh = millis() + (settings.refreshInterval * settings.riScale);
      ScreenMgr.hideActivityIcon();
    }
}
