/** \file telink_mesh.h
 *  Class handling connection with a Bluetooth LE device with Telink mesh protocol.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#ifndef __TELINK_MESH_H__
#define __TELINK_MESH_H__

#include <string>
#include <vector>
#include <tinyb.hpp>

namespace telink {
  
  /** \brief UUID for Bluetooth GATT information service */
  static std::string uuid_info_service = "00010203-0405-0607-0809-0a0b0c0d1910";
  /** \brief UUID for Bluetooth GATT notification characteristic */
  static std::string uuid_notification_char = "00010203-0405-0607-0809-0a0b0c0d1911";
  /** \brief UUID for Bluetooth GATT command characteristic */
  static std::string uuid_command_char = "00010203-0405-0607-0809-0a0b0c0d1912";
  /** \brief UUID for Bluetooth GATT pairing characteristic */
  static std::string uuid_pair_char = "00010203-0405-0607-0809-0a0b0c0d1914";

  /** \class TelinkMesh
   *  \brief Class handling connection with a Bluetooth LE device with Telink mesh protocol.
   */
  class TelinkMesh {
  private:
    /** \property std::string address
     *  \brief MAC address of Bluetooth device.
     */
    std::string address;
    
    /** \property std::string reverse_address
     *  \brief MAC address formatted for little-endianness.
     */
    std::string reverse_address;
    
    /** \property std::string name
     *  \brief Device name.
     */
    std::string name;
    
    /** \property std::string password
     *  \brief Device password. Used to generate shared key.
     */
    std::string password;
    
    /** \property std::string shared_key
     *  \brief Shared key used to encrypt communication with device.
     */
    std::string shared_key;
    
    /** \property int vendor
     *  \brief Bluetooth vendor code.
     */
    int vendor = 0x211;
    
    /** \property int packet_count
     *  \brief Packet counter used to tag transmitted packets.
     */
    int packet_count = 1;
  
    /** \property std::unique_ptr<BluetoothDevice> ble_mesh
     *  \brief TinyB Bluetooth device object.
     */
    std::unique_ptr<BluetoothDevice> ble_mesh;
    
    /** \property std::unique_ptr<BluetoothGattCharacteristic> notification_char
     *  \brief TinyB object for notification Bluetooth GATT characteristic.
     */
    std::unique_ptr<BluetoothGattCharacteristic> notification_char;
    
    /** \property std::unique_ptr<BluetoothGattCharacteristic> command_char
     *  \brief TinyB object for command Bluetooth GATT characteristic.
     */
    std::unique_ptr<BluetoothGattCharacteristic> command_char;
    
    /** \property std::unique_ptr<BluetoothGattCharacteristic> pair_char
     *  \brief TinyB object for pairing Bluetooth GATT characteristic.
     */
    std::unique_ptr<BluetoothGattCharacteristic> pair_char;
  
    /** \fn std::string combine_name_and_password()
     *  \brief Combines the device name and password for use with shared key generation.
     *  \returns a string containing combined device name and password.
     */
    std::string combine_name_and_password() const;
    
    /** \fn void generate_shared_key(const std::string & data1, const std::string & data2)
     *  \brief Generates a shared key from given data. Result is stored in member variable shared_key.
     *  \param data1 : 8-byte string.
     *  \param data2 : another 8-byte string.
     */
    void generate_shared_key(const std::string & data1, const std::string & data2);
    
    /** \fn std::string key_encrypt(std::string & key) const
     *  \brief Encrypts device name/password combination with given key.
     *  \param key : 16-byte string used as encryption key.
     *  \returns a 16-byte string containing the encrypted name/password combination.
     */
    std::string key_encrypt(std::string & key) const;
    
    /** \fn std::string encrypt_packet(std::string & packet) const
     *  \brief Encrypts given packet with stored shared key.
     *  \param packet : 20-byte packet to encrypt.
     *  \returns the encrypted 20-byte packet.
     */
    std::string encrypt_packet(std::string & packet) const;
    
    /** \fn std::string decrypt_packet(std::string & packet) const
     *  \brief Decrypts given packet with stored shared key.
     *  \param packet : 20-byte packet to decrypt.
     *  \returns the decrypted 20-byte packet.
     */
    std::string decrypt_packet(std::string & packet) const;
    
    /** \fn std::string build_packet(int command, const std::string & data)
     *  \brief Builds a command packet to be sent to the device.
     *  \param command : command code.
     *  \param data : command parameters (up to 10 byte).
     *  \returns the encrypted generated packet.
     */
    std::string build_packet(int command, const std::string & data);
  
    /** \fn virtual void parse_command(const std::string & packet)
     *  \brief Parses a command packet.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_command(const std::string & packet) {}
    
    /** \fn void notification_callback(BluetoothGattCharacteristic & c, std::vector<unsigned char> & data, void * userdata)
     *  \brief Callback for notification Bluetooth GATT characteristic.
     *  \param c : GATT characteristic that received data.
     *  \param data : received data.
     */
    void notification_callback(BluetoothGattCharacteristic & c, std::vector<unsigned char> & data);
  
  protected:
     /** \property int mesh_id
      *  \brief Device ID.
      */
    int mesh_id = 0;
  
  public:
    /** \fn TelinkMesh(const std::string address)
     *  \brief Object instantiation.
     *  \param address : device MAC address.
     */
    TelinkMesh(const std::string address);
    /** \fn TelinkMesh(const std::string address, const std::string name, const std::string password)
     *  \brief Object instantiation.
     *  \param address : device MAC address.
     *  \param name : device name.
     *  \param password : device password.
     */
    TelinkMesh(const std::string address, const std::string name, const std::string password);
  
    /** \fn void set_address(const std::string address)
     *  \brief Sets the MAC address to connect to.
     *  \param address : MAC address in the form AA:BB:CC:DD:EE:FF.
     */
    void set_address(const std::string address);
    
    /** \fn void set_name(const std::string name)
     *  \brief Sets the device name to be used for connecting.
     *  \param name : device name.
     */
    void set_name(const std::string name);
    
    /** \fn void set_password(const std::string password)
     *  \brief Sets the password to be used for connecting.
     *  \param password : password.
     */
    void set_password(const std::string password);
    
    /** \fn void set_vendor(int vendor)
     *  \brief Sets the Bluetooth vendor code (0x0211 for Telink).
     *  \param vendor : vendor code.
     */
    void set_vendor(int vendor);
    
    /** \fn virtual void set_mesh_id(int mesh_id)
     *  \brief Sets the device ID on the mesh.
     *  \param mesh_id : mesh ID, from 1 to 254.
     */
    virtual void set_mesh_id(int mesh_id);
  
    /** \fn void send_packet(int command, const std::string & data)
     *  \brief Sends a command packet to the device.
     *  \param command : command code.
     *  \param data : command parameters (up to 10 byte).
     */
    void send_packet(int command, const std::string & data);
  
    /** \fn bool connect()
     *  \brief Connects to Bluetooth device.
     *  \returns true if connection succeeded, false otherwise.
     */
    bool connect();
    
    /** \fn void disconnect()
     *  \brief Disconnects from Bluetooth device.
     */
    void disconnect();
  };
  
}

#endif // __TELINK_MESH_H__
