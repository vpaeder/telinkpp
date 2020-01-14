/** \file telink_mesh.cxx
 *  Class handling connection with a Bluetooth LE device with Telink mesh protocol.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#include <functional>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>

#include <boost/algorithm/string.hpp>

#include "telink_mesh.h"

namespace telink {
  
  #define ABORT() (fprintf(stderr, "%s\nAborting in %s at %s:%d\n", ERR_error_string(ERR_get_error(), NULL), __PRETTY_FUNCTION__, __FILE__, __LINE__), abort(), 0)

  /** \fn static std::string encrypt(std::string key, std::string data)
   *  \brief Encrypts a n x 16-byte data string with a 16-byte key, using AES encryption.
   *  \param key : 16-byte encryption key.
   *  \param data : n x 16-byte data string
   *  \returns the encrypted data string.
   */
  static std::string encrypt(std::string key, std::string data) {
    std::reverse(key.begin(), key.end());
    std::reverse(data.begin(), data.end());
  
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex (ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)key.c_str(), NULL);
    EVP_CIPHER_CTX_set_padding(ctx, false);
    unsigned char buffer[1024], *pointer = buffer;
    int outlen;
    EVP_EncryptUpdate (ctx, pointer, &outlen, (const unsigned char*)data.c_str(), data.length()) or ABORT();
    pointer += outlen;
    EVP_EncryptFinal_ex(ctx, pointer, &outlen) or ABORT();
    pointer += outlen;
    std::string result = std::string((char*)buffer, pointer-buffer);
    std::reverse(result.begin(), result.end());
    EVP_CIPHER_CTX_free(ctx);
    return result;
  }

   /** \fn static std::string decrypt(std::string key, std::string data)
    *  \brief Decrypts a n x 16-byte data string with a 16-byte key, using AES encryption.
    *  \param key : 16-byte encryption key.
    *  \param data : n x 16-byte data string
    *  \returns the decrypted data string.
    */
  static std::string decrypt(std::string key, std::string data) {
    std::reverse(key.begin(), key.end());
    std::reverse(data.begin(), data.end());
  
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex (ctx, EVP_aes_128_ecb(), NULL, (unsigned char*)key.c_str(), NULL);
    EVP_CIPHER_CTX_set_padding(ctx, false);
    unsigned char buffer[1024], *pointer = buffer;
    int outlen;
    EVP_DecryptUpdate (ctx, pointer, &outlen, (unsigned char*)data.c_str(), data.length()) or ABORT();
    pointer += outlen;
    EVP_DecryptFinal_ex(ctx, pointer, &outlen) or ABORT();
    pointer += outlen;
    std::string result = std::string((char*)buffer, pointer-buffer);
    std::reverse(result.begin(), result.end());
    EVP_CIPHER_CTX_free(ctx);
    return result;
  }

   /** \fn static std::vector<unsigned char> to_vector(const std::string & str)
    *  \brief Converts a string into an unsigned char vector.
    *  \param str : string to convert.
    *  \returns an unsigned char vector with input string as content.
    */
  static std::vector<unsigned char> to_vector(const std::string & str) {
    std::vector<unsigned char> out;
    for (auto & chr : str)
      out.push_back(chr);
    return out;
  }

   /** \fn static std::string from_vector(const std::vector<unsigned char> & vec)
    *  \brief Converts an unsigned char vector into a string.
    *  \param vec : unsigned char vector to convert.
    *  \returns a string with input char vector as content.
    */
  static std::string from_vector(const std::vector<unsigned char> & vec) {
    std::string out;
    for (auto & chr : vec)
      out.push_back(chr);
    return out;
  }

   /** \fn static void print_hex_string(const std::string desc, const std::string & str)
    *  \brief Prints out a string with characters represented in hexadecimal format.
    *  \param desc : description text.
    *  \param str : string to print out.
    */
  static void print_hex_string(const std::string desc, const std::string & str) {
    std::cout << desc << ": ";
    std::stringstream stream;
    for (auto & chr : str)
      stream << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)chr << ",";
    std::cout << stream.str().substr(0,stream.tellp()-1LL) << std::endl;
  }

  void TelinkMesh::notification_callback(BluetoothGattCharacteristic & c, std::vector<unsigned char> & data) {
    std::string data_string = from_vector(data);
    std::string decoded_string = this->decrypt_packet(data_string);
    
    #ifdef DEBUG
    print_hex_string("Received data", decoded_string);
    #endif
    
    // check that targetted vendor is correct
    if ((decoded_string[8] == (this->vendor & 0xff)) && (decoded_string[9] == (this->vendor >> 8)))
      this->parse_command(decoded_string);
  }


  TelinkMesh::TelinkMesh(const std::string address) {
    this->set_address(address);
  }

  TelinkMesh::TelinkMesh(const std::string address, const std::string name, const std::string password) {
    this->set_address(address);
    this->set_name(name);
    this->set_password(password);
  }

  void TelinkMesh::set_address(const std::string address) {
    if (this->ble_mesh != nullptr) {
      std::cerr << "Address change can only occur when disconnected." << std::endl;
      return;
    }
    
    this->address = address;
    this->reverse_address = "";
    std::vector<std::string> mac;
    boost::split(mac, address, []( char c ){ return c == ':'; });
    for (auto rit = mac.rbegin(); rit != mac.rend(); ++rit)
      this->reverse_address.push_back(std::stoul(*rit, nullptr, 16));
  
  }

  void TelinkMesh::set_name(const std::string name) {
    if (this->ble_mesh != nullptr)
      std::cerr << "Connection already established. Name change will apply only after reconnection." << std::endl;
    this->name = name;
    this->name.append(16-name.size(), 0);
  }

  void TelinkMesh::set_password(const std::string password) {
    if (this->ble_mesh != nullptr)
      std::cerr << "Connection already established. Password change will apply only after reconnection." << std::endl;
    this->password = password;
    this->password.append(16 - password.size(), 0);
  }

  void TelinkMesh::set_vendor(int vendor) {
    this->vendor = vendor & 0xffff;
  }

  void TelinkMesh::set_mesh_id(int mesh_id) {
    this->mesh_id = mesh_id & 0xffff;
  }

  std::string TelinkMesh::combine_name_and_password() const {
    std::string data;
    for (int i=0; i<16; i++)
      data.push_back(this->name[i] ^ this->password[i]);
    return data;
  }

  void TelinkMesh::generate_shared_key(const std::string & data1, const std::string & data2) {
    std::string key = this->combine_name_and_password();
    this->shared_key = encrypt(key, data1.substr(0,8) + data2.substr(0,8));
  }

  std::string TelinkMesh::key_encrypt(std::string & key) const {
    std::string data = combine_name_and_password();
    return encrypt(key, data);
  }

  std::string TelinkMesh::encrypt_packet(std::string & packet) const {
    std::string auth_nonce = this->reverse_address.substr(0,4) + '\1' + packet.substr(0,3) + '\x0f';
    auth_nonce.append(7,0);
    std::string authenticator = encrypt(this->shared_key, auth_nonce);
    for (int i=0; i<15; i++)
      authenticator[i] ^= packet[i+5];
  
    std::string mac = encrypt(this->shared_key, authenticator);
  
    for (int i=0; i<2; i++)
      packet[i+3] = mac[i];
  
    std::string iv = '\0' + this->reverse_address.substr(0,4) + '\1' + packet.substr(0,3);
    iv.append(7,0);
    std::string buffer = encrypt(this->shared_key, iv);
  
    for (int i=0; i<15; i++)
      packet[i+5] ^= buffer[i];
  
    return packet;
  }

  std::string TelinkMesh::decrypt_packet(std::string & packet) const {
    std::string iv = '\0' + this->reverse_address.substr(0,3) + packet.substr(0,5);
    iv.append(7,0);
    std::string result = encrypt(this->shared_key, iv);
    for (int i=0; i<packet.size()-7; i++)
      packet[i+7] ^= result[i];
  
    return packet;
  }

  std::string TelinkMesh::build_packet(int command, const std::string & data) {
    /* Telink mesh packets take the following form:
       bytes 0-1   : packet counter
       bytes 2-4   : not used (=0)
       bytes 5-6   : mesh ID
       bytes 7     : command code
       bytes 8-9   : vendor code
       bytes 10-20 : command data
    
      All multi-byte elements are in little-endian form.
      Packet counter runs between 1 and 0xffff.
    */
    std::string packet;
    packet.resize(20, 0);
    packet[0] = this->packet_count & 0xff;
    packet[1] = (this->packet_count++ >> 8) & 0xff;
    packet[5] = this->mesh_id & 0xff;
    packet[6] = (this->mesh_id >> 8) & 0xff;
    packet[7] = command & 0xff;
    packet[8] = this->vendor & 0xff;
    packet[9] = (this->vendor >> 8) & 0xff;
    for (int i=0; i<data.size(); i++)
      packet[i+10] = data[i];
  
    std::string enc_packet = this->encrypt_packet(packet);
  
    if (this->packet_count > 0xffff)
      this->packet_count = 1;
  
    return enc_packet;
  }

  bool TelinkMesh::connect() {
    if (this->ble_mesh != nullptr){
      std::cerr << "Error: mesh node with address " << this->address << " is already connected" << std::endl;
      return false;
    }
  
    /* access local Bluetooth peripheral */
    BluetoothManager * manager = nullptr;
    try {
        manager = BluetoothManager::get_bluetooth_manager();
    } catch(const std::runtime_error& e) {
        std::cerr << "Error while initializing libtinyb: " << e.what() << std::endl;
        manager = nullptr;
        return false;
    }

    /* start discovery of devices and search for target device */
    bool ret = manager->start_discovery();
    this->ble_mesh = manager->find<BluetoothDevice>(nullptr, &(this->address), nullptr, std::chrono::seconds(10));
    if (this->ble_mesh == nullptr) {
        std::cerr << "Device not found" << std::endl;
        ret = manager->stop_discovery();
        return false;
    }
  
    /* connect to device and get info service */
    this->ble_mesh->connect();
    std::unique_ptr<BluetoothGattService> info_service = this->ble_mesh->find(&uuid_info_service);
    ret = manager->stop_discovery(); // stop discovery (device found or timed out)
  
    /* get characteristics */
    this->notification_char = info_service->find(&uuid_notification_char);
    this->command_char = info_service->find(&uuid_command_char);
    this->pair_char = info_service->find(&uuid_pair_char);
  
    /* create public key */
    unsigned char buffer[8];
    int rc = RAND_bytes(buffer, 8);
    if(rc != 1) {
      unsigned long err = ERR_get_error();
      std::cerr << "Cannot generate random key. Error " << err << std::endl;
      return false;
    }
    std::string data = std::string((char*)buffer).substr(0,8);
    // 2nd part of key is encrypted with mesh name and password
    data.append(8,0);
    std::string enc_data = this->key_encrypt(data);
    std::string packet = '\x0c' + data.substr(0,8) + enc_data.substr(0,8);
  
    /* send public key to device and get response */
    this->pair_char->write_value(to_vector(packet));
    std::vector<unsigned char> response = this->pair_char->read_value();
    std::string response_string = from_vector(response);
  
    /* generate shared key */
    std::string data1 = data.substr(0,8), data2 = response_string.substr(1,9);
    this->generate_shared_key(data1, data2);
  
    /* set notification callback and enable notifications from device */
    using namespace std::placeholders;
    this->notification_char->enable_value_notifications(std::bind(&TelinkMesh::notification_callback, this, _1, _2), nullptr);
    this->notification_char->write_value({0x01});
  
    return true;
  }

  void TelinkMesh::disconnect() {
    if (this->ble_mesh != nullptr)
        this->ble_mesh->disconnect();
    this->ble_mesh = nullptr;
  }

  bool TelinkMesh::is_connected() {
    if (this->ble_mesh == nullptr) return false;
    return this->ble_mesh->get_connected();
  }
  
  void TelinkMesh::send_packet(int command, const std::string & data) {
    if (!this->is_connected()) {
      this->disconnect();
      this->connect();
      if (!this->is_connected()) {
        std::cerr << "Device with address " << this->address << " is disconnected and reconnection failed." << std::endl;
        return;
      }
    }
    std::string enc_packet = this->build_packet(command, data);
    this->command_char->write_value(to_vector(enc_packet));
  }
  
}
