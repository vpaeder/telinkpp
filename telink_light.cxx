/** \file telink_light.cxx
 *  Class handling a Bluetooth LE light with Telink protocol.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#include <iostream>
#include <algorithm>
#include "telink_light.h"

namespace telink {
  
  void TelinkColor::set_brightness(unsigned char brightness) {
    this->brightness = brightness % 101;
  }
  
  void TelinkColor::set_color(unsigned char R, unsigned char G, unsigned char B) {
    this->R = R;
    this->G = G;
    this->B = B;
    this->W = 0;
    this->Y = 0;
  }
  
  void TelinkColor::set_temperature(int temperature) {
    unsigned char W = 0xff, Y = 0xff;
    this->R = 0;
    this->G = 0;
    this->B = 0;
    temperature = std::max(std::min(6500, temperature), 2700);
    if (temperature > 4600) {
      Y = static_cast<unsigned char>((((float) (6500 - temperature)) * 255.0f) / 1900.0f);
    } else {
      W = static_cast<unsigned char>((((float) (temperature - 2700)) * 255.0f) / 1900.0f);
    }
    this->set_temperature(Y, W);
  }
  
  void TelinkColor::set_temperature(unsigned char Y, unsigned char W) {
    if (this->brightness == 0)
      this->brightness = 3;
    this->W = W;
    this->Y = Y;
    this->R = 0;
    this->G = 0;
    this->B = 0;
  }
  
  std::string TelinkColor::get_bytes() const {
    return {schar(this->brightness), schar(this->R), schar(this->G), schar(this->B), schar(this->Y), schar(this->W), 0, 0};
  }
  
  
  void TelinkScenario::add_color(TelinkColor color, unsigned char speed) {
    this->colors.push_back(color);
    this->speeds.push_back(speed & 0xf);
  }
  
  void TelinkScenario::add_color(TelinkColor color) {
    this->add_color(color, 7);
  }
  
  void TelinkScenario::replace_color(int color_index, TelinkColor color) {
    this->colors[color_index] = color;
  
  }
  
  void TelinkScenario::remove_color(int color_index) {
    this->colors.erase(this->colors.begin() + color_index);
    this->speeds.erase(this->speeds.begin() + color_index);
  }
  
  void TelinkScenario::set_speed(int color_index, unsigned char speed) {
    this->speeds[color_index] = speed & 0xf;
  }
  
  std::string TelinkScenario::get_bytes(int color_index) const {
    std::string packet = {0, color_index == this->colors.size()-1, schar(0x10 + this->speeds[color_index]), schar(0x10*color_index + this->colors.size())};
    return packet + this->colors[color_index].get_bytes();
  }
  
  void TelinkLight::query_alarm() {
    this->send_packet(COMMAND_ALARM_QUERY, {0x10});
  }
  
  void TelinkLight::query_scenario(unsigned char scenario_id) {
    this->send_packet(COMMAND_SCENARIO_QUERY, {0, 0, schar(scenario_id), schar(0xff)});
  }
  
  void TelinkLight::query_status() {
    this->send_packet(COMMAND_STATUS_QUERY, {0x10});
  }
  
  void TelinkLight::set_temperature(int temperature) {
    TelinkColor color(temperature, this->brightness);
    std::string packet = color.get_bytes();
    packet[6] = this->music_mode;
    this->send_packet(COMMAND_LIGHT_ATTRIBUTES_SET, packet);
  }

  void TelinkLight::set_state(bool on_off) {
    this->state = on_off;
    this->send_packet(COMMAND_LIGHT_ON_OFF, {on_off, 0, 0});
  }
  
  void TelinkLight::add_scenario(unsigned char scenario_id) {
    this->send_packet(COMMAND_SCENARIO_EDIT, {0x01, schar(scenario_id)});
  }
  
  void TelinkLight::delete_scenario(unsigned char scenario_id) {
    this->send_packet(COMMAND_SCENARIO_EDIT, {0x00, schar(scenario_id)});
  }
  
  void TelinkLight::set_brightness(int brightness) {
    this->brightness = std::min(100, std::max(brightness, 0));
    this->send_packet(COMMAND_LIGHT_ATTRIBUTES_SET, {schar(this->brightness), 0, 0, 0, 0, 0, 0, 1});
  }

  void TelinkLight::set_color(unsigned char R, unsigned char G, unsigned char B) {
    TelinkColor color(R, G, B, this->brightness);
    std::string packet = color.get_bytes();
    packet[6] = this->music_mode;
    this->send_packet(COMMAND_LIGHT_ATTRIBUTES_SET, packet);
  }
  
  void TelinkLight::set_music_mode(bool music_mode) {
    this->music_mode = music_mode;
  }
  
  void TelinkLight::load_scenario(unsigned char scenario_id, unsigned char speed) {
    this->send_packet(COMMAND_SCENARIO_LOAD, {schar(scenario_id), schar(speed), schar(this->brightness)});
  }
  
  void TelinkLight::set_alarm(unsigned char alarm_id, const std::vector<bool> & weekdays, unsigned char hour, unsigned char minute, unsigned char second, unsigned char action) {
    unsigned char action_code;
    std::string data = {2, schar(alarm_id), 0, 0, 0, schar(hour), schar(minute), schar(second), 0, 0};
    if (action == 0 || action == 1) {
      data[2] = 0x90 + action;
    } else {
      data[2] = 0x92;
      data[8] = action;
    }
    // compile byte for days
    for (int i=0; i<weekdays.size(); i++)
      data[4] |= weekdays[i] << i;
    
    this->send_packet(COMMAND_ALARM_EDIT, data);
  }
  
  void TelinkLight::set_alarm(unsigned char alarm_id, bool state) {
    this->send_packet(COMMAND_ALARM_EDIT, {state ? '\3' : '\4', schar(alarm_id)});
  }
  
  void TelinkLight::delete_alarm(unsigned char alarm_id) {
    this->send_packet(COMMAND_ALARM_EDIT, {1, schar(alarm_id)});
  }
  
  void TelinkLight::edit_scenario(unsigned char scenario_id, TelinkScenario & scenario) {
    this->load_scenario(0xff, 7); // scenario need not be the edited one
    
    for (int i=0; i<scenario.get_size(); i++) {
      std::string packet = scenario.get_bytes(i);
      packet[0] = scenario_id;
      this->send_packet(COMMAND_SCENARIO_EDIT, packet);
    }
  }
  
  void TelinkLight::parse_online_status_report(const std::string & packet) {
    this->brightness = packet[12];
    this->state = ~(packet[13] & 1); // 0x40 = light on, 0x41 = light off
  }
  
  void TelinkLight::parse_status_report(const std::string & packet) {
    this->brightness = packet[10];
    unsigned char R = packet[11];
    unsigned char G = packet[12];
    unsigned char B = packet[13];
    unsigned char W = packet[15];
  }
  
  void TelinkLight::parse_alarm_report(const std::string & packet) {
    int alarm_count = packet[19];
    unsigned char alarm_id = packet[11];
    unsigned char alarm_action = packet[12];
    unsigned char scenario_id = 0xff;
    if (alarm_action & 2) // alarm action: use scenario with id = packet[18]
      scenario_id = packet[18];
    bool alarm_state = alarm_action >> 7;
    std::vector<bool> weekdays(7);
    for (int i=0; i<7; i++)
      weekdays[i] = (packet[14] >> i) & 0x01;
    
    int hour = packet[15];
    int minute = packet[16];
    int second = packet[17];
    
    // packet[13] contains details on months, but since it isn't set
    // in any example code I could find, I leave it aside for now
    // packet[10] seems to always contain 0xA5
  }
  
  void TelinkLight::parse_scenario_report(const std::string & packet) {
    int scenario_size = packet[12] & 0xf;
    int scenario_id = packet[10];
    int color_speed = packet[11] - 0x10;
    int color_index = (packet[12] - scenario_size) >> 4;
    int color_brightness = packet[13];
    int color_R = packet[14];
    int color_G = packet[15];
    int color_B = packet[16];
    int color_Y = packet[17];
    int color_W = packet[18];
  }
  
  void TelinkLight::parse_command(const std::string & packet) {
    if (this->check_packet_validity(packet)) {
      if (static_cast<unsigned char>(packet[7]) == COMMAND_ONLINE_STATUS_REPORT) {
        this->parse_online_status_report(packet);
      
      } else if (static_cast<unsigned char>(packet[7]) == COMMAND_STATUS_REPORT) {
        this->parse_status_report(packet);
        
      } else if (static_cast<unsigned char>(packet[7]) == COMMAND_ALARM_REPORT) {
        this->parse_alarm_report(packet);
        
      } else if (static_cast<unsigned char>(packet[7]) == COMMAND_SCENARIO_REPORT) {
        this->parse_scenario_report(packet);
      
      } else {
        TelinkMesh::parse_command(packet);
      }
    } 
  
  }
  
}

