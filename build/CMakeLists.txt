cmake_minimum_required(VERSION 3.0)
project(mage C)

set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE src ../src/*.c src/*.h)
file(GLOB_RECURSE res ../res/*.*)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

add_executable(mage ${src} ${res})

target_include_directories(mage PUBLIC ../src)
target_include_directories(mage PUBLIC /usr/local/include/)

target_link_libraries(mage PUBLIC SDL2 SDL2_image)

install(TARGETS mage DESTINATION mage_destination)
install(DIRECTORY res DESTINATION mage_destination)

set(CPACK_PACKAGE_NAME "mage editor")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)
