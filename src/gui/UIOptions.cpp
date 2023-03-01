#include <ArduinoLog.h>
#include "UIOptions.h"

BrightnessSchedule::BrightnessSchedule() {
  active = true;
  morning.hr =  8; morning.min = 0; morning.brightness = 100;
  evening.hr = 20; evening.min = 0; evening.brightness = 20;
}

void BrightnessSchedule::fromJSON(const JsonDocument &doc) {
  active = doc[F("scheduleActive")];
  morning.hr = doc[F("morning")][F("hr")]|8;
  morning.min = doc[F("morning")][F("min")]|0;
  morning.brightness = doc[F("morning")][F("brightness")]|100;
  evening.hr = doc[F("evening")][F("hr")]|20;
  evening.min = doc[F("evening")][F("min")]|0;
  evening.brightness = doc[F("evening")][F("brightness")]|20;
}

void BrightnessSchedule::toJSON(JsonDocument &doc) {
  doc[F("scheduleActive")] = active;

  doc[F("morning")][F("hr")] = morning.hr;
  doc[F("morning")][F("min")] = morning.min;
  doc[F("morning")][F("brightness")] = morning.brightness;

  doc[F("evening")][F("hr")] = evening.hr;
  doc[F("evening")][F("min")] = evening.min;
  doc[F("evening")][F("brightness")] = evening.brightness; 
}

void BrightnessSchedule::logSettings() {
  Log.verbose(F("Schedules (active: %T)"), active);
  Log.verbose(F("  Morning: [time: %d:%d, %d]"), morning.hr, morning.min, morning.brightness);
  Log.verbose(F("  Evening: [time: %d:%d, %d]"), evening.hr, evening.min, evening.brightness);
}

UIOptions::UIOptions() {
  use24Hour = false;
  useMetric = false;
  showDevMenu = false;
  screenBlankMinutes = 0;
}

void UIOptions::fromJSON(const JsonDocument &doc) {
  use24Hour = doc[F("use24Hour")];
  useMetric = doc[F("useMetric")];
  showDevMenu = doc[F("showDevMenu")];
  screenBlankMinutes = doc[F("screenBlankMinutes")];
  schedule.fromJSON(doc);
}

void UIOptions::toJSON(JsonDocument &doc) {
  doc["showDevMenu"] = showDevMenu;
  doc[F("use24Hour")] = use24Hour;
  doc[F("useMetric")] = useMetric;
  doc[F("screenBlankMinutes")] = screenBlankMinutes;
  schedule.toJSON(doc);
}

void UIOptions::logSettings() {
  Log.verbose(F("UI Settings"));
  Log.verbose(F("  use24Hour: %T"), use24Hour);
  Log.verbose(F("  useMetric: %T"), useMetric);
  Log.verbose(F("  show dev menu: %T"), showDevMenu);
  Log.verbose(F("  Blank screen in N minutes: %d"), screenBlankMinutes);
  schedule.logSettings();
}