/** \file telink_light.h
 *  Class handling a Bluetooth LE light with Telink mesh protocol.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#ifndef __TELINK_LIGHT_H__
#define __TELINK_LIGHT_H__

#include "telink_mesh.h"

namespace telink {
  
  // Command codes
  #define COMMAND_SCENARIO_QUERY        0xC0
  #define COMMAND_SCENARIO_REPORT       0xC1
  #define COMMAND_SCENARIO_LOAD         0xF2
  #define COMMAND_SCENARIO_EDIT         0xF3
  #define COMMAND_OTA_UPDATE            0xC6
  #define COMMAND_QUERY_OTA_STATE       0xC7
  #define COMMAND_OTA_STATUS_REPORT     0xC8
  #define COMMAND_GROUP_ID_QUERY        0xDD
  #define COMMAND_GROUP_ID_REPORT       0xD4
  #define COMMAND_GROUP_EDIT            0xD7
  #define COMMAND_STATUS_QUERY          0xDA
  #define COMMAND_STATUS_REPORT         0xDB
  #define COMMAND_ONLINE_STATUS_REPORT  0xDC
  #define COMMAND_ADDRESS_EDIT          0xE0
  #define COMMAND_ADDRESS_REPORT        0xE1
  #define COMMAND_RESET                 0xE3
  #define COMMAND_TIME_QUERY            0xE8
  #define COMMAND_TIME_REPORT           0xE9
  #define COMMAND_TIME_SET              0xE4
  #define COMMAND_ALARM_QUERY           0xE6
  #define COMMAND_ALARM_REPORT          0xE7
  #define COMMAND_ALARM_EDIT            0xE5
  #define COMMAND_DEVICE_INFO_QUERY     0xEA
  #define COMMAND_DEVICE_INFO_REPORT    0xEB
  #define COMMAND_LIGHT_ON_OFF          0xF0
  #define COMMAND_LIGHT_ATTRIBUTES_SET  0xF1


  // scenario ID definitions
  #define SCENARIO_CUSTOM_1 0x00
  #define SCENARIO_CUSTOM_2 0x01
  #define SCENARIO_CUSTOM_3 0x02
  #define SCENARIO_CUSTOM_4 0x03
  #define SCENARIO_SEA 0x8e
  #define SCENARIO_COLD 0x89
  #define SCENARIO_3_COLOR_GRADIENT 0x90
  #define SCENARIO_3_COLOR_JUMP 0x8f
  #define SCENARIO_6_COLOR_GRADIENT 0x8a
  #define SCENARIO_6_COLOR_JUMP 0x8b
  #define SCENARIO_7_COLOR_GRADIENT 0x92
  #define SCENARIO_7_COLOR_JUMP 0x91
  #define SCENARIO_DINNER 0x87
  #define SCENARIO_FLAME 0x97
  #define SCENARIO_FOREST 0x96
  #define SCENARIO_FREQUENCY 0x83
  #define SCENARIO_GRADIENT 0x82
  #define SCENARIO_JUMP 0x81
  #define SCENARIO_LOOP 0x84
  #define SCENARIO_MORNING 0x85
  #define SCENARIO_NOON 0x86
  #define SCENARIO_RGB_DINNER 0x95
  #define SCENARIO_RGB_MORNING 0x93
  #define SCENARIO_RGB_NOON 0x94
  #define SCENARIO_RED_GRADIENT 0x8d
  #define SCENARIO_RELAX 0x98
  #define SCENARIO_WARN 0x88
  #define SCENARIO_WHITE 0x8c
  #define SCENARIO_WORK 0x99
  #define SCENARIO_DEFAULT 0xff
  
  
  /** \class TelinkColor
   *  \brief Class representing a color state.
   */
  class TelinkColor {
  private:
    /** \property unsigned char R
     *  \brief Red component
     */
    unsigned char R;
    
    /** \property unsigned char G
     *  \brief Green component
     */
    unsigned char G;
    
    /** \property unsigned char B
     *  \brief Blue component
     */
    unsigned char B;
    
    /** \property unsigned char Y
     *  \brief CCT Y parameter
     */
    unsigned char Y;
    
    /** \property unsigned char W
     *  \brief CCT W parameter
     */
    unsigned char W;
    
    /** \property unsigned char brightness
     *  \brief Light brightness
     */
    unsigned char brightness;
    
  public:
    /** \fn TelinkColor(unsigned char R, unsigned char G, unsigned char B, unsigned char brightness)
     *  \brief Object instantiation.
     *  \param Y : CCT Y value
     *  \param W : CCT W value
     *  \param brightness : brightness value, from 0 to 100%
     */
    TelinkColor(unsigned char R, unsigned char G, unsigned char B, unsigned char brightness) {
      this->set_brightness(brightness);
      this->set_color(R, G, B);
    }
    
    /** \fn TelinkColor(unsigned char Y, unsigned char W, unsigned char brightness)
     *  \brief Object instantiation.
     *  \param R : red component, from 0 to 255
     *  \param G : green component, from 0 to 255
     *  \param B : blue component, from 0 to 255
     *  \param brightness : brightness value, from 0 to 100%
     */
    TelinkColor(unsigned char Y, unsigned char W, unsigned char brightness) {
      this->set_brightness(brightness);
      this->set_temperature(Y, W);
    }
    
    /** \fn TelinkColor(int temperature, unsigned char brightness)
     *  \brief Object instantiation.
     *  \param temperature : equivalent temperature, from 2700 to 6500 K
     *  \param brightness : brightness value, from 0 to 100%
     */
    TelinkColor(int temperature, unsigned char brightness) {
      this->set_brightness(brightness);
      this->set_temperature(temperature);
    }
    
    /** \fn void set_brightness(unsigned char brightness)
     *  \brief Sets the color brightness.
     *  \param brightness : brightness value, from 0 to 100%
     */
    void set_brightness(unsigned char brightness);
    
    /** \fn void set_color(unsigned char R, unsigned char G, unsigned char B)
     *  \brief Sets the RGB color.
     *  \param R : red component, from 0 to 255
     *  \param G : green component, from 0 to 255
     *  \param B : blue component, from 0 to 255
     */
    void set_color(unsigned char R, unsigned char G, unsigned char B);
    
    /** \fn void set_temperature(int temperature)
     *  \brief Sets the color temperature using equivalent black body temperature.
     *  \param temperature : equivalent temperature, from 2700 to 6500 K
     */
    void set_temperature(int temperature);
    
    /** \fn void set_temperature(unsigned char Y, unsigned char W)
     *  \brief Sets the color temperature using CCT Y and W parameters.
     *  \param Y : CCT Y value
     *  \param W : CCT W value
     */
    void set_temperature(unsigned char Y, unsigned char W);
    
    /** \fn std::string get_bytes() const
     *  \brief Compiles a byte string to be sent as data to the device.
     *  \returns a byte string with color definitions.
     */
    std::string get_bytes() const;
    
  };
  
  
  /** \class TelinkScenario
   *  \brief Class representing a scenario.
   */
  class TelinkScenario {
  private:
    /** \property std::vector<TelinkColor> colors
     *  \brief List of scenario colors.
     */
    std::vector<TelinkColor> colors;
    
    /** \property std::vector<unsigned char> speeds
     *  \brief List of speeds associated with scenario colors.
     */
    std::vector<unsigned char> speeds;
    
  public:
    /** \fn TelinkScenario()
     *  \brief Object instantiation.
     */
    TelinkScenario() {}
    
    /** \fn void add_color(TelinkColor color, unsigned char speed)
     *  \brief Adds a color to the list.
     *  \param color : color object to add to the list
     *  \param speed : associated speed; speed is an integer between 0 and 15
     */
    void add_color(TelinkColor color, unsigned char speed);
    
    /** \fn void add_color(TelinkColor color)
     *  \brief Adds a color to the list.
     *  \param color : color object to add to the list
     */
    void add_color(TelinkColor color);
    
    /** \fn void replace_color(int color_index, TelinkColor color)
     *  \brief Replace a color in the list.
     *  \param color_index : index of the color to be replaced
     *  \param color : color object to replace with
     */
    void replace_color(int color_index, TelinkColor color);
    
    /** \fn void remove_color(int color_index)
     *  \brief Removes a color from the list.
     *  \param color_index : index of the color to be removed
     */
    void remove_color(int color_index);
    
    /** \fn void set_speed(int color_index, unsigned char speed)
     *  \brief Sets the speed for the given color; speed is an integer between 0 and 15.
     *  \param color_index : index of the color to set the speed for
     */
    void set_speed(int color_index, unsigned char speed);
    
    /** \fn int get_size()
     *  \brief Returns the size of the color vector.
     *  \returns the size of the color vector.
     */
    int get_size() const { return this->colors.size(); }
    
    /** \fn std::string get_bytes(int color_index) const
     *  \brief Compiles a byte string to be sent as data to the device.
     *  \param color_index : index of the color to get the string for
     *  \returns a byte string with color definitions.
     */
    std::string get_bytes(int color_index) const;
  };
  
  
  /** \class TelinkLight
   *  \brief Class handling a Bluetooth LE light with Telink mesh protocol.
   */
  class TelinkLight : public TelinkMesh {
  private:
    /** \property bool state
     *  \brief Light power state: true = on, false = off
     */
    bool state;
    
    /** \property unsigned char brightness
     *  \brief Light brightness from 0 to 100
     */
    unsigned char brightness;
    
    /** \property bool music_mode
     *  \brief If true, light is in music mode
     */
    bool music_mode = false;
    
    /** \fn void parse_command(const std::string & packet) override
     *  \brief Parses a command packet.
     *  \param packet : decrypted packet to be parsed.
     */
    void parse_command(const std::string & packet) override;
    
  public:
    /** \fn TelinkLight(const std::string address, const std::string name, const std::string password)
     *  \brief Object instantiation.
     *  \param address : device MAC address.
     *  \param name : device name.
     *  \param password : device password.
     */
    TelinkLight(const std::string address, const std::string name, const std::string password) : TelinkMesh(address, name, password) {}
    
    /** \fn void query_groups()
     *  \brief Queries mesh group IDs from device.
     */
    void query_groups();
    
    /** \fn void set_time()
     *  \brief Sets device date and time.
     */
    void set_time();
    
    /** \fn void query_time()
     *  \brief Queries device date and time.
     */
    void query_time();
    
    /** \fn void query_alarm()
     *  \brief Queries alarm status from device.
     */
    void query_alarm();
    
    /** \fn void query_device_info()
     *  \brief Queries device information.
     */
    void query_device_info();
    
    /** \fn void query_device_version()
     *  \brief Queries device firmware version.
     */
    void query_device_version();
    
    /** \fn void query_scenarios()
     *  \brief Queries scenarios from device.
     */
    void query_scenarios();
    
    /** \fn void query_status()
     *  \brief Queries device status.
     */
    void query_status();
    
    /** \fn void query_mesh_id()
     *  \brief Queries mesh ID from device.
     */
    void query_mesh_id();
    
    /** \fn void set_state(bool on_off)
     *  \brief Sets device power state.
     *  \param on_off : state to set (true = on, false = off)
     */
    void set_state(bool on_off);
    
    /** \fn void set_mesh_id(int mesh_id) override
     *  \brief Sets device mesh ID.
     *  \param mesh_id : mesh ID to set, from 1 to 254 for single device ID, and from 0x8000 to 0x80ff for group ID
     */
    void set_mesh_id(int mesh_id) override;
    
    /** \fn void add_group(unsigned char group_id)
     *  \brief Adds device to given group.
     *  \param group_id : ID of the group to add device to.
     */
    void add_group(unsigned char group_id);
    
    /** \fn void delete_group(unsigned char group_id)
     *  \brief Removes device from given group.
     *  \param group_id : ID of the group to remove device from.
     */
    void delete_group(unsigned char group_id);
   
    /** \fn void add_scenario(unsigned char scenario_id)
     *  \brief Adds given scenario to device.
     *  \param scenario_id : ID of the scenario to add to device.
     */
    void add_scenario(unsigned char scenario_id);
    
    /** \fn void delete_scenario(unsigned char scenario_id)
     *  \brief Deletes given scenario from device.
     *  \param scenario_id : ID of the scenario to remove from device.
     */
    void delete_scenario(unsigned char scenario_id);
    
    /** \fn void set_brightness(int brightness)
     *  \brief Sets light brightness.
     *  \param brightness : brithness value, from 0 to 100.
     */
    void set_brightness(int brightness);
    
    /** \fn void set_temperature(int temperature)
     *  \brief Sets light color temperature.
     *  \param temperature : temperature value, from 2700 to 6500 K.
     */
    void set_temperature(int temperature);
    
    /** \fn void set_color(unsigned char R, unsigned char G, unsigned char B)
     *  \brief Sets light RGB color.
     *  \param R : red component, from 0 to 255.
     *  \param G : green component, from 0 to 255.
     *  \param B : blue component, from 0 to 255.
     */
    void set_color(unsigned char R, unsigned char G, unsigned char B);
    
    /** \fn void set_music_mode(bool music_mode)
     *  \brief Sets device music mode: color/brightness changes are faster, but aren't acknowledged by replies.
     *  \param music_mode : state of music mode to set.
     */
    void set_music_mode(bool music_mode);
    
    /** \fn load_scenario(unsigned char scenario_id)
     *  \brief Loads the scenario with given scenario ID on device.
     *  \param scenario_id : scenario ID
     *  \param speed : scenario animation speed
     */
    void load_scenario(unsigned char scenario_id, unsigned char speed);
    
    /** \fn void set_alarm(unsigned char alarm_id, const std::vector<bool> & weekdays, unsigned char hour, unsigned char minute, unsigned char second, unsigned char action)
     *  \brief Sets an alarm with given parameters.
     *  \param alarm_id : index of the alarm
     *  \param weekdays : a list of 7 booleans representing which day the alarm is set on; starting index is Sunday
     *  \param hour : hour of the alarm
     *  \param minute : minute of the alarm
     *  \param second : second of the alarm
     *  \param action : what is done when the alarm triggers; 0 = switch off, 1 = switch on, >1 = start scenario with corresponding number
     */
    void set_alarm(unsigned char alarm_id, const std::vector<bool> & weekdays, unsigned char hour, unsigned char minute, unsigned char second, unsigned char action);
    
    /** \fn void set_alarm(unsigned char alarm_id, bool state)
     *  \brief Changes the state of an alarm.
     *  \param alarm_id : index of the alarm
     *  \param state : true to set the alarm on, false to set it off
     */
    void set_alarm(unsigned char alarm_id, bool state);
    
    /** \fn void delete_alarm(unsigned char alarm_id)
     *  \brief Deletes an alarm.
     *  \param alarm_id : index of the alarm
     */
    void delete_alarm(unsigned char alarm_id);
    
    /** \fn void edit_scenario(unsigned char scenario_id, const TelinkScenario & light_scenario)
     *  \brief Edits a light scenario. A light scenario is a series of colors that are cycled through.
     *  \param scenario_id : index of the scenario
     *  \param scenario : light scenario definition
     */
    void edit_scenario(unsigned char scenario_id, TelinkScenario & scenario);
    
    /** \fn virtual void parse_online_status_report(const std::string & packet)
     *  \brief Parses a command packet from an online status report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_online_status_report(const std::string & packet);
    
    /** \fn virtual void parse_status_report(const std::string & packet)
     *  \brief Parses a command packet from a device status report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_status_report(const std::string & packet);
    
    /** \fn virtual void parse_time_report(const std::string & packet)
     *  \brief Parses a command packet from a time report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_time_report(const std::string & packet);
    
    /** \fn virtual void parse_address_report(const std::string & packet)
     *  \brief Parses a command packet from an address report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_address_report(const std::string & packet);
    
    /** \fn virtual void parse_alarm_report(const std::string & packet)
     *  \brief Parses a command packet from an alarm report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_alarm_report(const std::string & packet);
    
    /** \fn virtual void parse_device_info_report(const std::string & packet)
     *  \brief Parses a command packet from a device info report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_device_info_report(const std::string & packet);
    
    /** \fn virtual void parse_group_id_report(const std::string & packet)
     *  \brief Parses a command packet from a group ID report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_group_id_report(const std::string & packet);
    
    /** \fn virtual void parse_scenario_report(const std::string & packet)
     *  \brief Parses a command packet from a scenario report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_scenario_report(const std::string & packet);
  };
}

#endif // __TELINK_LIGHT_H__
