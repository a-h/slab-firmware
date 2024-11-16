add_compile_definitions(SQUIRREL_KEYCOUNT=30)

include_directories(${PROJECT_SOURCE_DIR}/external/squirrel/include) # Include SQUIRREL headers in target and dependencies.
add_subdirectory(${PROJECT_SOURCE_DIR}/external/squirrel squirrel) # Add SQUIRREL as a subdirectory. This will build the library.

include_directories(${PROJECT_SOURCE_DIR}/external/pico_pca9555/include) # Include PCA9555 headers in target and dependencies.
add_subdirectory(${PROJECT_SOURCE_DIR}/external/pico_pca9555 pico_pca9555) # Add PCA9555 as a subdirectory. This will build the library.

include_directories(${PROJECT_SOURCE_DIR}/external/pico-ssd1306) # Include SSD1306 headers in target and dependencies.

add_executable(slab-right
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        # The SSD1306 library does not have a cmake file, so we have to add the source files manually.
        ${PROJECT_SOURCE_DIR}/external/pico-ssd1306/ssd1306.c
        )

target_sources(slab-right PUBLIC
        ${PROJECT_SOURCE_DIR}/shared/tinyusb/tinyusb_squirrel.c
        ${PROJECT_SOURCE_DIR}/shared/tinyusb/usb_descriptors.c
        ${PROJECT_SOURCE_DIR}/shared/tinyusb/tinyusb_openrgb.c
        ${PROJECT_SOURCE_DIR}/shared/display/display.c
        ${PROJECT_SOURCE_DIR}/shared/buzzer/buzzer.c
        ${PROJECT_SOURCE_DIR}/shared/rgbleds/rgbleds.c
        ${CMAKE_CURRENT_LIST_DIR}/keymap.c
        )

target_include_directories(slab-right PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${PROJECT_SOURCE_DIR}/shared/tinyusb
        ${PROJECT_SOURCE_DIR}/shared/rgbleds
        ${PROJECT_SOURCE_DIR}/shared/buzzer
        ${PROJECT_SOURCE_DIR}/shared/display
        ${PROJECT_SOURCE_DIR}/shared
        )

# Generate the PIO header files
pico_generate_pio_header(slab-right ${PROJECT_SOURCE_DIR}/shared/pio/ws2812.pio)

# Pull in common dependencies
target_link_libraries(slab-right
        pico_stdlib
        hardware_pwm
        pico_unique_id
        tinyusb_device 
        tinyusb_board 
        pico_multicore 
        hardware_gpio 
        hardware_i2c 
        squirrel 
        pico_pca9555 
        pico_i2c_slave
        hardware_pio 
        pico_flash 
        hardware_exception 
        hardware_flash
        )

# Enable UART output
pico_enable_stdio_uart(slab-right 1)

# Create map/bin/hex file etc.
pico_add_extra_outputs(slab-right)
