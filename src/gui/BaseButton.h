#ifndef BaseButton_h
#define BaseButton_h

class BaseButton {
public:
  using PressType = enum {NormalPress, LongPress, VeryLongPress};
  using ButtonCallback = std::function<void(int, PressType)>;
  
  static constexpr uint32_t LongPressInterval = 500;
  static constexpr uint32_t VeryLongPressInterval = 1000;
};

#endif	// BaseButton_h