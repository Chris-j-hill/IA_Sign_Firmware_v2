

#ifndef MENUS_CPP
#define MENUS_CPP

#include "Menus.h"
#include "Graphics.h"


Menu_Struct menu_parameters;
extern Graphics graphics;


byte current_menu = 0;
byte previous_menu = 255;

uint16_t time_since_menu_last_changed = 0;
uint16_t time_since_menu_startup_run = 0;
uint16_t startup_counter = 0;



int Menu::init_menu_tree() {

#ifdef SKIP_INTITAL_STARTUP_SEQUENCE
  current_menu = DEFUALT_MENU;
#else
  current_menu = STARTUP;
#endif

}

void Menu::display_menu() {

  switch (current_menu) {
    case STARTUP:                     display_startup_sequence();
    case DEFAULT_MENU:                default_display(); break;
    case MAIN_MENU:                   display_main_menu(); break;
    case SCREEN_MODE_MENU:            display_screen_mode_menu(); break;
    case BRIGHTNESS_MENU:             display_brightness_menu(); break;
    case TEXT_SETTINGS_MENU:          display_text_settings_menu(); break;
    case FAN_SETTINGS_MENU:           display_fan_settings_menu(); break;
    case INTERNET_CONFIG_MENU:        display_internet_config_menu(); break;
    case SD_CARD_MENU:                display_SD_cards_menu(); break;
    case LED_STRIP_MENU:              display_led_strip_menu(); break;
    case TEXT_SIZE_MENU:              display_text_size_menu(); break;
    case TEXT_COLOUR_MENU:            display_text_colour_menu(); break;
    case SCROLL_SPEED_MENU:           display_scroll_speed_menu(); break;
    case FAN_SPEED_MENU:              display_fan_speed_menu(); break;
    case MIN_FAN_SPEED_MENU:          display_min_fan_speed_menu(); break;
    case SD_FOLDERS_MENU:             display_sd_folder_menu(); break;
    case LED_STRIP_BRIGHTNESS_MENU:   display_led_strip_brightness_menu(); break;
    case TEXT_COLOUR_RED:             display_text_colour_red_menu(); break;
    case TEXT_COLOUR_GREEN:           display_text_colour_green_menu(); break;
    case TEXT_COLOUR_BLUE:            display_text_colour_blue_menu(); break;

    default: current_menu = STARTUP;    //restart, run startup
  }
}

void Menu::display_startup_sequence() { // startup is to draw a circle expnding from the screen global centre

  graphics.set_object_colour(STARTUP_R, STARTUP_G, STARTUP_B);

  if (current_menu != previous_menu) {
    previous_menu = current_menu;   //edge detecter
    startup_counter = 1;
    time_since_menu_startup_run = millis();
  }
  if (startup_counter < STARTUP_RING_MAX_RADIUS) {} // do nothing

  else if (millis() - time_since_menu_startup_run > STARTUP_RING_EXPANSION_RATE - graphics.non_linear_startup_function(startup_counter)) { //otherwise increment counter at non linear rate
    time_since_menu_startup_run = millis();
    startup_counter++;
  }

  graphics.draw_circle(TOTAL_WIDTH / 2, SINGLE_MATRIX_HEIGHT, startup_counter);
}



#endif // MENUS_CPP
