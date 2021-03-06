

#ifndef Due_Pins_H
#define Due_Pins_H      //define to stop double declarations
#include "Arduino.h"

    //      due pins

//also uses SDA1 and SCL1 and spi pins
#define PIN_A       5   //clk
#define PIN_B       6   //dt
#define BUTTON_PIN  7

#define LED_STRIP   8
#define FAN_PIN     9

#define LDR1        A0      // these require external pullup resistors
#define LDR2        A1      // maybe change to none adjacent pins, adjacent pins seem to interfere significantly if one is floating

#define CURRENT_METER_1 A2  
#define CURRENT_METER_2 A3      

#define TEMP1       26  //read using one wire protocol
#define TEMP2       27
#define TEMP3       28

#define SD_LED_RED_PIN  22  //choose hardware pwm (possible addition to add software pwm, not many timers left though)
#define SD_LED_GREEN_PIN 23
#define SD_LED_BLUE_PIN 24

#define SD2_ENABLE  46    //internal sd card CS pin
#define SD1_ENABLE  47    //external sd card CS pin


//NB: configure ports to run on hardware serial if possible, should auto configure in setup code
#define RX_PIN_1 42 // software serial port's reception pin
#define TX_PIN_1 43 // software serial port's transmision pin
#define RX_PIN_2 17 
#define TX_PIN_2 16 
#define RX_PIN_3 19 
#define TX_PIN_3 18 
#define RX_PIN_4 15 
#define TX_PIN_4 14 


#endif // Due_Pins_H
