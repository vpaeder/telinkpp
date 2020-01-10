# Summary
This project provides C++ classes to handle a Bluetooth LE lighting with Telink mesh protocol. Telink chips can be found for instance in cheap connected light bulbs from Lidl (Livarno LUX brand), from Briloner Leuchten GmbH, or C by GE connected lighting.

The command set is derived from Telink demo apps together with an analysis of the Briloner Control phone app. Since the app makes use of a subset of the available features and Telink's documentation on the subject is between absconse and inexistant, it is probable that some features will never be implemented.

Parts of the key exchange and packet encryption/decryption routines are inspired from https://github.com/google/python-dimond .

##### Implemented features
 * power on/off
 * change brightness
 * change color / temperature
 * set alarms
 * edit custom scenarios
 * select scenarios
 * device groups (use mesh ID 0x8000 + group ID to communicate with defined groups)

##### Not implemented
 * device reset
 * OTA operations

Device replies are treated minimally to demonstrate what data packets contain. Queries are treated asynchronously.

# Requirements
- Intel TinyB - https://github.com/intel-iot-devkit/tinyb
- OpenSSL - https://www.openssl.org
- CMake (for compilation, optional) - https://cmake.org
- Doxygen (for docs, optional) - http://doxygen.nl

Note that you need a system on which TinyB works. In practice this means some blend of Linux with the BlueZ stack. I tested it on Raspbian.

# Compiling
This can be done easily with CMake using the provided CMakeLists.txt file.
On command line, in source folder, type:
```
 $ mkdir build && cd build
 $ cmake ..
 $ make
```

# Usage of C++ classes
At the moment I didn't write a compilation script to produce a library. Therefore one must take care of including the header and the cxx file in one's own source tree.
For details on class methods and features, see documentation in doc folder.

# Usage of example program
` $ sudo ./telink_test <device_MAC_address> <device_name> <device_password>`

Device MAC address can be found by scanning for Bluetooth devices. Device name and password depend on brand and model. Factory defaults proposed by Telink are *telink_mesh1* and *123*.
