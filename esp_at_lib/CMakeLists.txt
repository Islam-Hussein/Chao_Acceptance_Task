
add_library(esp_at_lib INTERFACE)

target_sources(esp_at_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/esp_at.c
        ${CMAKE_CURRENT_LIST_DIR}/uart_to_esp.c
        )


target_include_directories(esp_at_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(esp_at_lib INTERFACE pico_stdlib hardware_uart)

################################################################################
# creates test_uart executable
add_executable(test_uart ${CMAKE_CURRENT_LIST_DIR}/test_uart.c)
target_include_directories(test_uart PUBLIC .)
# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(test_uart pico_stdlib pico_bootrom hardware_uart)

# enable usb output, disable uart output
pico_enable_stdio_usb(test_uart 1)
pico_enable_stdio_uart(test_uart 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(test_uart)
################################################################################
# creates test_esp_at executable
add_executable(test_esp_at ${CMAKE_CURRENT_LIST_DIR}/test_esp_at.c)
target_include_directories(test_esp_at PUBLIC .)
# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(test_esp_at pico_stdlib pico_bootrom esp_at_lib)

# enable usb output, disable uart output
pico_enable_stdio_usb(test_esp_at 1)
pico_enable_stdio_uart(test_esp_at 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(test_esp_at)