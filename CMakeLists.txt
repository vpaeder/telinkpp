cmake_minimum_required(VERSION 3.0)

project(telink_light_cpp)

option(BUILD_PYTHON_WRAPPER "Build Python wrapper" ON)

FIND_PACKAGE(OpenSSL REQUIRED) # for AES encryption/decryption

FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
	option(BUILD_DOC "Build documentation" ON)
ENDIF()

INCLUDE(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(TINYB_LIBRARIES NAMES "tinyb" REQUIRED)
find_path(TINYB_INCLUDE_DIRS NAMES "tinyb.hpp" PATHS "/usr/include /usr/local/include /opt/local/include")

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -DDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib")

add_library ( telink_light_o STATIC telink_mesh.cxx telink_light.cxx )
target_include_directories( telink_light_o PUBLIC ${TINYB_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(telink_light_o ${TINYB_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARIES})

add_executable(telink_test telink_test.cxx)

target_include_directories(telink_test PUBLIC ${TINYB_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(telink_test telink_light_o ${TINYB_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARIES})

IF (BUILD_PYTHON_WRAPPER)
  add_subdirectory(pytelink)
ENDIF()

# Documentation
IF (DOXYGEN_FOUND AND BUILD_DOC)
  set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
	add_custom_target( doc_doxygen ALL
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM )
		
		install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html DESTINATION share/doc/telink_light )
ENDIF()
