cmake_minimum_required(VERSION 3.7.1)
add_executable(dbus_asio_test dbus_asio_test.cpp)
target_link_libraries(dbus_asio_test LINK_PRIVATE dbus-asio)
add_test(NAME run_dbus_asio_test COMMAND dbus_asio_test)
