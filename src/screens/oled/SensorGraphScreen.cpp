#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_OLED

#include "SensorGraphScreen.h"

AQIGraphScreen::AQIGraphScreen(const AQIMgr& mgr)
  : SensorGraphScreen(mgr, 1, "AQI")
{
  displayParams = { true, 0.0, 100.0 };
}

float AQIGraphScreen::AQIGraphScreen::getVal(uint16_t index) const {
  return _mgr.buffers[_range].peekAt(index).aqi;
}



WeatherGraphScreen::WeatherGraphScreen(const WeatherMgr& mgr)
  : SensorGraphScreen(mgr, 1, "Temp")
{
  displayParams = { true, 0.0, 100.0 };
}

float WeatherGraphScreen::getVal(uint16_t index) const {
  return _mgr.buffers[_range].peekAt(index).temp;
}

#endif