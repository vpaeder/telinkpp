/** \file telink_test.cxx
 *  Simple command line example demonstrating basic control of a Telink light device.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#include <thread>
#include <csignal>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "telink_light.h"

int main(int argc, char **argv) {

  using namespace telink;
  
  TelinkLight ble_light("AA:BB:CC:DD:EE:FF", "telink_mesh1", "123");
  
  // attempt connection
  if (!ble_light.connect()) return 1;
  
  ble_light.set_time(); // set device time to computer time
  ble_light.query_time(); // query device time
  ble_light.set_state(true); // turn light on
  ble_light.set_temperature(4600); // set white light temperature
  ble_light.set_brightness(100); // set light brightness to maximum
  ble_light.set_music_mode(true); // set device to music mode
  
  /* initialize random seed: */
  std::srand(time(NULL));
  // change light color randomly
  while (1) {
    ble_light.set_color(std::rand() % 0xff, std::rand() % 0xff, std::rand() % 0xff);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

}
