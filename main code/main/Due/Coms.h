
/*
   Parent class used to genreate the frames, parse them, do error checking and any other functionallity
   Actually transmitting the frames is done in the daughter class of the protocol used

*/

#ifndef Coms_H
#define Coms_H

#include "Config_Local.h"

//https://github.com/ivanseidel/DueTimer
#include "DueTimer.h"       // wrapper library to handle timers 0-4 

//https://github.com/antodom/soft_uart
#include "soft_uart.h"

// variables for initialising soft serial for comms
using namespace arduino_due;
#define COMS_SPEED 38400         //speed of coms between due and megas when using serial
#define SOFT_UART_BIT_RATE COMS_SPEED // 57600 38400 1200 19200 9600 115200 300
#define RX_BUF_LENGTH 256 // software serial port's reception buffer length 
#define TX_BUF_LENGTH 256 // software serial port's transmision buffer length


#define HEADER_LENGTH 4  //length,type,num frames, frame no
#define TRAILER_LENGTH 1 //just checksum
#define DATA_IDENTIFIER_BYTE  2   //frame type byte
#define FRAME_LENGTH_BYTE  1
#define FRAME_DATA_LENGTH MEGA_SERIAL_BUFFER_LENGTH-HEADER_LENGTH-TRAILER_LENGTH
#define FRAME_OVERHEAD HEADER_LENGTH+TRAILER_LENGTH        //number of overhead bytes -> frame length, frame type, num frames, frame num, checksum

//#define MEGA_SERIAL_BUFFER_LENGTH 32

#define WAIT_TIME_FOR_USB_PORT_CONNECTION 5000

#define MEGA_SERIAL_BUFFER_LENGTH 32
#define MAX_TWEET_SIZE 280
#define MAX_FRAME_SIZE MAX_TWEET_SIZE+((MAX_TWEET_SIZE % MEGA_SERIAL_BUFFER_LENGTH)*FRAME_OVERHEAD)   // max amount of data to be sent in one go by either the text_frame and limit for sensor_data_frame
                                                                                                      // need different approach for bitmaps...






struct Frame {            //frame details for the due, seperate one for the mega below

  byte frame_buffer[MEGA_SERIAL_BUFFER_LENGTH]; // use to pack single frame
  byte extended_frame_buffer[MAX_FRAME_SIZE];   // large buffer to store sequence of frames to transmit later, eg text_frame generated and in queue
  bool send_extended_buffer = false;
  byte frame_length = 0;
  byte frame_type = 1;
  byte checksum = 0;
  byte num_frames = 0;
  byte this_frame = 0;
  byte header_checksum = 0;   //can calculate in advance for some pos and menu frames to save time
  uint16_t checksum_address = 0;   // can also allocate in advance for pos and menu frames
  bool frame_queued = false;    //queue frame this loop to send at the beginning of next
  
};



#ifdef ENABLE_ERROR_CHECKING
struct Nack_details {
  byte mega1_failed_frame_type = 0;   // the type of frame last sent, to recalculate
  byte mega2_failed_frame_type = 0;
  byte mega3_failed_frame_type = 0;
  byte mega4_failed_frame_type = 0;

  byte mega1_failed_frame_number = 0;   // the frame number that failed
  byte mega2_failed_frame_number = 0;
  byte mega3_failed_frame_number = 0;
  byte mega4_failed_frame_number = 0;
};
Nack_details nack;
#endif // ENABLE_ERROR_CHECKING




class Coms {

  private:


  public:

    Coms() {}

    int startup_handshake();      //startup sequence to ensure due boots first and transmission begins when all megas are ready
    int send_disp_string_frame(int address);                             //complete function to send strings over i2c to display on specified screen
    void pack_disp_string_frame(int frame_type, int frame_offset);        //function to pack a frame of text to display
    void build_pos_frame();                                               //function to send the xy coordinates along with a number of other related variables
    void pack_xy_coordinates() ;                                          //function to pack the 4 bytes to send the x and y positions of the text cursor
//    int send_all_calibration_data(int address);                          //function to send all data calibration
//    bool send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset);  //function to send specific value
    int send_all_pos_on_interrupt();     // function to send pos data to all megas periodically based on timer interrupt
    int send_all_pos_now();
    int send_all_text();    // send the text frame to all megas
    void print_frame();      //print the currently stored frame to the serial monitor

    int get_text_colour_hue(int byte_number){};                            //function to return the MSB or LSB of the current hue value to send over i2c

    int calc_delay();

    int build_menu_data_frame(int menu_number, int encoder_position);    //function to build the frame to send menu info
    int init_frames();  //set length elements of frames

    //  todo
    void echo_menu();
    int generate_checksum();
    int error_check();
    
    int get_frame_code();
    int get_data();             //extract data from frame



};




#endif  //Coms_H

