# telink_light_cpp
C++ classes to handle a Bluetooth LE light with Telink mesh protocol. Such lights can be found for instance as cheap connected light bulbs from Lidl (Livarno Lux brand), or from Briloner Gmbh.

# Requirements
- Intel TinyB - https://github.com/intel-iot-devkit/tinyb
- OpenSSL - https://www.openssl.org
- CMake (for compilation, optional) - https://cmake.org
- Doxygen (for docs, optional) - http://doxygen.nl

# Compilation
Make sure to change MAC address, device name and password in telink_test.cxx before compiling.
This can be done easily with CMake using the provided CMakeLists.txt file.
On command line, in source folder, type:
```
 $ mkdir build && cd build
 $ cmake ..
 $ make
```

# Usage of C++ classes
At the moment I didn't write a compilation script to produce a library. Therefore one must take care of including the header and the cxx file in one's own source tree.
For details on class methods and features, see documentation in files (one can use doxygen to produce documentation files, if wished).

# Usage of example program
` $ sudo ./telink_test`
