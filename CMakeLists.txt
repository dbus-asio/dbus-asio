cmake_minimum_required(VERSION 3.7.1)

include(GNUInstallDirs)

project(dbus-asio VERSION 1.0.0)

set(HEADER_FILES
	include/dbus_auth.h
	include/dbus.h
	include/dbus_introspectable.h
	include/dbus_log.h
	include/dbus_matchrule.h
	include/dbus_message.h
	include/dbus_messageprotocol.h
	include/dbus_messagestream.h
	include/dbus_native.h
	include/dbus_platform.h
	include/dbus_transport.h
	include/dbus_type_array.h
	include/dbus_type_base.h
	include/dbus_type_boolean.h
	include/dbus_type_byte.h
	include/dbus_type_dictentry.h
	include/dbus_type_double.h
	include/dbus_type.h
	include/dbus_type_helpers.h
	include/dbus_type_int16.h
	include/dbus_type_int32.h
	include/dbus_type_int64.h
	include/dbus_type_objectpath.h
	include/dbus_type_signature.h
	include/dbus_type_string.h
	include/dbus_type_struct.h
	include/dbus_type_uint16.h
	include/dbus_type_uint32.h
	include/dbus_type_uint64.h
	include/dbus_type_variant.h
	include/dbus_utils.h
	include/dbus_validation.h
)

add_library(dbus-asio SHARED
	src/dbus_auth.cpp
	src/dbus_introspectable.cpp
	src/dbus_log.cpp
	src/dbus_matchrule.cpp
	src/dbus_message.cpp
	src/dbus_message_error.cpp
	src/dbus_message_methodcall.cpp
	src/dbus_message_methodreturn.cpp
	src/dbus_messageprotocol.cpp
	src/dbus_message_signal.cpp
	src/dbus_native.cpp
	src/dbus_native_messages.cpp
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
find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
set (PRIVATE_LIBS ${Boost_LIBRARIES})
link_directories(${Boost_LIBRARY_DIRS})
target_link_libraries (dbus-asio ${PRIVATE_LIBS})

# PTHREAD DEPENDENCIES
find_package (Threads REQUIRED)
set (PRIVATE_LIBS "${PRIVATE_LIBS} ${CMAKE_THREAD_LIBS_INIT}")

set_target_properties(dbus-asio PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION 1
	PUBLIC_HEADER "${HEADER_FILES}")

foreach(LIB ${PRIVATE_LIBS})
	get_filename_component(BARE_LIB ${LIB} NAME)
	set(PLATFORM_LIBS "${PLATFORM_LIBS} -l${BARE_LIB}")
endforeach()

set(PLATFORM_LIBS "${PLATFORM_LIBS} -ldbus-asio")

message(${PLATFORM_LIBS})
configure_file(dbus-asio.pc.in dbus-asio.pc @ONLY)

target_include_directories(dbus-asio PUBLIC include)

add_subdirectory(example)

install(TARGETS dbus-asio
	LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include/libdbus-asio)

install(FILES ${CMAKE_BINARY_DIR}/dbus-asio.pc
	DESTINATION lib/pkgconfig)
