add_compile_definitions(SQUIRREL_KEYCOUNT=30)

#add_subdirectory(${PROJECT_SOURCE_DIR}/shared/tinyusb tinyusb-right)
#add_subdirectory(${PROJECT_SOURCE_DIR}/shared/buzzer buzzer-right)
#add_subdirectory(${PROJECT_SOURCE_DIR}/shared/slider slider-right)
#add_subdirectory(${PROJECT_SOURCE_DIR}/shared/display display-right)
#add_subdirectory(${PROJECT_SOURCE_DIR}/shared/rgbleds rgbleds-right)
#add_subdirectory(${PROJECT_SOURCE_DIR}/shared/communication communication-right)

add_executable(slab-right
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        )

#add_subdirectory($ENV{SQUIRREL_PATH} squirrel-right)
target_include_directories(slab-right PUBLIC $ENV{SQUIRREL_PATH}/include)
#add_subdirectory($ENV{PCA9555_PATH} pca9555-right)
target_include_directories(slab-right PUBLIC $ENV{PCA9555_PATH}/include)

target_sources(slab-right PUBLIC
        ${PROJECT_SOURCE_DIR}/shared/rgbleds/rgbleds.c
        ${CMAKE_CURRENT_LIST_DIR}/keymap.c
        )

target_include_directories(slab-right PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${PROJECT_SOURCE_DIR}/shared/rgbleds
        ${PROJECT_SOURCE_DIR}/shared
        )

# Pull in common dependencies
target_link_libraries(slab-right
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
pico_enable_stdio_uart(slab-right 1)

# Create map/bin/hex file etc.
pico_add_extra_outputs(slab-right)
