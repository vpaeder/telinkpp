# Summary
This project provides C++ classes to handle a Bluetooth LE lighting with Telink mesh protocol. A Python wrapper is also provided. Telink chips can be found for instance in cheap connected light bulbs from Lidl (Livarno LUX brand), from Briloner Leuchten GmbH, or C by GE connected lighting.

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
- Python 2.x or 3.x (for Python wrapper, optional) - https://python.org
- Boost Python (for Python wrapper, optional) - https://boost.org
- Doxygen (for docs, optional) - http://doxygen.nl

Note that you need a system on which TinyB works. In practice this means some blend of Linux with the BlueZ stack. I tested it on Raspbian Buster with BlueZ 5.50.

# Compiling and installing
This can be done easily with CMake using the provided CMakeLists.txt file.
On command line, in source folder, type:
```
 $ mkdir build && cd build
 $ cmake ..
 $ make
 $ sudo make install
```
To build documentation (doxygen required), add `-DBUILD_DOC=1` option to cmake.
To build the Python wrapper, add `-DBUILD_PYTHON_WRAPPER=1`. Add `-DBUILD_FOR_PYTHON_3=1` to build for Python 3 instead of 2.

# Usage of C++ classes
For details on class methods and features, see documentation in doc folder.

When building a project that requires `telinkpp`, remember to add it to the list of dependencies. Header files are installed in the subdirectory `telinkpp` of the *include* folder.

# Usage of example program
` $ sudo ./telink_test <device_MAC_address> <device_name> <device_password>`

Device MAC address can be found by scanning for Bluetooth devices. Device name and password depend on brand and model. Factory defaults proposed by Telink are *telink_mesh1* and *123*, but will have likely been changed by the manufacturer of your device to something else.

##### Finding the MAC address
On the command line, this can be done with:
` $ sudo ./bluetoothctl`
It opens the bluetooth control utility from BlueZ in interactive mode. Type `scan on` and wait that the device appears in the discovery report.

##### Finding device name and password
This can be a little longer. For the device I wanted to control, I had to use Android Studio with the smalidea plugin to debug the Briloner Control app. This involved the following steps:
1. Download and install Android Studio from https://developer.android.com/studio
2. Download and install the smalidea plugin - https://github.com/JesusFreke/smali
3. Install VirtualBox - https://www.virtualbox.org
4. Install Android-x86 on VirtualBox - https://www.android-x86.org
5. In the virtual machine configuration, under *Network*, configure a *Host-only adapter*
6. In the virtual machine configuration, under *Ports*, *USB*, add the Bluetooth controller
7. Enable Developer mode on Android
8. Use `adb connect <virtual_machine_IP_address>` to link computer and virtual machine together
9. Set root mode with `adb root`
10. Download app package in APK format
11. Decompile the app with JADX - this can be done online on http://www.javadecompilers.com
12. Install the app on the virtual machine with `adb install your_app_file.apk`
13. In Android developer settings, select app in *Select debug app* and enable *Wait for debugger*
14. Create a project in Android Studio with the decompiled app - tutorial here: https://malacupa.com/2018/11/11/debug-decompiled-smali-code-in-android-studio-3.2.html
15. Find the appropriate *login* method within the code, and place a breakpoint there.
16. Start app on Android.
17. Start debugger in Android Studio and attach with the right process (for me: cn.telink.blelight )
18. When the breakpoint triggers, check variables for device name and password

One can do the same with a real phone, in which case steps 3-6 are not necessary.

# Usage of Python wrapper
The Python wrapper gives access to public methods of all C++ classes. To build it, invoke cmake with option `-DBUILD_PYTHON_WRAPPER=1`. It will build by default with Python 2 (tested to work on Raspbian Buster). To build with Python 3, add cmake option `-DBUILD_FOR_PYTHON_3=1` (not tested).

For TelinkLight, Python callbacks can be set by creating a derived class. The following example shows how to overload `parse_online_status_report` in Python:
```
from pytelink import TelinkLight

class MyLight(TelinkLight):
    def parse_online_status_report(self, packet):
        pass # do something here with packet content

ml = MyLight("AA:BB:CC:DD:EE:FF", "DeviceName", "Password")
ml.connect()
ml.set_state(True) # turn light on
```
In this example, `parse_online_status_report` is triggered after `connect()` and after `set_state(...)`. Similar overloads can be written for every callback. See documentation in telink_python.h for details.
