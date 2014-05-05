cmake_minimum_required(VERSION 2.8.12)
project(MCOLLECTIVE-ZEROMQ-BROKER)

set(VENDOR_DIRECTORY "${PROJECT_SOURCE_DIR}/vendor")
list(APPEND CMAKE_MODULE_PATH ${VENDOR_DIRECTORY})

find_package(ZEROMQ REQUIRED)
find_package(LOG4CXX REQUIRED)
find_package(Boost 1.48 COMPONENTS program_options REQUIRED)

# Include vendored library
include(${VENDOR_DIRECTORY}/gmock.cmake)

# Set include directories
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${ZEROMQ_INCLUDE_DIR}
    ${LOG4CXX_INCLUDE_DIR}
    "${VENDOR_DIRECTORY}/cppzmq"
)

# Add the main binary
add_subdirectory(src)

