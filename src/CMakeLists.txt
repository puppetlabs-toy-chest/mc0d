cmake_minimum_required(VERSION 2.8.12)

# Set compiler-specific flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-tautological-constant-out-of-range-compare")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-local-typedefs")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
endif()

# Set include directories
include_directories(
    ${ZEROMQ_INCLUDE_DIR}
)


# broker
set(BROKER_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/broker_main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/broker.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/message.cpp"
)

add_executable(broker ${BROKER_SOURCES})

target_link_libraries(broker
    ${ZEROMQ_LIBRARIES}
)

install(TARGETS broker DESTINATION .)

# The torture tool
set(TORTURE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/torture_main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/message.cpp"
)

add_executable(torture ${TORTURE_SOURCES})

target_link_libraries(torture
    ${ZEROMQ_LIBRARIES}
)

install(TARGETS torture DESTINATION .)


