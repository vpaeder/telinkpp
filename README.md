# telink_light_cpp
C++ classes to handle a Bluetooth LE light with Telink mesh protocol. Telink chips can be found for instance in cheap connected light bulbs from Lidl (Livarno LUX brand), or from Briloner Leuchten GmbH.

The command set is derived from Telink demo apps together with an analysis of the Briloner Control phone app. Since the app makes use of only a small subset of the available features and Telink's documentation on the subject is between absconse and inexistant, it is probable that some potential features will never be implemented. For now, basic light control (power on/off, change brightness, temperature, color) is working fine.

Parts of the key exchange and packet encryption/decryption routines are inspired from https://github.com/google/python-dimond .

# Requirements
- Intel TinyB - https://github.com/intel-iot-devkit/tinyb
- OpenSSL - https://www.openssl.org
- CMake (for compilation, optional) - https://cmake.org
- Doxygen (for docs, optional) - http://doxygen.nl

Note that you need a system on which TinyB works. In practice this means some blend of Linux with the BlueZ stack. I tested it on Raspbian.

# Compiling
Make sure to change MAC address, device name and password in telink_test.cxx before compiling.

Compiling can be done easily with CMake using the provided CMakeLists.txt file.
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
` $ sudo ./telink_test`
