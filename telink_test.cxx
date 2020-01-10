/** \file telink_test.cxx
 *  Simple command line example demonstrating basic control of a Telink light device.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#include <thread>
#include <iostream>

#include "telink_light.h"
	
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
	
	ble_light.load_scenario(SCENARIO_SEA, 8); // set light in Sea scenario, with speed = 8
	std::this_thread::sleep_for(std::chrono::seconds(5));
	
  // create custom scenario
  TelinkScenario custom_scenario;
  // add 6 colors to scenario
  for (int i=0; i<6; i++) {
    TelinkColor colorn(255, i*40, 255-i*40, 100);
    custom_scenario.add_color(colorn);
  }
  // set custom scenario on device as custom scenario 3
  ble_light.edit_scenario(SCENARIO_CUSTOM_3, custom_scenario);
  // set light in new custom scenario, with speed = 3
	ble_light.load_scenario(SCENARIO_CUSTOM_3, 3);
  // set alarm with custom scenario to turn on at 12:30:00 every day except Sunday
  std::vector<bool> days = {false, true, true, true, true, true, true};
  ble_light.set_alarm(1, days, 12, 30, 0, SCENARIO_CUSTOM_3);
  
  ble_light.set_music_mode(true); // set device in music mode
  
  /* initialize random seed: */
  std::srand(time(NULL));
  // change light color randomly
  while (1) {
    ble_light.set_color(std::rand() % 0xff, std::rand() % 0xff, std::rand() % 0xff);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

}
