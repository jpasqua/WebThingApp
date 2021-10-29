#ifndef GraphScreen_h
#define GraphScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
//                                  Local Includes
#include "../../WTApp.h"
#include "../../gui/Display.h"
#include "../../gui/Region.h"
#include "../../gui/Theme.h"
#include "../../gui/ScreenMgr.h"
//--------------- End:    Includes ---------------------------------------------


class GraphDataProvider {
public:
  GraphDataProvider() = default;

  virtual float hiVal(bool recompute) const {
    if (recompute || (minVal > maxVal)) findMinMax();
    return maxVal;
  }

  virtual float loVal(bool recompute) const {
    if (recompute || (minVal > maxVal)) findMinMax();
    return minVal;
  }

  virtual uint16_t count() const = 0;
  virtual float getVal(uint16_t index) const = 0;
  virtual void getXLabels(String& xMin, String& xMax) const = 0;
  
  virtual String getXLegend() const { return ""; };
  virtual String getYLegend() const { return ""; };

protected:  
  virtual void findMinMax() const {
    uint16_t size = count();
    for (int i = 0; i < size; i++) {
      float cur = getVal(i);
      if (cur < minVal) minVal = cur;
      if (cur > maxVal) maxVal = cur;
    }
  }

  mutable float minVal = std::numeric_limits<float>::max();;
  mutable float maxVal = std::numeric_limits<float>::min();
};


class GraphDisplayParams {
public:
  bool autoScale;
  float suggestedYAxisMin;
  float suggestedYAxisMax;
};

class GraphScreen : public Screen {
public:
  GraphScreen();

  virtual void display(bool);
  virtual void processPeriodicActivity() { }

  void setDataProvider(GraphDataProvider* provider) { dataProvider = provider; }
  void setDisplayParams(GraphDisplayParams& params) {displayParams = params; }

protected:
  GraphDataProvider* dataProvider;
  GraphDisplayParams displayParams;
};

#endif	// GraphScreen_h