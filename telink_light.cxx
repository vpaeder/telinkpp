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
    return {this->brightness, this->R, this->G, this->B, this->Y, this->W, 0, 0};
  }
  
  
  void TelinkLightMode::add_color(TelinkColor color, unsigned char speed) {
    this->colors.push_back(color);
    this->speeds.push_back(speed & 0xf);
  }
  
  void TelinkLightMode::add_color(TelinkColor color) {
    this->add_color(color, 7);
  }
  
  void TelinkLightMode::replace_color(int color_index, TelinkColor color) {
    this->colors[color_index] = color;
  
  }
  
  void TelinkLightMode::remove_color(int color_index) {
    this->colors.erase(this->colors.begin() + color_index);
    this->speeds.erase(this->speeds.begin() + color_index);
  }
  
  void TelinkLightMode::set_speed(int color_index, unsigned char speed) {
    this->speeds[color_index] = speed & 0xf;
  }
  
  std::string TelinkLightMode::get_bytes(int color_index) const {
    std::string packet = {0, color_index == this->colors.size()-1, static_cast<char>(0x10 + this->speeds[color_index]), static_cast<char>(0x10*color_index + this->colors.size())};
    return packet + this->colors[color_index].get_bytes();
  }
  
  
  void TelinkLight::query_group_id() {
    this->send_packet(COMMAND_GROUP_ID_QUERY, {0x0A, 0x01});
  }
  
  void TelinkLight::query_time() {
    this->send_packet(COMMAND_TIME_QUERY, {0x10});
  }
  
  void TelinkLight::query_alarm() {
    this->send_packet(COMMAND_ALARM_QUERY, {0x10});
  }
  
  void TelinkLight::query_device_info() {
    this->send_packet(COMMAND_DEVICE_INFO_QUERY, {0x10});
  }
  
  void TelinkLight::query_device_version() {
    this->send_packet(COMMAND_DEVICE_INFO_QUERY, {0x10, 0x02});
  }
  
  void TelinkLight::query_scene() {
    this->send_packet(COMMAND_SCENE_QUERY, {0x0A, 0x01});
  }
  
  void TelinkLight::query_status() {
    this->send_packet(COMMAND_STATUS_QUERY, {0x10});
  }
  
  void TelinkLight::set_time() {
    time_t now = std::time( 0 );
    tm *ltm = std::localtime( &now );
    int year = 1900 + ltm->tm_year;
    this->send_packet(COMMAND_TIME_SET, {static_cast<char>(year & 0xff), static_cast<char>(year >> 8), static_cast<char>(ltm->tm_mon + 1), static_cast<char>(ltm->tm_mday), static_cast<char>(ltm->tm_hour), static_cast<char>(ltm->tm_min), static_cast<char>(ltm->tm_sec)});
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
  
  void TelinkLight::set_mesh_id(int mesh_id) {
    TelinkMesh::set_mesh_id(mesh_id);
    this->send_packet(COMMAND_ADDRESS_EDIT, {static_cast<char>(mesh_id & 0xff), static_cast<char>((mesh_id >> 8) & 0xff)});
  }
  
  void TelinkLight::query_mesh_id() {
    this->send_packet(COMMAND_ADDRESS_EDIT, {0xff, 0xff});
  }
  
  void TelinkLight::add_group(unsigned char group_id) {
    this->send_packet(COMMAND_GROUP_EDIT, {0x01, group_id, 0});
  }
  
  void TelinkLight::add_scene(unsigned char scene_id) {
    this->send_packet(COMMAND_SCENE_EDIT, {0x01, scene_id});
  }
  
  void TelinkLight::set_brightness(int brightness) {
    this->brightness = std::min(100, std::max(brightness, 0));
    this->send_packet(COMMAND_LIGHT_ATTRIBUTES_SET, {this->brightness, 0, 0, 0, 0, 0, 0, 1});
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
  
  void TelinkLight::load_mode(unsigned char mode_id, unsigned char speed) {
    this->send_packet(COMMAND_LIGHT_MODE_LOAD, {mode_id, speed, this->brightness});
  }
  
  void TelinkLight::set_alarm(unsigned char alarm_id, const std::vector<bool> & weekdays, unsigned char hour, unsigned char minute, unsigned char second, unsigned char action) {
    unsigned char action_code;
    std::string data = {2, alarm_id, 0, 0, 0, hour, minute, second, 0, 0};
    if (action == 0 || action == 1) {
      data[2] = 0x90 + action;
    } else {
      data[2] = 0x92;
      data[8] = action;
    }
    // compile byte for days
    for (int i=0; i<weekdays.size(); i++)
      data[4] += weekdays[i] << i;
    
    this->send_packet(COMMAND_ALARM_EDIT, data);
  }
  
  void TelinkLight::set_alarm(unsigned char alarm_id, bool state) {
    this->send_packet(COMMAND_ALARM_EDIT, {state ? '\3' : '\4', alarm_id});
  }
  
  void TelinkLight::delete_alarm(unsigned char alarm_id) {
    this->send_packet(COMMAND_ALARM_EDIT, {1, alarm_id});
  }
  
  void TelinkLight::edit_mode(unsigned char mode_id, TelinkLightMode & light_mode) {
    this->load_mode(0xff, 7); // mode need not be the edited one
    
    for (int i=0; i<light_mode.get_size(); i++) {
      std::string packet = light_mode.get_bytes(i);
      packet[0] = mode_id;
      this->send_packet(COMMAND_LIGHT_MODE_EDIT, packet);
    }
  }
  
  void TelinkLight::parse_command(const std::string & packet) {
    unsigned int received_id;
    if (packet[7] == COMMAND_ONLINE_STATUS_REPORT) {
      received_id = packet[10];
      if (this->mesh_id == 0)
        this->mesh_id = received_id;
    } else {
      received_id = packet[3];
    }
    
    if (this->mesh_id == received_id || received_id == 0) {
      // NOTE: from specs, received_id == 0xffff targets all connected devices,
      //  but presently received_id will never exceed 0xff.
      // received_id == 0 targets the connected device only
      
      if (packet[7] == COMMAND_ONLINE_STATUS_REPORT) {
        this->brightness = packet[12];
        this->state = ~(packet[13] & 1);
      
      } else if (packet[7] == COMMAND_STATUS_REPORT) {
        this->brightness = packet[10];
        unsigned char R = packet[11];
        unsigned char G = packet[12];
        unsigned char B = packet[13];
        unsigned char W = packet[15];
      
      } else if (packet[7] == COMMAND_TIME_REPORT) {
        unsigned int year = packet[10] + (packet[11] << 8);
        unsigned char month = packet[12];
        unsigned char day = packet[13];
        unsigned char hour = packet[14];
        unsigned char minute = packet[15];
        unsigned char second = packet[16];
        char timestamp[9], datestamp[9];
        std::sprintf( timestamp, "%02d:%02d:%02d", hour, minute, second );
        std::sprintf( datestamp, "%04d-%02d-%02d", year, month, day );
        std::cout << "Lamp date: " << datestamp << ", time: " << timestamp << std::endl;
      
      } else if (packet[7] == COMMAND_ADDRESS_REPORT) {
        // mesh_id = packet[10]
        
      } else if (packet[7] == COMMAND_ALARM_REPORT) {
        
      
      } else if (packet[7] == COMMAND_DEVICE_INFO_REPORT) {
        if (packet[19] == 0) { // packet contains device info
          
        } else if (packet[19] == 2) { // packet contains device version
          
        }
      
      } else if (packet[7] == COMMAND_GROUP_ID_REPORT) {
      
      } else if (packet[7] == COMMAND_SCENE_REPORT) {
      
      }
    } 
  
  }
  
}
