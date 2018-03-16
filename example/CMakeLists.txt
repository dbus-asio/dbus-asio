cmake_minimum_required(VERSION 3.7.1)

find_package (Threads REQUIRED)
find_package (Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(dbus-test dbus-test.cpp)
add_executable(test-validation tests/validation.cpp)
add_executable(pummel-server tests/pummel-server.cpp)
add_executable(pummel-client tests/pummel-client.cpp)

set(COMMON_LINK_LIBRARIES dbus-asio ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

target_link_libraries (dbus-test LINK_PUBLIC ${COMMON_LINK_LIBRARIES})
target_link_libraries (test-validation LINK_PUBLIC ${COMMON_LINK_LIBRARIES})
target_link_libraries (pummel-server LINK_PUBLIC ${COMMON_LINK_LIBRARIES})
target_link_libraries (pummel-client LINK_PUBLIC ${COMMON_LINK_LIBRARIES})
