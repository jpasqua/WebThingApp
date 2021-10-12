#ifndef HWConfig_h
#define HWConfig_h

//
// A Hardware configuration is a bundle of definitions that describe a
// particular collection of hardware such as what kind of display is attached,
// which pins are being used as buttons, which pins to use for standard
// functions (e.g. SCL and SDA). Most of the definitions are runtime constants,
// but some may be compile-time constants (#define's)
//
// This file contains a number of predefined configurations. You can:
// a) choose from one of the predefined configurations
// b) add a new configuration if you think it is common
// c) modify the Custom config to match your unique configuration
//    your hardware. 
//
// This file is specific to hard3are configurations which contain a
// DEVICE_TYPE_OLED display device. It can be used as a basis for
// HWConfig files with other types of display, or no display
//

// SECTION 0: Generic definitions
#define UNUSED_PIN 255

// SECTION 1: Select the general class of Display Device we're using.
// The available types are defined in DeviceTypes.h
// This config provides options based on DEVICE_TYPE_OLED.
#include <gui/devices/DeviceTypes.h>
#define DEVICE_TYPE DEVICE_TYPE_OLED

// SECTION 2: [BOILERPLATE] Include Display.h to get the list of specific device
// types that are available
#include <gui/Display.h>

// SECTION 3: [BOILERPLATE] Define a list of the predefined hardware configs
// that we can choose from. If you add a new configuration, list it here.
#define Config_Custom        1
#define Config_EmbeddedOLED  2
#define Config_D1Mini        3
#define Config_ESP32Mini     4
#define Config_ESP32WithOLED 5

// SECTION 4: [CUSTOMIZE] Choose a specific configuration
#define SelectedConfig Config_D1Mini

// SECTION 5: The definitions of the available configurations
// Add new configs below if you add an option
#if (SelectedConfig == Config_D1Mini)
  // ----- Config Info for D1Mini with 1.4" SH1106 display

  // ----- I2C Settings
  constexpr uint8_t SDA_PIN = D2;
  constexpr uint8_t SCL_PIN = D5;

  // ----- Display Type
  constexpr auto DISPLAY_DRIVER = DisplayDeviceOptions::DeviceType::SH1106;
  constexpr uint8_t DISPLAY_I2C_ADDRESS = 0x3c;

  // ----- Buttons
  constexpr uint8_t physicalButtons[] = { D6 };
  constexpr uint8_t syntheticGrounds[] = { D8 };

#elif (SelectedConfig == Config_ESP32Mini)
  // -----Config Info for ESP32D1Mini with 1.4" SH1106 display

  // ----- I2C Settings
  constexpr uint8_t SDA_PIN = 21;
  constexpr uint8_t SCL_PIN = 22;

  // ----- Display Type
  constexpr auto DISPLAY_DRIVER = DisplayDeviceOptions::DeviceType::SH1106;
  constexpr uint8_t DISPLAY_I2C_ADDRESS = 0x3c;

  // ----- Buttons
  constexpr uint8_t physicalButtons[] = { 13 };
  constexpr uint8_t syntheticGrounds[] = { UNUSED_PIN };

#elif (SelectedConfig == Config_ESP32WithOLED)
  //----- Config Info for ESP32 with embedded 0.96" OLED

  // ----- I2C Settings
  constexpr uint8_t SDA_PIN = 5;
  constexpr uint8_t SCL_PIN = 4;

  // ----- Display Type
  constexpr auto DISPLAY_DRIVER = DisplayDeviceOptions::DeviceType::SSD1306;
  constexpr uint8_t DISPLAY_I2C_ADDRESS = 0x3c;
  
  // ----- Buttons
  constexpr uint8_t physicalButtons[] = { 13 };
  constexpr uint8_t syntheticGrounds[] = { UNUSED_PIN };

#elif (SelectedConfig == Config_EmbeddedOLED)
  // ----- Config Info for Wemos board with embedded 0.96" OLED

  // ----- I2C Settings
  constexpr uint8_t SDA_PIN = D1;
  constexpr uint8_t SCL_PIN = D2;

  // ----- Display Type
  constexpr auto DISPLAY_DRIVER = DisplayDeviceOptions::DeviceType::SSD1306;
  constexpr uint8_t DISPLAY_I2C_ADDRESS = 0x3c;

  // ----- Buttons
  constexpr uint8_t physicalButtons[] = { D3 };
  constexpr uint8_t syntheticGrounds[] = { UNUSED_PIN };

#elif (SelectedConfig ==  Config_Custom)
  /*------------------------------------------------------------------------------
   *
   * Custom Config - put your settings here
   *
   *----------------------------------------------------------------------------*/
  // ----- I2C Settings
  constexpr uint8_t SDA_PIN = D2;  // Substitute the correct Pin for your config
  constexpr uint8_t SCL_PIN = D5;  // Substitute the correct Pin for your config

    // ----- Display Type
    // #define DISPLAY_DRIVER   SSD1306
  constexpr auto DISPLAY_DRIVER = DisplayDeviceOptions::DeviceType::SH1106;
  //constexpr auto DISPLAY_DRIVER = DisplayDeviceOptions::DeviceType::SSD1306;
  constexpr uint8_t DISPLAY_I2C_ADDRESS = 0x3c;

  // ----- Buttons
  constexpr int8_t physicalButtons[] = { D3 };
  constexpr int8_t syntheticGrounds[] = { UNUSED_PIN };
#else
    #error "Please set SelectedConfig"
#endif


class HWConfig {
public:
  struct CorePins {
    uint8_t scl;
    uint8_t sda;
  };

  const CorePins corePins;
  const DisplayDeviceOptions displayDeviceOptions;
  const uint8_t* physicalButtons;
  const uint8_t  nPhysicalButtons;
  const uint8_t* syntheticGrounds;
  const uint8_t  nSyntheticGrounds;
};

constexpr HWConfig hwConfig {
  { SCL_PIN, SDA_PIN },
  { DISPLAY_DRIVER, SCL_PIN, SDA_PIN, DISPLAY_I2C_ADDRESS },
  physicalButtons, ARRAY_SIZE(physicalButtons),  
  syntheticGrounds, ARRAY_SIZE(syntheticGrounds) 
};

#endif  // HWConfig_h
