#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
//                                  Local Includes
#include "GraphScreen.h"
#include "../../gui/Display.h"
//--------------- End:    Includes ---------------------------------------------

// TEST
// static constexpr float values[] {
//   19.7, 20.4, 16.4, 22.0, 23.3, 18.8, 15.3, 22.4, 16.9, 17.0,
//   21.9, 15.2, 18.5, 18.7, 17.3, 15.1, 20.8, 20.6, 15.4, 23.9,
//   15.6, 21.3, 18.1, 22.1, 16.1, 20.1, 20.2, 16.8, 18.3, 23.5,
//   19.6, 16.7, 23.0, 23.6, 17.8, 23.1, 23.2, 23.8, 21.5, 22.2,
//   22.8, 19.4, 17.2, 18.6, 17.4, 15.8, 16.6, 20.9, 22.7, 15.5,
//   19.1, 16.5, 21.2, 19.5, 15.0, 17.1, 22.9, 15.7, 23.7, 18.4,
//   20.3, 16.2, 19.0, 20.5, 21.1, 19.8, 16.0, 16.3, 17.7, 19.3,
//   18.0, 23.4, 18.2, 17.9, 21.0, 21.6, 18.9, 21.4, 22.6, 21.8,
//   19.9, 15.9, 20.7, 19.2, 17.6, 20.0, 17.5, 22.3, 21.7, 22.5,
//   18.0, 23.4, 18.2, 17.9, 21.0, 21.6, 18.9, 21.4, 22.6, 21.8,
//   20.3, 16.2, 19.0, 20.5, 21.1, 19.8, 16.0, 16.3, 17.7, 19.3,
//   22.8, 19.4, 17.2, 18.6, 17.4, 15.8, 16.6, 20.9, 22.7, 15.5,
//   15.6, 21.3, 18.1, 22.1, 16.1, 20.1, 20.2, 16.8, 18.3, 23.5,
//   19.7, 20.4, 16.4, 22.0, 23.3, 18.8, 15.3, 22.4, 16.9, 17.0
// };
// static constexpr uint16_t nValues = ARRAY_SIZE(values);

// class TestProvider : public GraphDataProvider {
// public:
//   virtual uint16_t count() const { return(nValues); }
//   virtual float hiVal() const { return(23.9); }
//   virtual float loVal() const { return(15.0); }

//   virtual float getVal(uint16_t index) const { return values[index]; }

//   virtual bool autoScale() const { return true; }
//   virtual float suggestedYAxisMin() const { return 0; }
//   virtual float suggestedYAxisMax() const { return 100; }
// };
// TestProvider testProvider;

// GraphDisplayParams testParams {
//   true,
//   0.0, 100.0,
//   "10:22 and a bunch of other text",
//   "23:07 and a bunch of other text"
// };

static constexpr Region GraphRegion {27, 0, 100, 54};

static const char* fmts[] {
  "%.2f",   // <            10
  "%.1f",   // <           100
  "%.0f",   // <         1,000
  "%.1fK",  // <        10,000
  "%.0fK",  // <       100,000
  "%.0fK",  // <     1,000,000
  "%.1fM",  // <    10,000,000
  "%.0fM",  // <   100,000,000
  "%.0fM"   // < 1,000,000,000
};
static size_t nFmts = sizeof(fmts)/sizeof(fmts[0]);

static String fourDigits(float val) {
  char buf[6];  // Space for -123M[NULL]
  bool negative = (val < 0);
  if (negative) val = -val;
  float cutoff = 10;

  for (uint16_t i = 0; i < nFmts; i++) {
    if (val < cutoff) {
      val /= pow(10, (i/3)*3);
      if (negative) val = -val;
      sprintf(buf, fmts[i], val);
      return(String(buf));
    }
    cutoff *= 10;
  }
  return String(negative ? "----" : "++++");
}

GraphScreen::GraphScreen() {
  // TEST
  // dataProvider = &testProvider;
  // displayParams = &testParams;
}

void GraphScreen::display(bool) {
  auto& oled = Display.oled;

  oled->clear();

  // Draw the axes:
  oled->setColor(OLEDDISPLAY_COLOR::WHITE);
  oled->drawHorizontalLine(GraphRegion.x, GraphRegion.bottom(), GraphRegion.w);
  oled->drawVerticalLine(GraphRegion.x, GraphRegion.y, GraphRegion.h);

  uint16_t nPoints = dataProvider->count();
  float hiVal = dataProvider->hiVal();
  float loVal = dataProvider->loVal();

  float yAxisMin = displayParams.suggestedYAxisMin;
  float yAxisMax = displayParams.suggestedYAxisMax;
  if (displayParams.autoScale) {
    yAxisMin = loVal * 0.95;
    yAxisMax = hiVal * 1.05;
  } else {
    if (hiVal > yAxisMax) yAxisMax = hiVal;
    if (loVal < yAxisMin) yAxisMin = loVal;
  }
  float range = yAxisMax - yAxisMin;

  Display.setFont(Display.FontID::M5);
  oled->setTextAlignment(TEXT_ALIGN_RIGHT);
  oled->drawString(GraphRegion.x-2, GraphRegion.bottom()-7, fourDigits(yAxisMin));
  oled->drawString(GraphRegion.x-2, GraphRegion.y, fourDigits(yAxisMax));

  String legend = dataProvider->getYLegend();
  if (legend.length() > 5) legend.remove(5);
  oled->drawString(GraphRegion.x-2, GraphRegion.y + GraphRegion.h/2, legend);

  uint16_t maxXLabelWidth = GraphRegion.w/2 - 5;  // Split the space and leave a buffer
  oled->setTextAlignment(TEXT_ALIGN_LEFT);
  String minLabel, maxLabel;
  dataProvider->getXLabels(minLabel, maxLabel);
  oled->drawStringMaxWidth(GraphRegion.x, Display.Height-7, maxXLabelWidth, minLabel);
  oled->setTextAlignment(TEXT_ALIGN_RIGHT);
  oled->drawStringMaxWidth(Display.Width, Display.Height-7, maxXLabelWidth, maxLabel);
  legend = dataProvider->getXLegend();
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  oled->drawString(GraphRegion.x+GraphRegion.w/2, Display.Height-7, legend);

  // Draw the actual graph contents
  uint16_t lastX, lastY;
  uint16_t bottom = GraphRegion.bottom();
  float x = GraphRegion.x;
  float deltaX = ((float)GraphRegion.w)/(nPoints-1);
  for (int i = 0; i < nPoints; i++) {
    float pctOfRange = ((dataProvider->getVal(i) - yAxisMin)/range);
    uint16_t y = bottom - (pctOfRange * GraphRegion.h);
    if (i) {
      // Log.verbose("(%d, %d) -> (%F, %d)", lastX, lastY, x, y);
      oled->drawLine(lastX, lastY, x, y);
    }
    lastX = x; lastY = y;
    x += deltaX;
  }

  oled->display();
}

#endif