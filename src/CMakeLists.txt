set(SOURCES arrlist.c hc_json.c hc_main.c hc_mqtt.c)

add_executable(heat-controller ${SOURCES})

target_link_libraries(heat-controller
                      ${CJSON_LIBRARIES}
                      ${PAHO_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT})
