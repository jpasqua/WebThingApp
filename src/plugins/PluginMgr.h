#ifndef PluginMgr_h
#define PluginMgr_h

/*
 * PluginMgr.h
 *    Finds, instantiates, and manages all Plugins
 *
 * NOTES:
 *
 */

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
#include <functional>
//                                  Third Party Libraries
#include <ArduinoJson.h>
//                                  WebThing Includes
#include <BPABasics.h>
//                                  Local Includes
#include "Plugin.h"
//--------------- End:    Includes ---------------------------------------------


class PluginMgr {
public:
  // ----- Types
  using Factory = std::function<Plugin*(const String&)>;

  // ----- Constants
  static constexpr char PluginNamespacePrefix = 'E';
  
  // ----- Class Functions
  static DynamicJsonDocument* getDoc(String filePathString, uint16_t maxFileSize);
  static void setFactory(Factory theFactory);

  // ----- Member Functions
  void      loadAll(String pluginRoot);
  void      refreshAll(bool force = false);
  uint8_t   getPluginCount();
  Plugin*   getPlugin(uint8_t index);
  Plugin**  getPlugins();
  void      map(const String& key, String& value);
  void      displayPlugin(uint8_t pluginIndex);
  void      displayNextPlugin();

private:
  static constexpr uint8_t MaxPlugins = 4;
  static Factory factory;
  
  uint8_t _nPlugins;
  Plugin* _plugins[MaxPlugins];

  int curPlugin = -1; // Used to iterate plugins for displayNextPlugin()

  bool validatePluginFiles(String pluginPath);
  void newPlugin(String pluginPath);
  uint8_t enumPlugins(const String& pluginRoot, String* pluginDirNames);
};

#endif // PluginMgr_h
