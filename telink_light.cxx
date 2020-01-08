/** \file telink_light.cxx
 *  Class handling a Bluetooth LE light with Telink protocol.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#include <iostream>
#include "telink_light.h"

namespace telink {
  
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
    this->send_packet(COMMAND_TIME_SET, {(char)(year & 0xff), (char)(year >> 8), (char)(ltm->tm_mon + 1), (char)ltm->tm_mday, (char)ltm->tm_hour, (char)ltm->tm_min, (char)ltm->tm_sec});
  }
  
  void TelinkLight::set_temperature(int temperature) {
    unsigned char CCT_W = 255, CCT_Y = 255;
    temperature = std::max(std::min(6500, temperature), 2700);
    if (temperature > 4600) {
      CCT_Y = (unsigned char) ((((float) (6500 - temperature)) * 255.0f) / 1900.0f);
    } else {
      CCT_W = (unsigned char) ((((float) (temperature - 2700)) * 255.0f) / 1900.0f);
    }
    if (this->brightness == 0)
      this->brightness = 3;
    
    this->send_packet(COMMAND_LIGHT_ATTRIBUTES, {this->brightness, 0, 0, 0, CCT_Y, CCT_W, 0, 0});
  }

  void TelinkLight::set_state(bool on_off) {
    this->state = on_off;
    this->send_packet(COMMAND_LIGHT_ON_OFF, {on_off, 0, 0});
  }
  
  void TelinkLight::set_mesh_id(int mesh_id) {
    TelinkMesh::set_mesh_id(mesh_id);
    this->send_packet(COMMAND_ADDRESS_EDIT, {(char)(mesh_id & 0xff), (char)((mesh_id >> 8) & 0xff)});
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
    this->send_packet(COMMAND_LIGHT_ATTRIBUTES, {this->brightness, 0, 0, 0, 0, 0, 0, 1});
  }

  void TelinkLight::set_color(unsigned char R, unsigned char G, unsigned char B) {
    this->send_packet(COMMAND_LIGHT_ATTRIBUTES, {this->brightness, R, G, B, 0, 0, this->music_mode, 0});
  }
  
  void TelinkLight::set_music_mode(bool music_mode) {
    this->music_mode = music_mode;
  }
  
  void TelinkLight::load_mode(unsigned char mode_id, unsigned char speed) {
    this->send_packet(COMMAND_LIGHT_MODE_LOAD, {mode_id, speed, this->brightness});
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
        // check that mesh ID corresponds
        this->brightness = packet[12];
        this->state = ~(packet[13] & 1);
      
      } else if (packet[7] == COMMAND_STATUS_REPORT) {
        this->brightness = packet[10];
        unsigned char R = packet[11];
        unsigned char G = packet[12];
        unsigned char B = packet[13];
        unsigned char W = packet[15];
      
      } else if (packet[7] == COMMAND_TIME_REPORT) {
        // check that mesh ID corresponds
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
