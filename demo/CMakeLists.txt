add_compile_definitions(SQUIRREL_KEYCOUNT=1)

add_executable(slab-demo
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        )

target_sources(slab-demo PUBLIC
        ${PROJECT_SOURCE_DIR}/shared/tinyusb/tinyusb_squirrel.c
        ${PROJECT_SOURCE_DIR}/shared/tinyusb/usb_descriptors.c
        ${PROJECT_SOURCE_DIR}/shared/tinyusb/tinyusb_openrgb.c
        ${PROJECT_SOURCE_DIR}/shared/communication/communication.c
        ${CMAKE_CURRENT_LIST_DIR}/keymap.c
        )

target_include_directories(slab-demo PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${PROJECT_SOURCE_DIR}/shared/tinyusb
        ${PROJECT_SOURCE_DIR}/shared/communication
        ${PROJECT_SOURCE_DIR}/shared
        )

# Pull in common dependencies
target_link_libraries(slab-demo
        pico_stdlib
        pico_unique_id
        tinyusb_device 
        tinyusb_board 
        pico_multicore 
        hardware_gpio 
        hardware_i2c 
        squirrel 
        pico_i2c_slave
        hardware_pio 
        pico_stdio_uart
        pico_flash 
        hardware_exception 
        hardware_flash
        )

target_compile_definitions(slab-demo PRIVATE
        PICO_DEFAULT_UART=0
        PICO_DEFAULT_UART_TX_PIN=12
        PICO_DEFAULT_UART_RX_PIN=13 )

# Enable UART output
pico_enable_stdio_usb(slab-demo 0)
pico_enable_stdio_uart(slab-demo 1)

# Create map/bin/hex file etc.
pico_add_extra_outputs(slab-demo)
