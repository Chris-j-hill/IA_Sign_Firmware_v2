#ifndef Current_Control_CPP
#define Current_Control_CPP

#include "Current_Control.h"
#ifdef ENABLE_CURRENT_METER
bool enable_current_meter = true;
#else
bool enable_current_meter = false;
#endif

#ifdef ENABLE_LIGHT_SENSORS
bool enable_LDR = true;
#else
bool enable_LDR = false;
#endif

byte screen_brightness = 100;

LDR_Struct light_sensor_parameters;
Current_Meter_Struct current_meter_parameters;

int Light_Sensor::init_LDR() {

  if (enable_LDR) {
    this -> enable();
    this -> read_sensor(1); //check connection
  }
  else
    this -> disable();
  this -> read_sensor(2);
}

void Light_Sensor::read_sensors() {

  delayMicroseconds(50);    //delay to allow accurate ADC reading
  this -> read_sensor(1);
  delayMicroseconds(50);
  this -> read_sensor(2);

  this -> avg_sensor_result();

}


byte Light_Sensor::calculate_target_brightness() {
  static int scaler = 0;
  byte temp_brightness = map(light_sensor_parameters.avg_reading, 0, 255, 0, 100);

  if (current_meter_parameters.total > current_meter_parameters.max_current_limit)
    scaler -= 2;      //reduce by 2% every loop until under current limit
  else if (scaler < 100)
    scaler += 2;

  temp_brightness += scaler; //scale the target brightness, scaler should fluctuate around 0
  temp_brightness = constrain(temp_brightness, 0, 100); // constrain result to range 0-100
  //require difference between target_brightness and current birghtness to be above threshold before update gets pushed to megas
  if (abs(screen_brightness - temp_brightness) > SCREEN_BRIGHTNESS_UPDATE_THRESHOLD)
    return temp_brightness;
  else
    return screen_brightness;

}



void Light_Sensor::enable() {
  if (!light_sensor_parameters.enabled1)
    this -> enable1();
  if (!light_sensor_parameters.enabled2)
    this -> enable2();

}

void Light_Sensor::disable() {

  this -> disable1();
  this -> disable2();

}

void Light_Sensor::enable1() {

  pinMode(light_sensor_parameters.pin1, INPUT);
  light_sensor_parameters.enabled1 = true;
}

void Light_Sensor::enable2() {

  pinMode(light_sensor_parameters.pin2, INPUT);
  light_sensor_parameters.enabled2 = true;
}

void Light_Sensor::disable1() {
  light_sensor_parameters.enabled1 = false;
}

void Light_Sensor::disable2() {
  light_sensor_parameters.enabled1 = false;
}

void Light_Sensor::read_sensor(byte sensor) {       // read the average value of the LDR's, if one fails to read correctly or large disparity fail returned

  int temp_reading = 0;
  if (sensor == 1 && light_sensor_parameters.enabled1) {

    //Serial.print("Read sensor 1: ");
    temp_reading = map(analogRead(light_sensor_parameters.pin1), 1, 1022, light_sensor_parameters.config_min1, (uint16_t)65025 / light_sensor_parameters.config_max1);
    light_sensor_parameters.reading1 = constrain(temp_reading, 0, 255); //constrain to limits, should retain infor of shorts or open circuit
    //Serial.println(light_sensor_parameters.reading1);

    if (light_sensor_parameters.reading1 == 0 || light_sensor_parameters.reading1 == 255) {
      Sprintln(F("From Light_Sensor.read_sensor: \t Problem reading light sensor 1, check wiring"));
      light_sensor_parameters.bad_connection1 = true;
    }
    else
      light_sensor_parameters.bad_connection1 = false;
  }

  else if (sensor == 2 && light_sensor_parameters.enabled2) {

    //Serial.print("Read sensor 2: ");
    temp_reading = map(analogRead(light_sensor_parameters.pin2), 1, 1022, light_sensor_parameters.config_min2, light_sensor_parameters.config_max2);
    light_sensor_parameters.reading2 = constrain(temp_reading, 0, 255);
    //Serial.println(light_sensor_parameters.reading2);

    if (light_sensor_parameters.reading2 == 0 || light_sensor_parameters.reading2 == 255) {
      Sprintln(F("From Light_Sensor.read_sensor: \t Problem reading light sensor 2, check wiring"));
      light_sensor_parameters.bad_connection2 = true;
    }
    else

      light_sensor_parameters.bad_connection2 = false;
  }
}


void Light_Sensor::avg_sensor_result() {

  static byte ring_buffer_index = 0; //index in ring buffer
  static int ring_buffer_total = 0;
  static byte ring_buffer[RING_BUFFER_LENGTH] = {0}; //ring buffer used to calculate average
  byte this_avg_reading = 0;

  if (!light_sensor_parameters.bad_connection1 && !light_sensor_parameters.bad_connection2) { //if connections fine, get average
    if (abs(light_sensor_parameters.reading1 - light_sensor_parameters.reading2) >= light_sensor_parameters.large_disparity) { //if large disparity between sensor readings, uses the higher, possibly one covered

      Sprintln(F("From LDR_check_value: \t Large disparity in sensor readings, return higher"));
      Sprint(F("sensor 1: "));
      Sprint(light_sensor_parameters.reading1);
      Sprint(F("\t sensor 2: "));
      Sprintln(light_sensor_parameters.reading2);

      if (light_sensor_parameters.reading1 > light_sensor_parameters.reading2)
        this_avg_reading = light_sensor_parameters.reading1;

      else
        this_avg_reading = light_sensor_parameters.reading2;
    }
    else {
      this_avg_reading = (light_sensor_parameters.reading1 + light_sensor_parameters.reading2) >> 1;
      Sprintln(F("From Light_Sensor.avg_sensor_result: Both sensor readings good, return average"));
      Sprint(F("Average reading: "));
      Sprintln(this_avg_reading);
    }
  }

  else { // return good result if one one available
    if (!light_sensor_parameters.bad_connection1 && light_sensor_parameters.bad_connection2) {  //if one sensor is usable just return the value
      this_avg_reading = light_sensor_parameters.reading1;
      Sprintln(F("From Light_Sensor.avg_sensor_result: \t Return sensor 1 reading"));
    }

    else if (light_sensor_parameters.bad_connection1 && !light_sensor_parameters.bad_connection2) {
      this_avg_reading = light_sensor_parameters.reading2;
      Sprintln(F("From Light_Sensor.avg_sensor_result: \t Return sensor 2 reading"));
    }

    else {  //if neither usable set avg as highest value
      this_avg_reading = 255;
      Sprintln(F("From Light_Sensor.avg_sensor_result: \t Error reading both sensors, check connections"));
    }

  }

  ring_buffer_total -= ring_buffer[ring_buffer_index];    //subtract off oldest value
  ring_buffer_total += this_avg_reading;                  //add new value to sum
  ring_buffer[ring_buffer_index] += this_avg_reading;     //log new value in array
  light_sensor_parameters.avg_reading = ring_buffer_total >> RING_BUFFER_SHIFT; //divide by number of indexes for rolling avg

  ring_buffer_index = (ring_buffer_index + 1) % RING_BUFFER_LENGTH; //move index
}

void Light_Sensor::get_readings() {
  static int previous_ldr_read_time = millis();

  if (millis() > LDR_UPDATE_PERIOD + previous_ldr_read_time) {
    previous_ldr_read_time = millis();
    read_sensors();
  }
}


#endif  // Current_Control_CPP
