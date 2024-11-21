add_library(display STATIC
display.c
)
target_include_directories(display PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
)
add_subdirectory($ENV{SSD1306_PATH} ssd1306)
target_include_directories(display PUBLIC $ENV{SSD1306_PATH}/include)
target_link_libraries(display PUBLIC
hardware_i2c
squirrel
pico-ssd1306
)
set_target_properties(display PROPERTIES PUBLIC_HEADER display.h)
