add_executable(imu-test main.c)

target_link_libraries(imu-test PRIVATE Fusion)
