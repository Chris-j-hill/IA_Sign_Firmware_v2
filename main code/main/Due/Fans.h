
#ifndef Fans_H
#define Fans_H

#include "Config_Local.h"
#include "function_declarations.h"

#ifdef ENABLE_FANS
bool enable_fans = true;   //initialise on startup?   
else
bool enable_fans = false; 
#endif

#ifdef ENABLE_TEMPERATURE_SENSORS
bool enable_temp_sensor = true;
#else
bool enable_temp_sensor = false;
#endif

bool fans_enabled = false; //to check if initialised sucessfully
bool temp_sensor_enabled = false;

#define FAN_DEFAULT_SPEED 180 

byte temp1 = 4;
byte temp2 = 5;
byte temp3 = 6;


struct Temp_sensor{

  int pin1 = TEMP1;
  int pin2 = TEMP2;
  int pin3 = temp3;

  bool temp1_enable = true;
  bool temp2_enable = true;
  bool temp3_enable = true;

  byte dat1[5];         //data array for temp and humidity
  byte dat2[5];
  byte dat3[5];

  int temp1 = -100;   //current temperature as read by sensor, set to something rediculous to start
  int temp2 = -100;
  int temp3 = -100;

  int humidity1 = -100;   //current humidity as read by sensor
  int humidity2 = -100;
  int humidity3 = -100;
};
Temp_sensor temp;

//byte fan_speed = FAN_DEFAULT_SPEED;

struct Fan_Struct {
  int fan_pin = FAN_PIN;                  // pin num
  int fan_target_speed = FAN_DEFAULT_SPEED;              // target value to approach, not necessicarily the current speed
  int fan_current_speed = 0;              // current value written to fan
  int fan_change_increment = 5;           // value to increment by to approach target
  int fan_change_interval = 50;           // milliseconds to wait between incrementing value
  int fan_minimum = 100;                  // minimum value where the fans are on
  bool fan_on  = true;                    // is fan enabled, if so, run functions

};
//Fan_Struct fan;        //create fan struct

int attach_timer_fan();

void fade_fans() {          // interrupt to change the current value of the fans to approach the target value

}

int fans_set_freq(){}      //interrupt to set the frequency the fans are adjusted




//class for both fan cantrol and temperature monitoring  
class Fans{

  private:

    int poll_temperature_sensor (int pin);       // read temp sensor registers, used to update stored value to most recent
    byte read_temp_data_from_register (int pin);        // convert incoming bits from temp sensor to a byte
    
  public:
        Fan_Struct fan1;        //create fan struct
        Fans(){}
        
          //initialisations of individual items, set pins, test if working if applicable, etc
        int init_fans();
        int init_temp_sensors();            
        int writeFanSpeed(int newValue);
    
        int get_temperature (int pin);       // read temperature based on specified pin, return value in degrees C


//TODO

    int set_fan(int newValue);
    int is_fan_on();                  //useful for menu display
    int return_fan_speed();
    int get_humidity();       //because we can
    int set_temp_sensor_enable(int sensor);   //code to enable sensors, might be useful to toggle them, maybe autotoggle if not working?
    int set_temp_sensor_disable(int sensor);


};


#endif // Fans_H