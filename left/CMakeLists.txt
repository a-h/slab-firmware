add_compile_definitions(SQUIRREL_KEYCOUNT=30)

add_subdirectory(${PROJECT_SOURCE_DIR}/shared/tinyusb tinyusb-left)
add_subdirectory(${PROJECT_SOURCE_DIR}/shared/buzzer buzzer-left)
add_subdirectory(${PROJECT_SOURCE_DIR}/shared/slider slider-left)
add_subdirectory(${PROJECT_SOURCE_DIR}/shared/display display-left)
add_subdirectory(${PROJECT_SOURCE_DIR}/shared/rgbleds rgbleds-left)
add_subdirectory(${PROJECT_SOURCE_DIR}/shared/communication communication-left)

add_executable(slab-left
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        )

add_subdirectory($ENV{SQUIRREL_PATH} squirrel-left)
target_include_directories(slab-left PUBLIC $ENV{SQUIRREL_PATH}/include)
add_subdirectory($ENV{PCA9555_PATH} pca9555-left)
target_include_directories(slab-left PUBLIC $ENV{PCA9555_PATH}/include)

target_sources(slab-left PUBLIC
        ${PROJECT_SOURCE_DIR}/shared/rgbleds/rgbleds.c
        ${CMAKE_CURRENT_LIST_DIR}/keymap.c
        )

target_include_directories(slab-left PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${PROJECT_SOURCE_DIR}/shared/rgbleds
        ${PROJECT_SOURCE_DIR}/shared
        )

# Pull in common dependencies
target_link_libraries(slab-left
        pico_pca9555 
        squirrel 
        slider
        buzzer
        display
        slab_tinyusb
        communication
        rgbleds

        pico_stdlib
        pico_multicore 
        pico_flash 
        )

# Enable UART output
pico_enable_stdio_uart(slab-left 1)

# Create map/bin/hex file etc.
pico_add_extra_outputs(slab-left)
