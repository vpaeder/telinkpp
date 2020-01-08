/** \file telink_test.cxx
 *  Simple command line example demonstrating basic control of a Telink light device.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#include <thread>
#include <iostream>

#include "telink_light.h"

// mode ID definitions 
#define MODE_SEA 0x8e
#define MODE_COLD 0x89
#define MODE_3_COLOR_GRADIENT 0x90
#define MODE_3_COLOR_JUMP 0x8f
#define MODE_6_COLOR_GRADIENT 0x8a
#define MODE_6_COLOR_JUMP 0x8b
#define MODE_7_COLOR_GRADIENT 0x92
#define MODE_7_COLOR_JUMP 0x91
#define MODE_DINNER 0x87
#define MODE_FLAME 0x97
#define MODE_FOREST 0x96
#define MODE_FREQUENCY 0x83
#define MODE_GRADIENT 0x82
#define MODE_JUMP 0x81
#define MODE_LOOP 0x84
#define MODE_MORNING 0x85
#define MODE_NOON 0x86
#define MODE_RGB_DINNER 0x95
#define MODE_RGB_MORNING 0x93
#define MODE_RGB_NOON 0x94
#define MODE_RED_GRADIENT 0x8d
#define MODE_RELAX 0x98
#define MODE_WARN 0x88
#define MODE_WHITE 0x8c
#define MODE_WORK 0x99
	
int main(int argc, char **argv) {

  if (argc < 4) {
    std::cerr << "Run as: " << argv[0] << " <device_MAC_address> <device_name> <device_password>" << std::endl;
    exit(1);
  }
  
  using namespace telink;
  
  TelinkLight ble_light(argv[1], argv[2], argv[3]);
  
  // attempt connection
  if (!ble_light.connect()) return 1;
  
  ble_light.set_time(); // set device time to computer time
  ble_light.query_time(); // query device time
  ble_light.set_state(true); // turn light on
  ble_light.set_temperature(4600); // set white light temperature
  ble_light.set_brightness(100); // set light brightness to maximum
	
	ble_light.load_mode(MODE_SEA, 8); // set light in Sea mode, with speed = 8
	std::this_thread::sleep_for(std::chrono::seconds(5));
	
  ble_light.set_music_mode(true); // set device to music mode
  
  /* initialize random seed: */
  std::srand(time(NULL));
  // change light color randomly
  while (1) {
    ble_light.set_color(std::rand() % 0xff, std::rand() % 0xff, std::rand() % 0xff);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

}
