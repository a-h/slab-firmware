add_library(slider STATIC
slider.c
)
target_include_directories(slider PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(slider PUBLIC
hardware_i2c
pico_stdlib
)
set_target_properties(slider PROPERTIES PUBLIC_HEADER slider.h)
