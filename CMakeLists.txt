# Minimum CMake version required to generate
# our build system
cmake_minimum_required(VERSION 3.0)

# Name of our Project
project(telink_light_cpp)

FIND_PACKAGE(OpenSSL REQUIRED)

INCLUDE(GNUInstallDirs)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++14)
set (CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib)
# add_executable creates an executable with
# The given name.
add_executable(telink_test telink_test.cxx telink_mesh.cxx telink_light.cxx)

# Make sure the executable links to the wiringPi lib
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR})
target_link_libraries(telink_test tinyb ${OPENSSL_CRYPTO_LIBRARIES})
