set(test_SOURCE
    "${CMAKE_CURRENT_LIST_DIR}/message_test.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../src/message.cpp"
)

set(test_BIN broker-unittests)

include_directories(
    ${GMOCK_INCLUDE_DIRS}
)

add_executable(${test_BIN} ${test_SOURCE})
target_link_libraries(${test_BIN}
   ${LOG4CXX_LIBRARIES}
   ${GMOCK_LIBRARIES}
   ${ZEROMQ_LIBRARIES}
)

ADD_CUSTOM_TARGET(check
    ALL
    "${EXECUTABLE_OUTPUT_PATH}/${test_BIN}"
    DEPENDS ${test_BIN}
    COMMENT "Executing unit tests..."
    VERBATIM
    SOURCES ${test_SRCS})
