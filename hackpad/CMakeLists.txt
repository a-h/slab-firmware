add_executable(slab-hackpad ${CMAKE_CURRENT_LIST_DIR}/main.c
                            ${CMAKE_CURRENT_LIST_DIR}/keymap.c)

target_include_directories(slab-hackpad PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(
  slab-hackpad
  pico_pca9555
  squirrel
  buzzer
  display
  slab_tinyusb
  communication
  rgbleds
  pico_stdlib
  pico_multicore
  pico_flash)

pico_enable_stdio_uart(slab-hackpad 1)
pico_add_extra_outputs(slab-hackpad)
