add_library(slab_tinyusb STATIC tinyusb_squirrel.c usb_descriptors.c
                                tinyusb_openrgb.c)
target_include_directories(slab_tinyusb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(slab_tinyusb PUBLIC tinyusb_device tinyusb_board
                                          pico_stdlib squirrel)
set_target_properties(slab_tinyusb PROPERTIES PUBLIC_HEADER tinyusb_squirrel.h)
