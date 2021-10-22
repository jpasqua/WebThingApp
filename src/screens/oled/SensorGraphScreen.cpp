#include "SensorGraphScreen.h"

AQIGraphScreen::AQIGraphScreen(const AQIMgr& mgr)
  : SensorGraphScreen(mgr, "AQI")
{
  displayParams = { true, 0.0, 100.0 };
}

float AQIGraphScreen::AQIGraphScreen::getVal(uint16_t index) const {
  return _mgr.buffers[_range].peekAt(index).aqi;
}



WeatherGraphScreen::WeatherGraphScreen(const WeatherMgr& mgr)
  : SensorGraphScreen(mgr, "Temp")
{
  displayParams = { true, 0.0, 100.0 };
}

float WeatherGraphScreen::getVal(uint16_t index) const {
  return _mgr.buffers[_range].peekAt(index).temp;
}
