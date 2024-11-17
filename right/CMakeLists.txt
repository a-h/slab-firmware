add_compile_definitions(SQUIRREL_KEYCOUNT=30)

add_subdirectory(${PROJECT_SOURCE_DIR}/shared/buzzer buzzer) # Build the library.


add_executable(slab-right
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        )

add_subdirectory($ENV{SQUIRREL_PATH} squirrel)
target_include_directories(slab-right PUBLIC $ENV{SQUIRREL_PATH}/include)
add_subdirectory($ENV{SSD1306_PATH} ssd1306)
target_include_directories(slab-right PUBLIC $ENV{SSD1306_PATH}/include)
add_subdirectory($ENV{PCA9555_PATH} pca9555)
target_include_directories(slab-right PUBLIC $ENV{PCA9555_PATH}/include)

target_sources(slab-right PUBLIC
        ${PROJECT_SOURCE_DIR}/shared/tinyusb/tinyusb_squirrel.c
        ${PROJECT_SOURCE_DIR}/shared/tinyusb/usb_descriptors.c
        ${PROJECT_SOURCE_DIR}/shared/tinyusb/tinyusb_openrgb.c
        ${PROJECT_SOURCE_DIR}/shared/display/display.c
        ${PROJECT_SOURCE_DIR}/shared/rgbleds/rgbleds.c
        ${CMAKE_CURRENT_LIST_DIR}/keymap.c
        )

target_include_directories(slab-right PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${PROJECT_SOURCE_DIR}/shared/tinyusb
        ${PROJECT_SOURCE_DIR}/shared/rgbleds
        ${PROJECT_SOURCE_DIR}/shared/display
        ${PROJECT_SOURCE_DIR}/shared
        )

# Generate the PIO header files
pico_generate_pio_header(slab-right ${PROJECT_SOURCE_DIR}/shared/rgbleds/ws2812.pio)

# Pull in common dependencies
target_link_libraries(slab-right
        pico_pca9555 
        squirrel 
        pico-ssd1306
        buzzer

        pico_stdlib
        hardware_pwm
        pico_unique_id
        tinyusb_device 
        tinyusb_board 
        pico_multicore 
        hardware_gpio 
        hardware_i2c 
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
