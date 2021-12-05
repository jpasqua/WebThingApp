#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_OLED

#ifndef SensorGraphScreen_h
#define SensorGraphScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <screens/oled/HistoryBufferDataProvider.h>
#include <sensors/WeatherMgr.h>
#include <sensors/AQIMgr.h>
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

template <typename SensorMgr>
class SensorGraphScreen : public HistoryBufferDataProvider, public GraphScreen {
public:
  SensorGraphScreen(const SensorMgr& mgr, uint8_t range, const char* yLegend)
    : _mgr(mgr), _range(range)
  {
    HistoryBufferDataProvider::init(&_mgr.buffers[_range], yLegend);
    dataProvider = this;
  }

  void selectBuffer(uint8_t range) {
    _range = range;
    _buffer = &_mgr.buffers[(_range = range)];
  }

  const SensorMgr& _mgr;
  uint8_t _range = 1;
};

class AQIGraphScreen : public SensorGraphScreen<AQIMgr> {
public:
  AQIGraphScreen(const AQIMgr& mgr);
  float getVal(uint16_t index) const override;
};

class WeatherGraphScreen : public SensorGraphScreen<WeatherMgr> {
public:
  WeatherGraphScreen(const WeatherMgr& mgr);
  float getVal(uint16_t index) const override;
};

#endif  // SensorGraphScreen_h
#endif
