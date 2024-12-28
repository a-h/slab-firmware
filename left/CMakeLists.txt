add_executable(slab-left ${CMAKE_CURRENT_LIST_DIR}/main.c
                         ${CMAKE_CURRENT_LIST_DIR}/keymap.c)

target_include_directories(slab-left PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(
  slab-left
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
  pico_flash)

pico_enable_stdio_uart(slab-left 1)
pico_add_extra_outputs(slab-left)
