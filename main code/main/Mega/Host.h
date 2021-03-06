#ifndef Host_H
#define Host_H

#include "Arduino.h"

#define HOST_SERIAL_SPEED 250000

// NB numbering here must match index of string in items array in Serial_Sub_Menu struct
#define STOP_REPORT           0
#define REPORT_TEXT           1   //apended to display object number 0 - N
#define REPORT_CONFIG         2
#define REPORT_MENU           3



#define HEADER_PRINT_INCREMENT  8     // make this a power of 2, auto overflow...
#define MESSAGE_DELAY_PERIOD    400   //delay this many ms between message prints
//class to manage all functions regarding communication with due host device (probably a pi)


class Host {

  private:

    byte data_to_report = STOP_REPORT;    //default to not printing
    byte header_print_counter = 0;

    void print_menu_tree();
    void print_text(byte *command);
    void print_config();

    byte data_set_LUT(byte data_set[6]);
    void position_to_menu_value();
    void print_menu_tree_options(int cur_menu = -1); //if not argument provided display all sub menus of current menu


  public:
    Host() {}
    void init_serial();
    void check_serial();    //to read incomming data
    void print_messages();

    void print_bits(uint32_t var, byte digits, byte units, bool carriage_return = false);
    inline void println_bits(uint32_t var, byte digits, byte units) {
      print_bits(var, digits, units, true);
    }
};

#endif //Host_H
