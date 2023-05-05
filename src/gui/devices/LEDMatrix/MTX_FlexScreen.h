#include "../DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX


class FlexScreen : public BaseFlexScreen {
public:
	static constexpr uint16_t MinDelayBetweenFrames = 20;	// 20 ms
	
	// ----- Functions defined in Screen class
  virtual void display(bool activating = false) override;
  virtual void processPeriodicActivity() override;

private:
  String _text;
  uint8_t _fontID;
  uint16_t _textWidth;
  uint16_t _baseline;

  uint16_t _offset = 0;
  uint16_t _mtxWidth;

  uint32_t _delayBetweenFrames = 20;
  uint32_t _nextTimeToDisplay = 0;

  void prepForDisplay();
  void innerDisplay();
};
#endif 	// DEVICE_TYPE_MTX
