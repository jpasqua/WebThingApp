#include <ArduinoLog.h>
#include "DisplayOptions.h"

CalibrationData::CalibrationData() {
  for (int i = 0; i < nCalReadings; i++) {
    readings[i] = 0;
  }
}

void CalibrationData::fromJSON(JsonDocument &doc) {
  for (int i = 0; i < nCalReadings; i++) {
    readings[i] = doc[F("calibrationData")][i];
  }
}

void CalibrationData::toJSON(JsonDocument &doc) {
  JsonArray cd = doc.createNestedArray(F("calibrationData"));
  for (int i = 0; i < nCalReadings; i++) {
    cd.add(readings[i]);
  }
}

void CalibrationData::logSettings() {
  Log.verbose(F("  CalibrationData: ["));
  for (int i = 0; i < nCalReadings; i++) {
    Log.verbose(F("    %d,"), readings[i]);
  }
  Log.verbose(F("  ]"));
}

DisplayOptions::DisplayOptions() {
  invertDisplay = false;
}

void DisplayOptions::fromJSON(JsonDocument &doc) {
  invertDisplay = doc[F("invertDisplay")];
  calibrationData.fromJSON(doc);
}

void DisplayOptions::toJSON(JsonDocument &doc) {
  doc[F("invertDisplay")] = invertDisplay;
  calibrationData.toJSON(doc);
}

void DisplayOptions::logSettings() {
  Log.verbose(F("Display Settings"));
  Log.verbose(F("  invertDisplay: %T"), invertDisplay);
  calibrationData.logSettings();
}

