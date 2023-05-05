#ifndef DeviceSelect_h
#define DeviceSelect_h

#include "DeviceTypes.h"

#ifndef DEVICE_TYPE
	#define DEVICE_TYPE DEVICE_TYPE_TOUCH
	// #define DEVICE_TYPE DEVICE_TYPE_OLED
	// #define DEVICE_TYPE DEVICE_TYPE_MTX
#endif

#define IDENT(x) x
#define XSTR(x) #x
#define STR(x) XSTR(x)
#define PATH(x,y) STR(IDENT(x)IDENT(y))

#if DEVICE_TYPE == DEVICE_TYPE_TOUCH
	#define PREFIX devices/Touch/Touch_
#elif DEVICE_TYPE == DEVICE_TYPE_OLED
	#define PREFIX devices/OLED/OLED_
#elif DEVICE_TYPE == DEVICE_TYPE_MTX
	#define PREFIX devices/LEDMatrix/MTX_
#endif

#define DeviceImplFor(module)  PATH(PREFIX, module.h)

#endif // DeviceSelect_h