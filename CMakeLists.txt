cmake_minimum_required(VERSION 3.7.1)

project(dbus-asio VERSION 1.0.0)

set(CMAKE_CXX_VISIBILITY_PRESET protected)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold")

set(HEADER_FILES
	src/dbus_auth.h
	src/dbus.h
	src/dbus_introspectable.h
	src/dbus_log.h
	src/dbus_matchrule.h
	src/dbus_message.h
	src/dbus_messageprotocol.h
	src/dbus_messageostream.h
	src/dbus_messageistream.h
	src/dbus_native.h
	src/dbus_octetbuffer.h
	src/dbus_platform.h
	src/dbus_transport.h
	src/dbus_type_array.h
	src/dbus_type_base.h
	src/dbus_type_boolean.h
	src/dbus_type_byte.h
	src/dbus_type_dictentry.h
	src/dbus_type_double.h
	src/dbus_type.h
	src/dbus_type_int16.h
	src/dbus_type_int32.h
	src/dbus_type_int64.h
	src/dbus_type_objectpath.h
	src/dbus_type_signature.h
	src/dbus_type_string.h
	src/dbus_type_struct.h
	src/dbus_type_uint16.h
	src/dbus_type_uint32.h
	src/dbus_type_uint64.h
	src/dbus_type_variant.h
	src/dbus_utils.h
	src/dbus_validation.h
)

add_library(dbus-asio
	src/dbus_auth.cpp
	src/dbus_introspectable.cpp
	src/dbus_log.cpp
	src/dbus_matchrule.cpp
	src/dbus_message.cpp
	src/dbus_message_error.cpp
	src/dbus_messageistream.cpp
	src/dbus_message_methodcall.cpp
	src/dbus_message_methodreturn.cpp
	src/dbus_messageprotocol.cpp
	src/dbus_message_signal.cpp
	src/dbus_native.cpp
	src/dbus_native_messages.cpp
	src/dbus_octetbuffer.cpp
	src/dbus_platform.cpp
	src/dbus_transport.cpp
	src/dbus_type_array.cpp
	src/dbus_type_base.cpp
	src/dbus_type_boolean.cpp
	src/dbus_type_byte.cpp
	src/dbus_type.cpp
	src/dbus_type_dictentry.cpp
	src/dbus_type_double.cpp
	src/dbus_type_int16.cpp
	src/dbus_type_int32.cpp
	src/dbus_type_int64.cpp
	src/dbus_type_objectpath.cpp
	src/dbus_type_signature.cpp
	src/dbus_type_string.cpp
	src/dbus_type_struct.cpp
	src/dbus_type_uint16.cpp
	src/dbus_type_uint32.cpp
	src/dbus_type_uint64.cpp
	src/dbus_type_variant.cpp
	src/dbus_utils.cpp
	src/dbus_validation.cpp
)

# DBUS DEPENDENCIES
find_package(Boost COMPONENTS chrono system thread REQUIRED)
target_include_directories(dbus-asio
	PUBLIC
		${Boost_INCLUDE_DIR}
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
		$<INSTALL_INTERFACE:include/dbus-asio>
)
target_link_directories(dbus-asio
	PUBLIC
		${Boost_LIBRARY_DIRS}
)

# PTHREAD DEPENDENCIES
find_package (Threads REQUIRED)

target_link_libraries (dbus-asio PUBLIC ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(dbus-asio PROPERTIES
	VERSION ${PROJECT_VERSION}
	PUBLIC_HEADER "${HEADER_FILES}")

if (${BUILD_SHARED_LIBS})
	set_target_properties(dbus-asio PROPERTIES
		SOVERSION 1
	)
endif ()

configure_file(dbus-asio.pc.in dbus-asio.pc @ONLY)

include(CTest)

option(ENABLE_EXAMPLES "Build example projects" OFF)
if (ENABLE_EXAMPLES)
	add_subdirectory(example)
endif(ENABLE_EXAMPLES)
if (BUILD_TESTING)
	add_subdirectory(tests)
endif ()

install(TARGETS dbus-asio
	EXPORT dbus-asioConfig
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION include/dbus-asio)

install(FILES ${CMAKE_BINARY_DIR}/dbus-asio.pc
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

install(EXPORT dbus-asioConfig DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dbus-asio/ NAMESPACE dbus-asio::)

include(GNUInstallDirs)
