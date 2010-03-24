#/**********************************************************\ 
# Auto-generated CMakeLists.txt for the linphone project
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/common.cmake)

set (PROJNAME ${PLUGIN_NAME})

browserplugin_project(${PROJNAME})

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    *.cmake
    )

SET(CORE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/linphone/")
INCLUDE_DIRECTORIES(${CORE_PATH}/oRTP/include ${CORE_PATH}/mediastreamer2/include ${CORE_PATH}/coreapi ${CORE_PATH}/)
SET(PLUGIN_INTERNAL_DEPS ${PLUGIN_INTERNAL_DEPS} ${CORE_PATH}/coreapi/.libs/liblinphone.so ${CORE_PATH}/mediastreamer2/src/.libs/libmediastreamer.so ${CORE_PATH}/oRTP/src/.libs/libortp.so -lspeex -losipparser2 -losip2)


# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET( SOURCES
    ${GENERAL}
    ${GENERATED}
    )

# This will include Win/projectDef.cmake, Linux/projectDef.cmake, etc
include_platform()
