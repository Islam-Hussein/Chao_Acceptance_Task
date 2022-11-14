
add_library(sensor_lib INTERFACE)

target_sources(sensor_lib INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/adxl345.c
        ${CMAKE_CURRENT_LIST_DIR}/pac193x.c
        ${CMAKE_CURRENT_LIST_DIR}/sht31.c
        ${CMAKE_CURRENT_LIST_DIR}/acceptance_task.c
        )


target_include_directories(sensor_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(sensor_lib INTERFACE pico_stdlib)