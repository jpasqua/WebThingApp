#ifndef MTX_Theme_h
#define MTX_Theme_h


namespace Theme {
  constexpr uint16_t RGB888toRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
  }
  constexpr uint16_t Color_WHITE = RGB888toRGB565(255, 255, 255);
  constexpr uint16_t Color_BLACK = RGB888toRGB565(  0,   0,   0);

  constexpr uint16_t Color_Online = Color_WHITE;
  constexpr uint16_t Color_Offline = Color_WHITE;
  constexpr uint16_t Color_Progress = Color_WHITE;
  constexpr uint16_t Color_Inactive = Color_WHITE;

  constexpr uint16_t Color_AlertError = Color_WHITE;
  constexpr uint16_t Color_AlertGood = Color_WHITE;
  constexpr uint16_t Color_AlertWarning = Color_WHITE;

  constexpr uint16_t Color_NormalText = Color_WHITE;

  constexpr uint16_t Color_Border = Color_WHITE;
  constexpr uint16_t Color_Background = Color_BLACK;

  constexpr uint16_t Color_SplashBkg  = Color_BLACK;
  constexpr uint16_t Color_SplashText = Color_WHITE;

  constexpr uint16_t Color_WeatherTxt = Color_WHITE;
  constexpr uint16_t Color_WeatherBkg = Color_WHITE;

  constexpr uint16_t Color_WiFiBlue = Color_WHITE;

  constexpr uint16_t Color_WebRequest = Color_WHITE;
  constexpr uint16_t Color_UpdatingWeather = Color_WHITE;
  constexpr uint16_t Color_UpdatingPlugins = Color_WHITE;
  constexpr uint16_t Color_UpdatingText = Color_WHITE;
  constexpr uint16_t Color_UpdatingFill = Color_BLACK;
}

#endif  // MTX_Theme_h
