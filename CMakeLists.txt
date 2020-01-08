cmake_minimum_required(VERSION 3.0)

project(telink_light_cpp)

FIND_PACKAGE(OpenSSL REQUIRED) # for AES encryption/decryption

FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
	option(BUILD_DOC "Build documentation" ON)
ENDIF()

INCLUDE(GNUInstallDirs)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
set (CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib)

add_executable(telink_test telink_test.cxx telink_mesh.cxx telink_light.cxx)

include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR})
target_link_libraries(telink_test tinyb ${OPENSSL_CRYPTO_LIBRARIES})
link_libraries(tinyb)

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
