cmake_minimum_required(VERSION 2.8.12)

# broker
set(BROKER_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/mc0d_main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/broker.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/message.cpp"
)

add_executable(mc0d ${BROKER_SOURCES})

target_link_libraries(mc0d
    ${ZEROMQ_LIBRARIES}
    ${LOG4CXX_LIBRARIES}
    ${Boost_LIBRARIES}
)

install(TARGETS mc0d DESTINATION .)

# The torture tool
set(TORTURE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/mc0torture_main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/message.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/torture.cpp"
)

add_executable(mc0torture ${TORTURE_SOURCES})

target_link_libraries(mc0torture
    ${ZEROMQ_LIBRARIES}
    ${LOG4CXX_LIBRARIES}
)

install(TARGETS mc0torture DESTINATION .)
