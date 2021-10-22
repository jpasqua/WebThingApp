#include <gui/devices/DeviceSelect.h>
#if DEVICE_TYPE == DEVICE_TYPE_OLED

#ifndef HistoryBufferDataProvider_h
#define HistoryBufferDataProvider_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <HistoryBuffer.h>
//                                  Local Includes
#include <screens/oled/GraphScreen.h>
//--------------- End:    Includes ---------------------------------------------


class HistoryBufferDataProvider : public GraphDataProvider {
public:
  HistoryBufferDataProvider(const HistoryBufferBase& buffer, const char* yLegend)
    : _buffer(buffer), _yLegend(yLegend)
  { }

  virtual uint16_t count() const override { return _buffer.size(); };

  virtual void getXLabels(String& xMin, String& xMax) const override {
    time_t start, end;
    _buffer.getTimeRange(start, end);
    int32_t tzOffset = WebThing::getGMTOffset();

    WebThing::TimeFormatOptions tfo {true, true, false, false};
    xMin = WebThing::formattedTime(start+tzOffset, tfo);
    xMax = WebThing::formattedTime(end+tzOffset, tfo);
  }

  virtual String getXLegend() const override {
    String result(_buffer._name);
    result[0] = toupper(_buffer._name[0]);
    return result;
  }

  virtual String getYLegend() const override { return _yLegend; }

protected:
  const HistoryBufferBase& _buffer;
  const char* _yLegend;
};



#endif  // HistoryBufferDataProvider_h
#endif
