set(SOURCES hc_main.c hc_mqtt.c)

add_executable(heat-controller ${SOURCES})

target_link_libraries(heat-controller ${PAHO_LIBRARIES})
