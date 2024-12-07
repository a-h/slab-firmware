add_executable(slab-right
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/keymap.c
        )

      target_include_directories(slab-right PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        )

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

target_compile_definitions(squirrel PUBLIC SQUIRREL_KEYCOUNT=30)

pico_enable_stdio_uart(slab-right 1)
pico_add_extra_outputs(slab-right)
