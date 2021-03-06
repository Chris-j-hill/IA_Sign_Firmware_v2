
#ifndef Host_CPP
#define Host_CPP

#include "Arduino.h"
#include "Host.h"
#include "Config_local.h"
#include "Fans.h"
#include "function_declarations.h"
#include "Led_strip.h"
#include "Encoder.h"
#include "Menu_Tree.h"
#include "Graphics.h"
#include "Coms.h"
#include "Current_Control.h"
#include "SD_Cards.h"

extern Menu menu;

Serial_Sub_Menu serial_sub_menu_items;

//access to structs
extern struct Temp_sensor temp_parameters;
extern struct Fan_Struct fan_parameters;
extern struct Timers timers;
extern struct Led_Strip_Struct led_strip_parameters;
extern struct Encoder_Struct encoder_parameters;
extern struct Button_Struct button_parameters;
extern struct Menu_tree_items menu_items;
extern struct Menu_tree_menu_limits menu_limits;
extern struct Frame text_frame;
extern struct LDR_Struct light_sensor_parameters;
extern struct Current_Meter_Struct current_meter_parameters;
extern struct Text text_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Text_cursor text_cursor[MAX_NUM_OF_TEXT_OBJECTS];

extern struct SD_Card card1;
extern struct SD_Card card2;
extern struct SD_Strings SD_string;

extern struct Frame_History text_frame_history[NUM_SCREENS];
extern struct Frame_History menu_frame_history[NUM_SCREENS];
extern struct Frame_History sensor_data_frame_history[NUM_SCREENS];
extern struct Frame_History pos_frame_history[NUM_SCREENS];




extern byte screen_mode;
extern byte screen_brightness;

extern char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE];

String last_command = "text0";

String pin_error_msg = "Error: Cannot change pin value during operation, aborting assignment";
String string_write_error_msg = "Error: Cannot write string from serial monitor interface";
String frame_hist_error_msg = "Error: Cannot modify frame history buffers or counters";

//some frequenly used strings
String space_colon_string = " : ";
String dash_space_string = "- ";
String tab_string = "\t";
String space_string = " ";
String percent_string = "%";
String divide_string = "/";
String comma_space_string = ", ";
String yes_string = "Yes";
String no_string = "No ";
String y_string = "Y";
String n_string = "N";
String pointer_string = "-> ";

inline void space() {
  Serial.print(space_string);
};

inline void tab() {
  Serial.print(tab_string);
};

inline void space_colon() {
  Serial.print(space_colon_string);
};

inline void dash_space() {
  Serial.print(dash_space_string);
};

inline void comma_space() {
  Serial.print(comma_space_string);
}

inline void yes() {
  Serial.print(yes_string);
}

inline void no() {
  Serial.print(no_string);
}

inline void y() {
  Serial.print(y_string);
}
inline void n() {
  Serial.print(n_string);
}

inline void print_command_name(String command_data) {
  Serial.print(command_data);
  space_colon();
};

inline void print_new_command_name(String command_data) {
  Serial.println(F("New Value"));
  print_command_name(command_data);
};


inline void Pointer() {
  Serial.print(pointer_string);
}

void Host::init_serial() {
  Serial.begin(HOST_SERIAL_SPEED);
}

void Host::check_serial() {   //to read incomming data

  if (Serial.available() > 0) {

    String rx = Serial.readString();
    rx.trim();  //trim off return carraige

    byte temp = data_to_report; // log what we were doing in case we only want

    //set message printing mode
    data_to_report = data_set_LUT(rx);

    //input might be to directly change value
    if (data_to_report == 255) {
      data_to_report = temp;
      serial_sub_menu(rx);
    }

  }
}

void Host::serial_sub_menu(String rx) {

  char delimiter_char = ' ';

  //split input into strings based on these delimiters
  byte first_delimiter = rx.indexOf(delimiter_char);
  byte second_delimiter = rx.indexOf(delimiter_char, first_delimiter + 1);
  byte third_delimiter = rx.indexOf(delimiter_char, second_delimiter + 1);

  String data_set = rx.substring(0, first_delimiter);
  String command_mode = rx.substring(first_delimiter + 1, second_delimiter);
  String command_data = rx.substring(second_delimiter + 1, third_delimiter);
  String command_arg =  rx.substring(third_delimiter + 1); //value to write as a string

  int value = command_arg.toInt();  //value as an int

  if (command_mode == "-h") {
    data_to_report = data_set_LUT(data_set);  //convert string to data to display
    print_help_options();
    data_to_report = STOP_REPORT;   // stop printing to keep options on screen
  }

  else if (command_mode == "-r") {
    byte temp = data_to_report;
    data_to_report = data_set_LUT(data_set);
    if (command_contains_num(data_to_report)) {
      String command_num_str = data_set.substring(data_set.length() - 1, data_set.length());//get the num from this string
      byte command_num = command_num_str.toInt();
      return_data(command_data, command_num);
    }
    else
      return_data(command_data);
    data_to_report = temp;    //return to whatever we were doing before
  }

  else if (command_mode == "-w") {
    byte temp_data_to_report = data_to_report;

    data_to_report = data_set_LUT(data_set);
    write_data(command_data, value);
    data_to_report = temp_data_to_report;    //return to whatever we were doing before
  }
}


byte Host::data_set_LUT(String data_set) {

  if (data_set == serial_sub_menu_items.data_elements[0][STOP_REPORT])
    return STOP_REPORT;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_FANS])
    return REPORT_FANS;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_TEMPS])
    return REPORT_TEMPS;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_LED_STRIP])
    return REPORT_LED_STRIP;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_MENU_TREE])
    return REPORT_MENU_TREE;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_LDRS])
    return REPORT_LDRS;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_ENCODER])
    return REPORT_ENCODER;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_BUTTON])
    return REPORT_BUTTON;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_CURRENT_METER])
    return REPORT_CURRENT_METER;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_SD_CARD])
    return REPORT_SD_CARD;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_LDR_CONFIG])
    return REPORT_LDR_CONFIG;



  //check substring for text, thest contain object nums
  String sub_string_data_set = data_set.substring(0, data_set.length() - 1);

  if (sub_string_data_set == serial_sub_menu_items.data_elements[0][REPORT_TEXT]) {
    last_command = data_set;
    return REPORT_TEXT;
  }

  else if (sub_string_data_set == serial_sub_menu_items.data_elements[0][REPORT_TEXT_FRAME_HISTORY]) {
    last_command = data_set;
    return REPORT_TEXT_FRAME_HISTORY;
  }

  else if (sub_string_data_set == serial_sub_menu_items.data_elements[0][REPORT_MENU_FRAME_HISTORY]) {
    last_command = data_set;
    return REPORT_MENU_FRAME_HISTORY;
  }

  else if (sub_string_data_set == serial_sub_menu_items.data_elements[0][REPORT_POS_FRAME_HISTORY]) {
    last_command = data_set;
    return REPORT_POS_FRAME_HISTORY;
  }

  else if (sub_string_data_set == serial_sub_menu_items.data_elements[0][REPORT_SENSOR_FRAME_HISTORY]) {
    last_command = data_set;
    return REPORT_SENSOR_FRAME_HISTORY;
  }
  else
    return 255;
}

void Host::print_help_options() {

  Serial.println();
  Serial.println(F("Command \t\t   Description"));

  //loop through commands
  for (int i = 0; i < serial_sub_menu_items.active_elements_by_row[data_to_report]; i++) {
    Serial.print(serial_sub_menu_items.data_elements[data_to_report][i]);

    //tab out
    tab();
    if (serial_sub_menu_items.data_elements[data_to_report][i].length() < 16)
      tab();
    if (serial_sub_menu_items.data_elements[data_to_report][i].length() < 8)
      tab();

    space_colon();

    Serial.println(serial_sub_menu_items.data_descriptions[data_to_report][i]);
  }

  Serial.println();
  if (data_to_report != STOP_REPORT)
    Serial.println(serial_sub_menu_items.prepend_commands);
  else
    Serial.println(serial_sub_menu_items.top_level_commands);
  Serial.println();
  data_to_report = STOP_REPORT;
}

void Host::write_data(String command_data, int value) {

  for (int i = 0; i < serial_sub_menu_items.active_elements_by_row[data_to_report]; i++) {
    if (command_data == serial_sub_menu_items.data_elements[data_to_report][i]) {
      Serial.println(F("Original Value"));
      print_command_name(command_data);
      read_write_LUT(i, 'r');

      read_write_LUT(i, 'w', value);

      print_new_command_name(command_data);
      read_write_LUT(i, 'r');
      break;
    }
  }
}

void Host::return_data(String command_data, byte num) {

  for (int i = 0; i < serial_sub_menu_items.active_elements_by_row[data_to_report]; i++) {  //scan through list of valid commands
    if (command_data == serial_sub_menu_items.data_elements[data_to_report][i]) {
      if (num == -1) {
        print_command_name(command_data);
        read_write_LUT(i, 'r');
      }
      else {
        print_command_name(command_data);
        read_write_LUT(i, 'r', num);  // only case of and obj num or frame num being provided
      }
      break;
    }
  }
}

void Host::read_write_LUT(byte index, char r_w, int value) {

  switch (data_to_report) { //row
    case REPORT_FANS:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(fan_parameters.pin)                          : Serial.println(pin_error_msg);                   break;
        case 1: (r_w == 'r')  ?  Serial.println(fan_parameters.manual_set_value)             : fan_parameters.manual_set_value = value;         break;
        case 2: (r_w == 'r')  ?  Serial.println(fan_parameters.target_speed)                 : fan_parameters.target_speed = value;             break;
        case 3: (r_w == 'r')  ?  Serial.println(fan_parameters.current_speed)                : fan_parameters.current_speed = value;            break;
        case 4: (r_w == 'r')  ?  Serial.println(fan_parameters.change_increment)             : fan_parameters.change_increment = value;         break;
        case 5: (r_w == 'r')  ?  Serial.println(fan_parameters.fan_change_interval)          : fan_parameters.fan_change_interval = value;      break;
        case 6: (r_w == 'r')  ?  Serial.println(fan_parameters.fan_minimum)                  : fan_parameters.fan_minimum = value;              break;
        case 7: (r_w == 'r')  ?  Serial.println(fan_parameters.enabled)                      : fan_parameters.enabled = value;                  break;
        case 8: (r_w == 'r')  ?  Serial.println(fan_parameters.manual)                       : fan_parameters.manual = value;                   break;
      }
      break;

    case REPORT_TEMPS:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(temp_parameters.pin1)                        : Serial.println(pin_error_msg);                   break;
        case 1: (r_w == 'r')  ?  Serial.println(temp_parameters.pin2)                        : Serial.println(pin_error_msg);                   break;
        case 2: (r_w == 'r')  ?  Serial.println(temp_parameters.pin3)                        : Serial.println(pin_error_msg);                   break;
        case 3: (r_w == 'r')  ?  Serial.println(temp_parameters.enabled1)                    : temp_parameters.enabled1 = value;                break;
        case 4: (r_w == 'r')  ?  Serial.println(temp_parameters.enabled2)                    : temp_parameters.enabled2 = value;                break;
        case 5: (r_w == 'r')  ?  Serial.println(temp_parameters.enabled3)                    : temp_parameters.enabled3 = value;                break;
      }
      break;

    case REPORT_LED_STRIP:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(led_strip_parameters.pin)                       : Serial.println(pin_error_msg);                              break;
        case 1: (r_w == 'r')  ?  Serial.println(led_strip_parameters.target_brightness)         : led_strip_parameters.target_brightness = value;             break;
        case 2: (r_w == 'r')  ?  Serial.println(led_strip_parameters.current_brightness)        : led_strip_parameters.current_brightness = value;            break;
        case 3: (r_w == 'r')  ?  Serial.println(led_strip_parameters.change_increment)          : led_strip_parameters.change_increment = value;              break;
        case 4: (r_w == 'r')  ?  Serial.println(led_strip_parameters.change_interval)           : led_strip_parameters.change_interval = value;               break;
        case 5: (r_w == 'r')  ?  Serial.println(led_strip_parameters.led_stable_interval)       : led_strip_parameters.led_stable_interval = value;           break;
        case 6: (r_w == 'r')  ?  Serial.println(led_strip_parameters.minimum_on)                : led_strip_parameters.minimum_on = value;                    break;
        case 7: (r_w == 'r')  ?  Serial.println(led_strip_parameters.enabled)                   : led_strip_parameters.enabled = value;                       break;
        case 8: (r_w == 'r')  ?  Serial.println(led_strip_parameters.fast_interval)             : led_strip_parameters.fast_interval = value;                 break;
        case 9: (r_w == 'r')  ?  Serial.println(led_strip_parameters.sinusoidal)                : led_strip_parameters.sinusoidal = value;                    break;
        case 10: (r_w == 'r')  ?  Serial.println(led_strip_parameters.sinusoidal_half_frequency) : led_strip_parameters.sinusoidal_half_frequency = value;     break;
      }
      break;

    case REPORT_ENCODER:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(encoder_parameters.pinA)                        : Serial.println(pin_error_msg);                              break;
        case 1: (r_w == 'r')  ?  Serial.println(encoder_parameters.pinB)                        : Serial.println(pin_error_msg);                              break;
        case 2: (r_w == 'r')  ?  Serial.println(encoder_parameters.PosCount / 2)                : encoder_parameters.PosCount = value * 2;
          (r_w == 'r')  ?                                                                 : encoder_parameters.position = value;                        break;  //change both
        case 3: (r_w == 'r')  ?  Serial.println(encoder_parameters.enabled)                     : encoder_parameters.enabled = value;                         break;
        case 4: (r_w == 'r')  ?  Serial.println(encoder_parameters.is_attached)                 : encoder_parameters.is_attached = value;                     break;
      }
      break;

    case REPORT_BUTTON:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(button_parameters.button_pin)                   : Serial.println(pin_error_msg);                              break;
        case 1: (r_w == 'r')  ?  Serial.println(button_parameters.button_pressed_ISR)           : button_parameters.button_pressed_ISR = value;               break;
        case 2: (r_w == 'r')  ?  Serial.println(button_parameters.button_press_interval)        : button_parameters.button_press_interval = value;            break;
        case 3: (r_w == 'r')  ?  Serial.println(button_parameters.enabled)                      : button_parameters.enabled = value;                          break;
        case 4: (r_w == 'r')  ?  Serial.println(button_parameters.is_attached)                  : button_parameters.is_attached = value;                      break;
      }
      break;

    case REPORT_LDR_CONFIG:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(light_sensor_parameters.config_max1)            : light_sensor_parameters.config_max1 = value;               break;
        case 1: (r_w == 'r')  ?  Serial.println(light_sensor_parameters.config_min1)            : light_sensor_parameters.config_min1 = value;               break;
        case 2: (r_w == 'r')  ?  Serial.println(light_sensor_parameters.config_max2)            : light_sensor_parameters.config_max2 = value;               break;
        case 3: (r_w == 'r')  ?  Serial.println(light_sensor_parameters.config_min2)            : light_sensor_parameters.config_min2 = value;               break;
      }
      break;

    case REPORT_LDRS:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(light_sensor_parameters.pin1)                   : Serial.println(pin_error_msg);                              break;
        case 1: (r_w == 'r')  ?  Serial.println(light_sensor_parameters.pin2)                   : Serial.println(pin_error_msg);                              break;
        case 2: (r_w == 'r')  ?  Serial.println(light_sensor_parameters.enabled1)               : light_sensor_parameters.enabled1 = value;                   break;
        case 3: (r_w == 'r')  ?  Serial.println(light_sensor_parameters.enabled2)               : light_sensor_parameters.enabled2 = value;                   break;
        case 4: (r_w == 'r')  ?  Serial.println(light_sensor_parameters.large_disparity)        : light_sensor_parameters.large_disparity = value;            break;
      }
      break;

    case REPORT_CURRENT_METER:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(current_meter_parameters.pin1)                   : Serial.println(pin_error_msg);                             break;
        case 1: (r_w == 'r')  ?  Serial.println(current_meter_parameters.pin2)                   : Serial.println(pin_error_msg);                             break;
        case 2: (r_w == 'r')  ?  Serial.println(current_meter_parameters.enabled1)               : current_meter_parameters.enabled1 = value;                 break;
        case 3: (r_w == 'r')  ?  Serial.println(current_meter_parameters.enabled2)               : current_meter_parameters.enabled2 = value;                 break;
        case 4: (r_w == 'r')  ?  Serial.println(current_meter_parameters.max_current_limit)      : current_meter_parameters.max_current_limit = value;        break;
      }
      break;

    case REPORT_SD_CARD:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(card1.pin)                                       : Serial.println(pin_error_msg);                             break;
        case 1: (r_w == 'r')  ?  Serial.println(card2.pin)                                       : Serial.println(pin_error_msg);                             break;
        case 2: (r_w == 'r')  ?  Serial.println(card1.enabled)                                   : card1.enabled = value;                                     break;
        case 3: (r_w == 'r')  ?  Serial.println(card2.enabled)                                   : card2.enabled = value;                                     break;
        case 5: (r_w == 'r')  ?  Serial.println(SD_string.Network)                               : Serial.println(string_write_error_msg);                    break;
        case 6: (r_w == 'r')  ?  Serial.println(SD_string.Password)                              : Serial.println(string_write_error_msg);                    break;
      }
      break;

    case REPORT_TEXT_FRAME_HISTORY:
      switch (index) {
        case 0: (r_w == 'r')  ?  print_frame_hist(TEXT_FRAME_TYPE, value)                        : Serial.println(frame_hist_error_msg);                      break;
        case 1: (r_w == 'r')  ?  Serial.println(text_frame_history[value].history_index)         : Serial.println(frame_hist_error_msg);                      break;
        case 2: (r_w == 'r')  ?  Serial.println(text_frame_history[value].num_populated_buffers) : Serial.println(frame_hist_error_msg);                      break;
      }
      break;

    case REPORT_POS_FRAME_HISTORY:
      switch (index) {
        case 0: (r_w == 'r')  ?  print_frame_hist(POS_FRAME_TYPE, value)                         : Serial.println(frame_hist_error_msg);                      break;
        case 1: (r_w == 'r')  ?  Serial.println(pos_frame_history[value].history_index)          : Serial.println(frame_hist_error_msg);                      break;
        case 2: (r_w == 'r')  ?  Serial.println(pos_frame_history[value].num_populated_buffers)  : Serial.println(frame_hist_error_msg);                      break;
      }
      break;

    case REPORT_MENU_FRAME_HISTORY:
      switch (index) {
        case 0: (r_w == 'r')  ?  print_frame_hist(MENU_FRAME_TYPE, value)                        : Serial.println(frame_hist_error_msg);                      break;
        case 1: (r_w == 'r')  ?  Serial.println(menu_frame_history[value].history_index)         : Serial.println(frame_hist_error_msg);                      break;
        case 2: (r_w == 'r')  ?  Serial.println(menu_frame_history[value].num_populated_buffers) : Serial.println(frame_hist_error_msg);                      break;
      }
      break;

    case REPORT_SENSOR_FRAME_HISTORY:
      switch (index) {
        case 0: (r_w == 'r')  ?  print_frame_hist(SENSOR_FRAME_TYPE, value)                              : Serial.println(frame_hist_error_msg);                      break;
        case 1: (r_w == 'r')  ?  Serial.println(sensor_data_frame_history[value].history_index)          : Serial.println(frame_hist_error_msg);                      break;
        case 2: (r_w == 'r')  ?  Serial.println(sensor_data_frame_history[value].num_populated_buffers)  : Serial.println(frame_hist_error_msg);                      break;
      }
      break;

  }
}

void Host::print_messages() {

  static uint32_t last_message_print_time = millis();
  static byte previously_reporting = 0; // reset header counter if just changed printing messages

  if (millis() > last_message_print_time + MESSAGE_DELAY_PERIOD) { // check if specified time delay elapsed

    last_message_print_time = millis();

    // increment counter for printing header info message
    if (previously_reporting != data_to_report)
      header_print_counter = 0;
    else
      header_print_counter += HEADER_PRINT_INCREMENT;

    previously_reporting = data_to_report;

    switch (data_to_report) {
      case STOP_REPORT:
        break;
      case REPORT_FANS:
        print_fans();
        break;
      case REPORT_TEMPS:
        print_temps();
        break;
      case REPORT_LED_STRIP:
        print_led_strip();
        break;
      case REPORT_MENU_TREE:
        print_menu_tree();
        break;
      case REPORT_LDRS:
        print_ldrs();
        break;
      case REPORT_ENCODER:
        print_encoder();
        break;
      case REPORT_BUTTON:
        print_button();
        break;

      case REPORT_CURRENT_METER:
        print_current_meters();
        break;

      case REPORT_SD_CARD:
        print_sd_cards();
        break;

      case REPORT_TEXT:
        print_text(last_command);
        break;

      case REPORT_LDR_CONFIG:
        print_ldr_config();
        break;

      default: break;
    }

  }
}




//_________Printing Functions__________

void Host::print_fans() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pin \tEnabled \tISR attached  \tCurrent Speed \tTarget Speed \tAverage Temp"));
  }

  Serial.print(fan_parameters.pin);
  tab();

  if (fan_parameters.enabled)
    yes();

  else
    no();

  tab(); tab();

  if (timers.fan_timer_attached)
    yes();
  else
    no();

  tab(); tab();

  Serial.print(fan_parameters.current_speed);
  tab(); tab();
  Serial.print(fan_parameters.target_speed);
  tab(); tab();
  Serial.print(temp_parameters.avg);
  Serial.println();

}

void Host::print_temps() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pins \t     Sensors Enabled \t Good Connection \t Current Temp Readings \t Average Temp"));
  }

  Serial.print(temp_parameters.pin1);
  space();

  Serial.print(temp_parameters.pin2);
  space();

  Serial.print(temp_parameters.pin3);

  space(); space(); space(); space(); space();

  if (temp_parameters.enabled1)
    y();
  else
    n();
  space();
  if (temp_parameters.enabled2)
    y();
  else
    n();
  space();
  if (temp_parameters.enabled3)
    y();
  else
    n();

  tab(); tab(); space();

  if (!temp_parameters.enabled1)
    dash_space();
  else {
    if (temp_parameters.bad_connection1)
      n();
    else
      y();
    space();
  }

  if (!temp_parameters.enabled2)
    dash_space();
  else {
    if (temp_parameters.bad_connection2)
      n();
    else
      y();
    space();
  }

  if (!temp_parameters.enabled3) {
    dash_space(); tab();
  }
  else {
    if (temp_parameters.bad_connection3)
      n();
    else
      y();
    tab();
  }
  tab(); tab(); space();

  Serial.print(temp_parameters.temp1);
  space();

  Serial.print(temp_parameters.temp2);
  space();

  Serial.print(temp_parameters.temp3);
  tab(); tab(); space();

  Serial.print(temp_parameters.avg);
  Serial.println();


}

void Host::print_led_strip() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pin \tLED Strip Enabled \tCurrent Brightness \tTarget Brightness \tUse Fast ISR Interval"));
  }

  Serial.print(led_strip_parameters.pin);
  tab();

  if (led_strip_parameters.enabled)
    yes();
  else
    no();
  tab(); tab(); tab();
  Serial.print(led_strip_parameters.current_brightness);
  tab(); tab(); tab();

  Serial.print(led_strip_parameters.target_brightness);
  tab(); tab(); tab();
  if (led_strip_parameters.fast_interval)
    Serial.println(F("Yes"));
  else
    Serial.println(F("No"));

}

void Host::print_encoder() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pins \tEncoder Enabled \tISR Attached \tPosition"));
  }

  Serial.print(encoder_parameters.pinA);
  space();

  Serial.print(encoder_parameters.pinB);
  tab();

  if (encoder_parameters.enabled)
    yes();
  else
    no();

  tab(); tab; tab(); tab();

  if (encoder_parameters.is_attached)
    yes();
  else
    no();

  tab(); tab; tab();

  Serial.println(encoder_parameters.position);


}

void Host::print_button() {
  static int function_called_last = button_parameters.last_button_pressed;

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pin \tButton Enabled \tISR Attached \tPressed"));
  }

  Serial.print(button_parameters.button_pin);
  tab();

  if (button_parameters.enabled)
    yes();
  else
    Serial.print(F("No "));

  tab; tab(); tab();

  if (button_parameters.is_attached)
    yes();
  else
    Serial.print(F("No "));

  tab(); tab; tab();

  if (button_parameters.last_button_pressed > function_called_last)
    yes();
  else
    no();
  Serial.println();

  function_called_last = millis();
}

void Host::print_ldrs() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pins \tLDR's Enabled \tGood Connection \tReading \tAvg Reading"));
  }

  Serial.print(light_sensor_parameters.pin1);
  space();
  Serial.print(light_sensor_parameters.pin2);
  tab();

  if (light_sensor_parameters.enabled1)
    y();
  else
    n();

  space();

  if (light_sensor_parameters.enabled2)
    y();
  else
    n();

  tab; tab(); tab();

  if (!light_sensor_parameters.enabled2)
    dash_space();
  else {
    if (light_sensor_parameters.bad_connection1)
      n();
    else
      y();
    space();
  }

  if (!light_sensor_parameters.enabled2)
    dash_space();
  else {
    if (light_sensor_parameters.bad_connection2)
      n();
    else
      y();
    space();
  }

  tab(); tab; tab(); tab();

  Serial.print(light_sensor_parameters.reading1);
  space();
  Serial.print(light_sensor_parameters.reading2);
  tab(); tab();
  Serial.print(light_sensor_parameters.avg_reading);
  Serial.println();

}

void Host::print_current_meters() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pins \tMeters Enabled \tMax Current \tAmp Drawn \tAvg Reading"));
  }

  Serial.print(current_meter_parameters.pin1);
  space();
  Serial.print(current_meter_parameters.pin2);
  tab();

  if (current_meter_parameters.enabled1)
    y();
  else
    n();

  space();

  if (current_meter_parameters.enabled2)
    y();
  else
    n();

  tab; tab(); tab();

  Serial.print(current_meter_parameters.max_current_limit);

  tab(); tab; tab();

  Serial.print(current_meter_parameters.reading1);
  space();
  Serial.print(current_meter_parameters.reading2);
  tab(); tab();
  Serial.print(current_meter_parameters.total);
  Serial.println();

}


void Host::print_sd_cards() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pins \tEnabled \tDetected \tFiles Found"));
  }
  Serial.print(card1.pin);
  space();
  Serial.print(card2.pin);
  tab();

  if (card1.enabled)
    y();
  else
    n();

  space();

  if (card2.enabled)
    y();
  else
    n();

  tab; tab(); tab; tab();

  if (!card1.enabled)
    dash_space();
  else {
    if (card1.detected)
      y();
    else
      n();
    space();
  }

  if (!card2.enabled)
    dash_space();
  else {
    if (card2.detected)
      y();
    else
      n();
  }

  tab; tab(); tab; tab();

  Serial.print(card1.working_dir);
  space_colon();
  if (card1.network_file_exists) {
    Serial.print(EXT_NETWORK_FILE);
    comma_space();
  }
  if (card1.disp_string_file_exists) {
    Serial.print(EXT_STRING_FILE);
    comma_space();
  }
  if (card1.log_file_exists) {
    Serial.print(EXT_LOG_FILE);
    comma_space();
  }
  if (card1.instruction_file_exists) {
    Serial.print(EXT_CALIBRATION_FILE);
    comma_space();
  }
  if (card1.calibration_file_exists) {
    Serial.print(EXT_INSTRUCTION_FILE);
    comma_space();
  }
  if (card1.bitmap_file_exists) {
    Serial.print(EXT_BITMAP_FILE);
    comma_space();
  }
  tab();
  Serial.print(card2.working_dir);
  space_colon();
  if (card2.network_file_exists) {
    Serial.print(INT_NETWORK_FILE);
    comma_space();
  }
  if (card2.disp_string_file_exists) {
    Serial.print(INT_STRING_FILE);
    comma_space();
  }
  if (card2.log_file_exists) {
    Serial.print(INT_LOG_FILE);
    comma_space();
  }
  if (card2.instruction_file_exists) {
    Serial.print(INT_CALIBRATION_FILE);
    comma_space();
  }
  if (card2.calibration_file_exists) {
    Serial.print(INT_INSTRUCTION_FILE);
    comma_space();
  }
  if (card2.bitmap_file_exists) {
    Serial.print(INT_BITMAP_FILE);
    comma_space();
  }
  Serial.println();
}


void Host::print_text(String command) {

  String obj_num_as_char = command.substring(command.length() - 1);
  byte obj_num = obj_num_as_char.toInt();
  if (obj_num >= MAX_NUM_OF_TEXT_OBJECTS)
    obj_num = 0;

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Configured \tSize \tR   G   B   H    \tUse Hue \tXpos Ypos \t\tScroll Speeds   \tStart points   \t\tEnd Points \t\tLimits X- X+ Y- Y+"));
  }
  if (text_cursor[obj_num].object_used)
    yes();
  else {
    no(); space();
  }
  tab(); tab();

  Serial.print(text_parameters[obj_num].text_size);
  tab();

  Serial.print(text_parameters[obj_num].red);
  if (text_parameters[obj_num].red < 10) space();
  if (text_parameters[obj_num].red < 100) space();
  space();

  Serial.print(text_parameters[obj_num].green);
  if (text_parameters[obj_num].green < 10) space();
  if (text_parameters[obj_num].green < 100) space();
  space();

  Serial.print(text_parameters[obj_num].blue);
  if (text_parameters[obj_num].blue < 10) space();
  if (text_parameters[obj_num].blue < 100) space();
  space();

  Serial.print(text_parameters[obj_num].hue);
  if (abs(text_parameters[obj_num].hue < 10)) space();
  if (abs(text_parameters[obj_num].hue < 100)) space();
  if (abs(text_parameters[obj_num].hue < 1000)) space();
  if (text_parameters[obj_num].hue >= 0) space();
  tab();

  if (text_parameters[obj_num].use_hue)
    yes();
  else {
    no(); space();
  }
  tab(); tab();

  Serial.print(text_cursor[obj_num].x);
  if (abs(text_cursor[obj_num].x < 10)) space();
  if (abs(text_cursor[obj_num].x < 100)) space();
  if (text_cursor[obj_num].x >= 0) space();
  space();

  Serial.print(text_cursor[obj_num].y);
  if (abs(text_cursor[obj_num].y < 10)) space();
  if (abs(text_cursor[obj_num].y < 100)) space();
  if (text_cursor[obj_num].y >= 0) space();
  tab(); tab();

  Serial.print(text_cursor[obj_num].x_pos_dir - 128);
  if (abs(text_cursor[obj_num].x_pos_dir - 128 < 10)) space();
  if (abs(text_cursor[obj_num].x_pos_dir - 128 < 100)) space();
  if (text_cursor[obj_num].x_pos_dir - 128 >= 0) space();
  space();

  Serial.print(text_cursor[obj_num].y_pos_dir - 128);
  if (abs(text_cursor[obj_num].y_pos_dir - 128 < 10)) space();
  if (abs(text_cursor[obj_num].y_pos_dir - 128 < 100)) space();
  if (text_cursor[obj_num].y_pos_dir - 128 >= 0) space();
  space();
  tab(); tab();

  Serial.print(text_cursor[obj_num].x_start);
  if (abs(text_cursor[obj_num].x_start < 10)) space();
  if (abs(text_cursor[obj_num].x_start < 100)) space();
  if (text_cursor[obj_num].x_start >= 0) space();
  space();

  Serial.print(text_cursor[obj_num].y_start);
  if (abs(text_cursor[obj_num].y_start < 10)) space();
  if (abs(text_cursor[obj_num].y_start < 100)) space();
  if (text_cursor[obj_num].y_start >= 0) space();
  space();

  tab(); tab();
  Serial.print(text_cursor[obj_num].x_end);
  if (abs(text_cursor[obj_num].x_end < 10)) space();
  if (abs(text_cursor[obj_num].x_end < 100)) space();
  if (text_cursor[obj_num].x_end >= 0) space();
  space();

  Serial.print(text_cursor[obj_num].y_end);
  if (abs(text_cursor[obj_num].y_end < 10)) space();
  if (abs(text_cursor[obj_num].y_end < 100)) space();
  if (text_cursor[obj_num].y_end >= 0) space();
  space();

  tab(); tab();

  Serial.print(text_cursor[obj_num].x_limit_min);
  space();
  Serial.print(text_cursor[obj_num].x_limit_max);
  space();
  Serial.print(text_cursor[obj_num].y_limit_min);
  space();
  Serial.print(text_cursor[obj_num].y_limit_max);


  Serial.println();

}


void Host::print_menu_tree() {

  //  static int previous_menu_displayed = menu.get_current_menu();
  //  if (previous_menu_displayed != menu.get_current_menu()) //if we changed menu for menu auto changed by other process, push all menu options to host
  //    print_menu_tree_options();
  //
  //  previous_menu_displayed = menu.get_current_menu();

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pos \t Current Menu  \t Value Selected"));
  }
  Serial.print(encoder_parameters.position);
  tab();
  print_menu_tree_options(menu.get_current_menu());
  tab();

  //convert encoder position to menu item
  position_to_menu_value();

  Serial.println();
}


void Host::print_menu_tree_options(int cur_menu) {

  if (cur_menu == -1) {
    switch (menu.get_current_menu()) {
      case DEFAULT_MENU:
        Serial.print(F("default menu, press encoder to enter menu tree"));
        break;

      case MAIN_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.screen_mode);
        Serial.println(menu_items.brightness);
        Serial.println(menu_items.text_settings);
        Serial.println(menu_items.fan_settings);
        Serial.println(menu_items.internet_settings);
        Serial.println(menu_items.sd_card_settings);
        Serial.println(menu_items.led_strip_settings);
        break;

      case SCREEN_MODE_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.screen_mode0);
        Serial.println(menu_items.screen_mode1);
        Serial.println(menu_items.screen_mode3);
        Serial.println(menu_items.screen_mode2);
        break;

      case TEXT_SETTINGS_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.text_size_settings);
        Serial.println(menu_items.text_colour_settings);
        Serial.println(menu_items.scroll_speed_settings);
        Serial.println(menu_items.flip_dir_settings);
        break;

      case FAN_SETTINGS_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.fan_speed_settings);
        Serial.println(menu_items.fan_enable);
        Serial.println(menu_items.fan_disable);
        Serial.println(menu_items.minimum_rotating_speed);
        break;

      case INTERNET_CONFIG_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.select_network_manually);
        Serial.println(menu_items.current_network);
        Serial.println(menu_items.wifi_enable);
        Serial.println(menu_items.wifi_disable);
        Serial.println(menu_items.git_settings);
        break;

      case SD_CARD_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.enable_ext_card);
        Serial.println(menu_items.disable_ext_card);
        Serial.println(menu_items.sd_card_folders);
        break;

      case LED_STRIP_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.enable_led_strip);
        Serial.println(menu_items.disable_led_strip);
        Serial.println(menu_items.led_strip_brightness);
        break;

      case TEXT_COLOUR_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.text_colour_red);
        Serial.println(menu_items.text_colour_green);
        Serial.println(menu_items.text_colour_blue);
        Serial.println(menu_items.text_colour_hue);
        Serial.println(menu_items.text_colour_use_hue);
        Serial.println(menu_items.text_colour_use_rgb);
        break;

      case INTERNET_CONNECTION_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.current_connected_network);
        Serial.println(menu_items.current_ip_address);
        break;

      case GIT_SETTING_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.show_git_auther);
        Serial.println(menu_items.show_git_commit_msg);
        Serial.println(menu_items.show_git_commit_tag);
        Serial.println(menu_items.get_firmware_update);
        Serial.println(menu_items.git_settings);
        break;
 
      //if any of these just print same string, these have no sub menu list
      case BRIGHTNESS_MENU:
      case TEXT_SIZE_MENU:
      case SCROLL_SPEED_MENU:
      case FAN_SPEED_MENU:
      case MIN_FAN_SPEED_MENU:
      //      case SD_FOLDERS_MENU:
      case LED_STRIP_BRIGHTNESS_MENU:
      case TEXT_COLOUR_RED:
      case TEXT_COLOUR_GREEN:
      case TEXT_COLOUR_BLUE:
      case TEXT_COLOUR_HUE:

        Serial.println(F("Variable adjustment menu, click button to return"));
        break;
    }

  }
  else
  {
    switch (cur_menu) {
      case STARTUP:                     Serial.print(F("System Starting, Please wait..."));     break;
      case DEFAULT_MENU:                Serial.print(F("Menu Minimised, Press button"));        break;
      case MAIN_MENU:                   Serial.print(menu_items.main_menu);                     break;
      case SCREEN_MODE_MENU:            Serial.print(menu_items.screen_mode_menu);              break;
      case TEXT_SETTINGS_MENU:          Serial.print(menu_items.text_settings_menu);            break;
      case FAN_SETTINGS_MENU:           Serial.print(menu_items.fan_settings_menu);             break;
      case INTERNET_CONFIG_MENU:        Serial.print(menu_items.internet_config);               break;
      case SD_CARD_MENU:                Serial.print(menu_items.sd_cards_menu);                 break;
      case LED_STRIP_MENU:              Serial.print(menu_items.led_strip_menu);                break;
      case TEXT_COLOUR_MENU:            Serial.print(menu_items.text_colour_menu);              break;
      case BRIGHTNESS_MENU:             Serial.print(menu_items.brightness_menu);               break;
      case TEXT_SIZE_MENU:              Serial.print(menu_items.text_size_menu);                break;
      case SCROLL_SPEED_MENU:           Serial.print(menu_items.scroll_speed_menu);             break;
      case FAN_SPEED_MENU:              Serial.print(menu_items.fan_speed_menu);                break;
      case MIN_FAN_SPEED_MENU:          Serial.print(menu_items.minimum_fan_speed_menu);        break;
      //      case SD_FOLDERS_MENU:             Serial.print(menu_items.SD_card_folders_menu);          break;
      case LED_STRIP_BRIGHTNESS_MENU:   Serial.print(menu_items.led_strip_brightness_menu);     break;
      case TEXT_COLOUR_RED:             Serial.print(menu_items.text_colour_red_menu);          break;
      case TEXT_COLOUR_GREEN:           Serial.print(menu_items.text_colour_green_menu);        break;
      case TEXT_COLOUR_BLUE:            Serial.print(menu_items.text_colour_blue_menu);         break;
      case TEXT_COLOUR_HUE:             Serial.print(menu_items.text_colour_hue_menu);          break;
      case TEXT_OBJ_SELECTION_MENU:     Serial.print(menu_items.text_obj_select_menu);          break;
    }
  }
}


void Host::position_to_menu_value() {
  //given our current menu, display what item the encoder positioned on
  switch (menu.get_current_menu()) {
    case MAIN_MENU:
      switch (encoder_parameters.position) {
        case 0: Serial.print(menu_items.RETURN);                   break;
        case 1: Serial.print(menu_items.screen_mode);              break;
        case 2: Serial.print(menu_items.brightness);               break;
        case 3: Serial.print(menu_items.text_settings);            break;
        case 4: Serial.print(menu_items.fan_settings);             break;
        case 5: Serial.print(menu_items.internet_settings);        break;
        case 6: Serial.print(menu_items.sd_card_settings);         break;
        case 7: Serial.print(menu_items.led_strip_settings);       break;
      }
      break;

    case SCREEN_MODE_MENU:
      switch (encoder_parameters.position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.screen_mode0);              break;
        case 2: Serial.print(menu_items.screen_mode1);              break;
        case 3: Serial.print(menu_items.screen_mode3);              break;
        case 4: Serial.print(menu_items.screen_mode2);              break;
      }
      break;

    case TEXT_SETTINGS_MENU:
      switch (encoder_parameters.position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.text_size_settings);        break;
        case 2: Serial.print(menu_items.text_colour_settings);      break;
        case 3: Serial.print(menu_items.scroll_speed_settings);     break;
        case 4: Serial.print(menu_items.flip_dir_settings);         break;
      }
      break;

    case FAN_SETTINGS_MENU:
      switch (encoder_parameters.position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.fan_speed_settings);        break;
        case 2:
          if (!fan_parameters.enabled)
            Serial.print(menu_items.fan_enable);
          else
            Serial.print(menu_items.fan_disable);
          break;
        case 3: Serial.print(menu_items.minimum_rotating_speed);    break;
      }
      break;

    case INTERNET_CONFIG_MENU:
      switch (encoder_parameters.position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.select_network_manually);   break;
        case 2: Serial.print(menu_items.current_network);           break;
        case 3: Serial.print(menu_items.wifi_enable);               break;
        case 4: Serial.print(menu_items.wifi_disable);              break;
        case 5: Serial.print(menu_items.git_settings);              break;
      }
      break;

    case SD_CARD_MENU:
      switch (encoder_parameters.position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1:
          if (!card1.enabled)
            Serial.print(menu_items.enable_ext_card);
          else
            Serial.print(menu_items.disable_ext_card);
          break;
        case 2:
          if (!card2.enabled)
            Serial.print(menu_items.enable_int_card);
          else
            Serial.print(menu_items.disable_int_card);
          break;
        case 3: Serial.print(menu_items.sd_card_folders);           break;
      }
      break;

    case LED_STRIP_MENU:
      switch (encoder_parameters.position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1:
          if (!led_strip_parameters.enabled)
            Serial.print(menu_items.enable_led_strip);
          else
            Serial.print(menu_items.disable_led_strip);
          break;
        case 2: Serial.print(menu_items.led_strip_brightness);      break;
      }
      break;

    case TEXT_COLOUR_MENU:
      switch (encoder_parameters.position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.text_colour_red);           break;
        case 2: Serial.print(menu_items.text_colour_green);         break;
        case 3: Serial.print(menu_items.text_colour_blue);          break;
        case 4: Serial.print(menu_items.text_colour_hue);           break;
        case 5:
          if (text_parameters[menu.get_selected_object()].use_hue)
            Serial.print(menu_items.text_colour_use_rgb);
          else
            Serial.print(menu_items.text_colour_use_hue);
          break;
      }
      break;

    case BRIGHTNESS_MENU:
      Serial.print(screen_brightness);
      Serial.print(percent_string);
      break;

    case TEXT_SIZE_MENU:
      Serial.print(text_parameters[menu.get_selected_object()].text_size);
      Serial.print(divide_string);
      Serial.print(menu_limits.text_size_menu);
      break;

    case SCROLL_SPEED_MENU:
      switch (encoder_parameters.position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.scroll_speed_x);            break;
        case 2: Serial.print(menu_items.scroll_speed_y);            break;
      }
      break;

    case FAN_SPEED_MENU:
      Serial.print(encoder_parameters.position);
      Serial.print(divide_string);
      Serial.print(menu_limits.fan_speed_menu);
      break;

    case MIN_FAN_SPEED_MENU:
      Serial.print(fan_parameters.fan_minimum);
      Serial.print(divide_string);
      Serial.print(menu_limits.minimum_fan_speed_menu);
      break;

    //      case SD_FOLDERS_MENU:


    case LED_STRIP_BRIGHTNESS_MENU:
      Serial.print(led_strip_parameters.target_brightness);
      Serial.print(divide_string);
      Serial.print(menu_limits.led_strip_brightness_menu);
      break;

    case TEXT_COLOUR_RED:
      Serial.print(text_parameters[menu.get_selected_object()].red);
      Serial.print(divide_string);
      Serial.print(menu_limits.text_colour_red_menu);
      break;

    case TEXT_COLOUR_GREEN:
      Serial.print(text_parameters[menu.get_selected_object()].green);
      Serial.print(divide_string);
      Serial.print(menu_limits.text_colour_green_menu);
      break;

    case TEXT_COLOUR_BLUE:
      Serial.print(text_parameters[menu.get_selected_object()].blue);
      Serial.print(divide_string);
      Serial.print(menu_limits.text_colour_blue_menu);
      break;

    case TEXT_COLOUR_HUE:
      Serial.print(text_parameters[menu.get_selected_object()].hue);
      Serial.print(" -> range: ");
      Serial.print(menu_limits.text_colour_hue_min);
      Serial.print(divide_string);
      Serial.print(menu_limits.text_colour_hue_max);
      break;

    case SCROLL_SPEED_MENU_X:
      Serial.print(text_cursor[menu.get_selected_object()].x_pos_dir - 128);
      Serial.print(" -> range: -");
      Serial.print(menu_limits.scroll_speed_menu - 129);
      Serial.print(divide_string);
      Serial.print(menu_limits.scroll_speed_menu - 128);
      break;

    case SCROLL_SPEED_MENU_Y:
      Serial.print(text_cursor[menu.get_selected_object()].y_pos_dir - 128);
      Serial.print(" -> range: -");
      Serial.print(menu_limits.scroll_speed_menu - 129);
      Serial.print(divide_string);
      Serial.print(menu_limits.scroll_speed_menu - 128);
      break;

    case TEXT_OBJ_SELECTION_MENU:
      switch (encoder_parameters.position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.text_obj_0_menu);           break;
        case 2: Serial.print(menu_items.text_obj_1_menu);           break;
        case 3: Serial.print(menu_items.text_obj_2_menu);           break;
        case 4: Serial.print(menu_items.text_obj_3_menu);           break;
        case 5: Serial.print(menu_items.text_obj_4_menu);           break;
      }
      break;
  }

}




void Host::print_ldr_config() {


  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pins \tMax1 \tMin1 \tMax2 \tMin2 \tAdjusted Reading \tLive Raw Readings \tInstruction"));
  }

  Serial.print(light_sensor_parameters.pin1);
  space();
  Serial.print(light_sensor_parameters.pin2);
  tab();

  Serial.print(light_sensor_parameters.config_max1);
  tab();
  Serial.print(light_sensor_parameters.config_min1);
  tab();
  Serial.print(light_sensor_parameters.config_max2);
  tab();
  Serial.print(light_sensor_parameters.config_min2);

  tab();

  Serial.print(light_sensor_parameters.reading1);
  if (light_sensor_parameters.reading1 < 100)
    space();
  space();
  Serial.print(light_sensor_parameters.reading2);

  tab(); tab(); tab();
  int temp_reading = map(analogRead(light_sensor_parameters.pin1), 0, 1024, 0, 255);

  Serial.print(temp_reading);
  if (temp_reading < 100)
    space();
  space();
  Serial.print(map(analogRead(light_sensor_parameters.pin2), 0, 1024, 0, 255));
  tab(); tab(); tab();

  if (light_sensor_parameters.reading1 >= 255)
    Serial.print(F("WARNING: sensor 1 saturating, increase max1 threshold"));
  else if (light_sensor_parameters.reading2 >= 255)
    Serial.print(F("WARNING: sensor 2 saturating, increase max2 threshold"));

  else if (light_sensor_parameters.reading1 <= 0)
    Serial.print(F("WARNING: sensor 1 saturating, decrease min1 threshold"));
  else if (light_sensor_parameters.reading2 <= 0)
    Serial.print(F("WARNING: sensor 2 saturating, decrease min2 threshold"));
  else
    Serial.print(F("Set range of max and min greater than range of raw readings"));
  Serial.println();
}



void Host::print_bits(uint32_t var, byte digits, byte units, bool carriage_return) {
  uint32_t test;
  if (units == BIN)
    test = (1 << (digits - 1));
  else
    test = (0xF << (digits - 1));

  for (test; test; ) {
    if (units == BIN) {
      Serial.write(var  & test ? '1' : '0');
      test >>= 1;
    }
    else if (units == HEX) {
      Serial.print(var & test, HEX);
      test >>= 4;
    }

  }
  if (carriage_return)
    Serial.println();
}


bool Host::command_contains_num(byte data_to_report) {
  switch (data_to_report) {

    case REPORT_TEXT:
    case REPORT_TEXT_FRAME_HISTORY:
    case REPORT_MENU_FRAME_HISTORY:
    case REPORT_POS_FRAME_HISTORY:
    case REPORT_SENSOR_FRAME_HISTORY: return true;  //if any of above expect command contains a number, so return true

    default: return false; //else return false
  }
}

bool Host::print_frame_hist(byte frame_type, byte screen_num) {

  byte cursor_index = 0;

  switch (frame_type) {
    case TEXT_FRAME_TYPE:        cursor_index = text_frame_history[screen_num].history_index;        break;
    case POS_FRAME_TYPE:         cursor_index = pos_frame_history[screen_num].history_index;         break;
    case SENSOR_FRAME_TYPE:      cursor_index = sensor_data_frame_history[screen_num].history_index; break;
    case MENU_FRAME_TYPE:        cursor_index = menu_frame_history[screen_num].history_index;        break;
    default: return false;
  }


  for (byte i = 0; i < FRAME_HISTORY_MEMORY_DEPTH; i++) {

    if (i == cursor_index)
      Pointer();
    else
      space(); space(); space();

    for (byte j = 0; j < MEGA_SERIAL_BUFFER_LENGTH; j++) {

      switch (frame_type) {
        case TEXT_FRAME_TYPE:        Serial.print(text_frame_history[screen_num].frame_content[i][j]);        break;
        case POS_FRAME_TYPE:         Serial.print(pos_frame_history[screen_num].frame_content[i][j]);         break;
        case SENSOR_FRAME_TYPE:      Serial.print(sensor_data_frame_history[screen_num].frame_content[i][j]); break;
        case MENU_FRAME_TYPE:        Serial.print(menu_frame_history[screen_num].frame_content[i][j]);        break;
      }
      space();
    }
    Serial.println();
  }

  Serial.println();
  return true;
}





//________HostNativeUSB__________

void HostNativeUSB::init_native_usb() {

  SerialUSB.begin(115200);
  int USB_timeout = millis() + NATIVE_USB_TIMEOUT_PERIOD;
  while (!SerialUSB && millis() < USB_timeout) {} // wait for serial port to connect. Needed for native USB port only

  while (SerialUSB.available() > 0) SerialUSB.read();
  Serial.println(F("Native usb init done"));
}

void HostNativeUSB::push_serial(int loc, String data) {

  String tx = data_out_keyword + space_colon_string + types[loc] + space_colon_string + data;
  SerialUSB.print(tx);

}



void HostNativeUSB::get_serial() {

  if (SerialUSB.available() > 0) {

    Serial.println("recieved request");

    String rx_type = SerialUSB.readString();    //pi sends type first
    int loc = type_ok(rx_type);
    if (loc == -1)
      SerialUSB.print('N');
    else    {
      SerialUSB.print('Y');

      int timeout = micros() + 10000;

      while (SerialUSB.available() == 0 && micros() < timeout) {} //wait for pi response
      if (!micros() >= timeout) { //if timeout skip to end
        delay(2); //short delay to allow some data to arrive

        String rx_string = SerialUSB.readString();
        put_data_into_loc(rx_string, loc);
      }
      else
        Serial.print(F("Native usb timeout waiting for pi"));
    }
  }
}


void HostNativeUSB::request_data(byte location) {

  // a note on the location variable
  // this variable is encoded.
  // values in range 0-9 correspond to string objects 0-9
  // value 10 is requesting the next instruction
  // value 20 is requesting ip address
  // etc...
  // see put_data_into_loc funcition below, though be aware loc is /10 to better extract stings

  if (location >= 0 && location < NUM_USB_COMMANDS) { //sanity check location
    String tx = request_keyword + space_colon_string + location;
    SerialUSB.print(tx);
    int timeout = micros() + 10000; //more fidelity using micros() for small time period
    while (SerialUSB.available() == 0 && micros() < timeout) {}
    if (!micros() >= timeout) {
      delay(2); //short delay to allow some data to arrive

      String rx_string = SerialUSB.readString();
      byte obj_num = location % 10; //the obj_num is encoded into the units part of the location, a location of 02 indicated request string 2 (text_str[2])
      put_data_into_loc(rx_string, location / 10, obj_num);
    }
    else
      Serial.print(F("Native usb timeout waiting for pi"));
  }
}


void HostNativeUSB::put_data_into_loc(String rx_string, byte loc, byte obj_num) {

  switch (loc) {
    case 0:
      Serial.print(F("New display text: "));
      Serial.println(rx_string);
      for (int i = 0; i < rx_string.length(); i++) {
        if (i < MAX_TWEET_SIZE - 3)
          text_str[obj_num][i] = rx_string[i];
        else if (i >= MAX_TWEET_SIZE - 5 && i < MAX_TWEET_SIZE) {
          if (i >= MAX_TWEET_SIZE - 4 && i < MAX_TWEET_SIZE - 1)
            text_str[obj_num][i] = '.'; //add ellipsis at the end of a long string
          else
            text_str[obj_num][i] = '\0';  // and endstring character
        }
      }
      break;

    case 1:
      Serial.print(F("New instruciton: "));
      Serial.println(rx_string);
      Serial.println(F("Instructions not implemented"));

      break;

    case 2:
      Serial.print(F("IP address: "));
      Serial.println(rx_string);
      Serial.println(F("IP address not implemented"));

      break;
    case 3:
      Serial.print(F("Network: "));
      Serial.println(rx_string);
      Serial.println(F("Network not implemented"));

      break;
    case 4:
      Serial.print(F("Git commit message: "));
      Serial.println(rx_string);
      Serial.println(F("Git commit message not implemented"));

      break;
    case 5:
      Serial.print(F("Git commit tag: "));
      Serial.println(rx_string);
      Serial.println(F("Git commit tag not implemented"));

      break;
    case 6:
      Serial.print(F("Git commit auther: "));
      Serial.println(rx_string);
      Serial.println(F("Git commit auther not implemented"));

      break;
    case 7:
      Serial.print(F("Ping Requested:"));



      break;

    default:
      Serial.print(F("Did not recognise host command: "));
      Serial.println(rx_string);
  }
}

int HostNativeUSB::type_ok(String rx_type)
{
  if (rx_type == types[0])     //new string to display
    return 0;
  else if (rx_type == types[1])  //coded instruction
    return 1;
  else if (rx_type == types[2])  // current ip address
    return 2;
  else if (rx_type == types[3])  //network pi currently connected to
    return 3;
  else if (rx_type == types[4])   //git commit mgs
    return 4;
  else if (rx_type == types[5])   //git commit tag
    return 5;
  else if (rx_type == types[6])   //git commit auther
    return 6;
  else if (rx_type == types[7])   //ping
    return 7;
  else
    return -1;
}


void HostNativeUSB::check_connection() {

  int USB_timeout = millis() + NATIVE_USB_TIMEOUT_PERIOD;

  while (!SerialUSB && millis() < USB_timeout) {} // wait for serial port to connect

  if (millis() >= USB_timeout)
    return;
  else
    request_data(7);  //send ping command
  return;
}

#endif // Host_CPP
