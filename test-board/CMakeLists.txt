add_executable(slab-test-board ${CMAKE_CURRENT_LIST_DIR}/main.c
                               ${CMAKE_CURRENT_LIST_DIR}/keymap.c)

target_include_directories(slab-test-board PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(
  slab-test-board
  squirrel
  slab_tinyusb
  communication
  pico_stdlib
  pico_multicore
  hardware_pio
  pico_flash)

pico_enable_stdio_uart(slab-test-board 1)
pico_add_extra_outputs(slab-test-board)
