/*
 * CurrencyMon:
 *    A monitor for a Green Mountain Grills BBQ 
 *
 * NOTES:
 * o This is just boilerplate that initializes the app from the setup() function
 *   and gives the app cycles from the loop() function.
 * o The only thing that changes here from one WebThingApp to another is the
 *   definition of MainClassName.
 *
 */

// CUSTOM: Define the name of your main class here. Don't change anything else
#define MainClassName OLEDTestApp

#define str(s) #s
#define HEADER(c)  str(c.h)

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include HEADER(MainClassName)
//--------------- End:    Includes ---------------------------------------------


void setup() {
  MainClassName::create();  // Creates and starts the WTApp singleton
}

void loop() { 
  wtAppImpl->loop();  // Passes control to the app's loop function
}
