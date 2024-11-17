add_library(buzzer STATIC
buzzer.c
)
target_include_directories(buzzer PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(buzzer PUBLIC
hardware_clocks
hardware_pwm
pico_stdlib
pico_time
)
set_target_properties(buzzer PROPERTIES PUBLIC_HEADER buzzer.h)
