cmake_minimum_required(VERSION 3.23)
project(dxatc VERSION 0.1)

option(DXATC_BUILD_TESTS "Build tests" ON)

#if(NOT DXATC_XPSDK)
    #message(FATAL_ERROR "Supply xplane sdk with -DDXATC_XPSDK=<dir>. <dir> should have a CHeaders directory")
#endif()

#if(LINUX)
    #add_compile_definitions("LIN")
#elseif(WIN32)
    #add_compile_definitions("IBM")
#elseif(APPLE)
    #add_compile_definitions("APL")
#endif()
#add_compile_definitions("XPLM303" "XPLM301" "XPLM300" "XPLM210" "XPLM200")

set(DXATC_XPSDK_INCLUDE_DIRS 
    "${DXATC_XPSDK}/CHeaders/Widgets"
    "${DXATC_XPSDK}/CHeaders/Wrappers"
    "${DXATC_XPSDK}/CHeaders/XPLM")

include_directories(${DXATC_XPSDK_INCLUDE_DIRS})

set(DXATC_ENGINE_INCLUDE_DIRS
    lib/dxatc-engine/include
    lib/dxatc-utils/include

)

##
include_directories(${DXATC_ENGINE_INCLUDE_DIRS})
##

add_subdirectory(lib/dxatc-engine)
if(DXATC_BUILD_TESTS)
    add_subdirectory(tests)
endif()
