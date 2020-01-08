/** \file telink_light.h
 *  Class handling a Bluetooth LE light with Telink mesh protocol.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#include "telink_mesh.h"

namespace telink {
  
  #define COMMAND_SCENE_QUERY           0xC0
  #define COMMAND_SCENE_REPORT          0xC1
  #define COMMAND_GROUP_ID_REPORT       0xD4
  #define COMMAND_GROUP_EDIT            0xD7
  #define COMMAND_STATUS_QUERY          0xDA
  #define COMMAND_STATUS_REPORT         0xDB
  #define COMMAND_ONLINE_STATUS_REPORT  0xDC
  #define COMMAND_GROUP_ID_QUERY        0xDD
  #define COMMAND_ADDRESS_EDIT          0xE0
  #define COMMAND_ADDRESS_REPORT        0xE1
  #define COMMAND_RESET                 0xE3
  #define COMMAND_TIME_SET              0xE4
  #define COMMAND_ALARM_EDIT            0xE5
  #define COMMAND_ALARM_QUERY           0xE6
  #define COMMAND_ALARM_REPORT          0xE7
  #define COMMAND_TIME_QUERY            0xE8
  #define COMMAND_TIME_REPORT           0xE9
  #define COMMAND_DEVICE_INFO_QUERY     0xEA
  #define COMMAND_DEVICE_INFO_REPORT    0xEB
  #define COMMAND_SCENE_EDIT            0xEE
  #define COMMAND_SCENE_LOAD            0xEF
  #define COMMAND_LIGHT_ON_OFF          0xF0
  #define COMMAND_LIGHT_ATTRIBUTES      0xF1
  #define COMMAND_LIGHT_MODE_LOAD       0xF2
  #define COMMAND_LIGHT_MODE_EDIT       0xF3
  
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
    
    /** \fn void query_group_id()
     *  \brief Queries mesh group ID from device.
     */
    void query_group_id();
    
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
    
    /** \fn void query_scene()
     *  \brief Queries scene from device.
     */
    void query_scene();
    
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
     *  \param mesh_id : mesh ID to set
     */
    void set_mesh_id(int mesh_id) override;
    
    /** \fn void add_group(unsigned char group_id)
     *  \brief Adds device to given group.
     *  \param group_id : ID of the group to add device to.
     */
    void add_group(unsigned char group_id);
    
    /** \fn void add_scene(unsigned char scene_id)
     *  \brief Adds device to given scene.
     *  \param scene_id : ID of the scene to add device to.
     */
    void add_scene(unsigned char scene_id);
    
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
    
    /** \fn load_mode(unsigned char mode_id)
     *  \brief Loads the mode with given mode ID on device.
     *  \param mode_id : mode ID
     *  \param speed : mode animation speed
     */
    void load_mode(unsigned char mode_id, unsigned char speed);
  };
}
