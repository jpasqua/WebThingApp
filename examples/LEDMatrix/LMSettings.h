/*
 * LMSettings.h
 *    Setting for the LMCurrencyApp.
 *
 */

#ifndef LMSettings_h
#define LMSettings_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <BPA_PrinterSettings.h>
//                                  WebThing Includes
#include <WTAppSettings.h>
//                                  Local Includes
#include "LEDMatrixApp.h"
//--------------- End:    Includes ---------------------------------------------

struct PrinterFields {
  bool printerName;
  bool fileName;
  bool pct;
  bool completeAt;

  void fromJSON(JsonObjectConst &obj);
  void toJSON(JsonObject &obj);
  void logSettings();
};

class LMSettings: public WTAppSettings {
public:
  // ----- Constructors and methods
  LMSettings();
  void fromJSON(const JsonDocument &doc) override;
  void toJSON(JsonDocument &doc);
  void logSettings();

  struct {
    String username;
    String key;
    String groupName;
  } aio;

  PrinterFields singlePrinter, allPrinters;
  enum {HSP_Horizontal, HSP_Vertical, HSP_None} homeScreenProgress;

  uint16_t scrollDelay;

  static constexpr uint8_t MaxPrinters = 4;
  bool printMonitorEnabled = false;
  PrinterSettings printer[MaxPrinters];
  uint32_t printerRefreshInterval = 10;

private:
  // ----- Constants -----
  static constexpr uint32_t CurrentVersion = 0x0001;
  void internalizePrinterFields(PrinterFields* fields, JsonObjectConst &obj);
};
#endif // LMSettings_h