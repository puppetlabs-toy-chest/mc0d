cmake_minimum_required(VERSION 2.8.12)
project(MCOLLECTIVE-ZEROMQ-BROKER)

set(VENDOR_DIRECTORY "${PROJECT_SOURCE_DIR}/vendor")
list(APPEND CMAKE_MODULE_PATH ${VENDOR_DIRECTORY})

find_package(ZEROMQ REQUIRED)
find_package(LOG4CXX REQUIRED)

include_directories("${VENDOR_DIRECTORY}/cppzmq")

# And compile our stuff
add_subdirectory(src)
