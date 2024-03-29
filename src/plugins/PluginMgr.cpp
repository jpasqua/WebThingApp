/*
 * PluginMgr
 *    Finds, instantiates, and manages all Plugins
 *
 * NOTES:
 * o Because of the way SPIFFS works (or actually because of the way it doesn't work)
 *   We need to take a circuitous path to finding all the well-formed plugins. We can't
 *   really enumerate directories (they are a convenient fiction in SPIFFS) nor can we
 *   depend on the order of enumeration being breadth-first OR depth-first.
 *   Instead, we just enumerate all the files under pluginRoot and keep track of
 *   the unique subdirectories
 *
 */


//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
#include <FS.h>
//                                  Third Party Libraries
#include <ArduinoJson.h>
#include <ArduinoLog.h>
//                                  WebThing Includes
#include <ESP_FS.h>
#include <DataBroker.h>
#include <WebThing.h>
//                                  Local Includes
#include "../gui/ScreenMgr.h"
#include "PluginMgr.h"
//--------------- End:    Includes ---------------------------------------------


// ----- Class variables
PluginMgr::Factory PluginMgr::factory;


/*------------------------------------------------------------------------------
 *
 * Internal Utility Functions
 *
 *----------------------------------------------------------------------------*/

DynamicJsonDocument* PluginMgr::getDoc(String filePathString, uint16_t maxFileSize) {
  const char *filePath = filePathString.c_str();
  File file = ESP_FS::open(filePath, "r");
  if (!file) {
    Log.warning(F("Error opening: %s"), filePath);
    return NULL;
  }

  DynamicJsonDocument *doc = new DynamicJsonDocument(maxFileSize);
  auto error = deserializeJson(*doc, file);
  file.close();
  if (error) {
    delete doc;
    Log.warning(F("Failed to parse %s: %s"), filePath, error.c_str());
    return NULL;
  }

  return doc;
}


/*------------------------------------------------------------------------------
 *
 * Private Functions
 *
 *----------------------------------------------------------------------------*/

void PluginMgr::newPlugin(String pluginPath) {
  Log.verbose("Trying to create plugin for %s", pluginPath.c_str());
  if (factory == NULL) {
    Log.warning("No Plugin Factory was registered");
    return;
  }
  if (_nPlugins == PluginMgr::MaxPlugins) {
    Log.warning(F("Maximum number of plugins exceeded"));
    return;
  }

  DynamicJsonDocument* doc = getDoc(pluginPath + "/plugin.json", 2048);
  if (doc == NULL) {
    Log.warning("Unable to process plugin.json");
    return;
  }
  String type = (*doc)[F("type")].as<String>();
  String name = (*doc)[F("name")].as<String>();
  String piNamespace = (*doc)[F("namespace")].as<String>();
  delete doc;

  Plugin *p = factory(type);
  if (p == NULL) {
    Log.warning("Unable to instantiate plugin of type: %s", type.c_str());
    return;
  }

  Log.verbose("Initializing plugin %s", name.c_str());
  if (!p->init(name, piNamespace, pluginPath)) {
    delete p;
    return;
  }

  if (p->getFlexScreen()) {
    _plugins[_nPlugins++] = p;
    Log.verbose("completed setting up Plugin %s", name.c_str());
  } else {
    _dataProviders[_nDataProviders++] = p;
    Log.verbose("completed setting up DataProvider %s", name.c_str());
  }
} 

bool PluginMgr::validatePluginFiles(String pluginPath) {
  Log.verbose("Validating %s", pluginPath.c_str());
  // Make sure all the required files exist, or don't bother going any further
  if (ESP_FS::exists(pluginPath + "/plugin.json") &&
      ESP_FS::exists(pluginPath + "/form.json") &&
      ESP_FS::exists(pluginPath + "/settings.json")) return true;
      // Note that pluginPath/screen.json is optional

  Log.warning(F("For plugin %s, not all json files are present"), pluginPath.c_str());
  Log.warning(F("  plugin.json: %T"), ESP_FS::exists(pluginPath + "/plugin.json"));
  Log.warning(F("  form.json: %T"), ESP_FS::exists(pluginPath + "/form.json"));
  Log.warning(F("  settings.json: %T"), ESP_FS::exists(pluginPath + "/settings.json"));
  Log.warning(F("  screen.json: %T"), ESP_FS::exists(pluginPath + "/screen.json"));
  return false;
}

uint8_t PluginMgr::enumPlugins(const String& pluginRoot, String* pluginDirNames) {
  uint8_t nPluginsFound = 0;
  uint8_t lengthOfPIPath = pluginRoot.length();

  ESP_FS::DirEnumerator* de = ESP_FS::newEnumerator();
  if (!de->begin(pluginRoot)) {
    Log.warning("The specified plugin path (%s) is not a directory", pluginRoot.c_str());
    delete de;
    return 0;
  }

  String path;
  while (nPluginsFound < MaxPlugins && de->next(path)) {
    int firstSlash = path.indexOf('/', lengthOfPIPath+2);
    if (firstSlash == -1) continue; // Not in a subdirectory;
    String pluginDirName = path.substring(lengthOfPIPath, firstSlash);
    int i;
    for (i = 0; i < nPluginsFound; i++) {
      if (pluginDirNames[i] == pluginDirName) break;
    }
    if (i == nPluginsFound && validatePluginFiles(pluginRoot+pluginDirName)) {
      pluginDirNames[nPluginsFound++] = pluginDirName;
    }
  }
  delete de;

  return nPluginsFound;
}


/*------------------------------------------------------------------------------
 *
 * Public Functions
 *
 *----------------------------------------------------------------------------*/

void PluginMgr::setFactory(Factory theFactory) {
  factory = theFactory;
}

void PluginMgr::loadAll(String pluginRoot) {

  auto buttonHandler =[this](int id, PressType type) -> void {
    Log.verbose(F("FlexScreen button delegate: id = %d, type = %d"), id, type);
    displayNextPlugin();
  };
  FlexScreen::setButtonDelegate(buttonHandler);

  _nPlugins = 0;
  _nDataProviders = 0;
  String  pluginDirNames[MaxPlugins];
  uint8_t nPluginsFound = enumPlugins(pluginRoot, &pluginDirNames[0]);

  // Sort the plugins by name
  for (size_t i = 1; i < nPluginsFound; i++) {
    for (size_t j = i; j > 0 && (pluginDirNames[j-1] >  pluginDirNames[j]); j--) {
      String tmp = pluginDirNames[j-1];
      pluginDirNames[j-1] = pluginDirNames[j];
      pluginDirNames[j] = tmp;
    }
  }

  // Now that we have a list of unique plugin subdirectories, enumerate and load them
  for (int i = 0; i < nPluginsFound; i++) newPlugin(pluginRoot + pluginDirNames[i]);

  auto mapper = [this](const String& key, String& value) -> void { map(key, value); };
  DataBroker::registerMapper(mapper, PluginNamespacePrefix);
}

void PluginMgr::refreshAll(bool force) {
  for (int i = 0; i < _nPlugins; i++) {
    _plugins[i]->refresh(force);
  }
  for (int i = 0; i < _nDataProviders; i++) {
    _dataProviders[i]->refresh(force);
  }
}

Plugin* PluginMgr::getPlugin(uint8_t index) {
  if (index >= _nPlugins) return nullptr;
  return _plugins[index];
}

Plugin* PluginMgr::getUnifiedPlugin(uint8_t index) {
  if (index < _nPlugins) return _plugins[index];
  if (index < _nPlugins + _nDataProviders) return _dataProviders[index - _nPlugins];
  return nullptr;
}

void PluginMgr::map(const String& key, String& value) {
  int indexOfSeparator = key.indexOf('.');
  String piNamespace = key.substring(0, indexOfSeparator);
  for (int i = 0; i < _nPlugins; i++) {
    Plugin *p = _plugins[i];
    if (p->getNamespace() == piNamespace) {
      if (p->enabled()) { p->typeSpecificMapper(key.substring(indexOfSeparator+1), value); }
      return;
    }
  }
  for (int i = 0; i < _nDataProviders; i++) {
    Plugin *p = _dataProviders[i];
    if (p->getNamespace() == piNamespace) {
      if (p->enabled()) { p->typeSpecificMapper(key.substring(indexOfSeparator+1), value); }
      return;
    }
  }
}

void PluginMgr::displayPlugin(uint8_t pluginIndex) {
  if (pluginIndex >= _nPlugins) {
    curPlugin = -1;
    ScreenMgr.displayHomeScreen();
    return;
  }
  Plugin *p = _plugins[(curPlugin = pluginIndex) ];
  if (p && p->enabled()) { ScreenMgr.display(p->getFlexScreen()); }
}

void PluginMgr::displayNextPlugin() {
  uint8_t nextPlugin = (curPlugin == -1) ? 0 : (curPlugin+1);
  displayPlugin(nextPlugin);
}




