cmake_minimum_required(VERSION 2.8.12)

# broker
set(BROKER_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/broker_main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/broker.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/message.cpp"
)

add_executable(broker ${BROKER_SOURCES})

target_link_libraries(broker
    ${ZEROMQ_LIBRARIES}
    ${LOG4CXX_LIBRARIES}
    ${Boost_LIBRARIES}
)

install(TARGETS broker DESTINATION .)

# The torture tool
set(TORTURE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/torture_main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/message.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/torture.cpp"
)

add_executable(torture ${TORTURE_SOURCES})

target_link_libraries(torture
    ${ZEROMQ_LIBRARIES}
    ${LOG4CXX_LIBRARIES}
)

install(TARGETS torture DESTINATION .)
