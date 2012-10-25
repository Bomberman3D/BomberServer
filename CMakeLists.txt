CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )

PROJECT( "Bomberman3DServer" )

SET( CMAKE_MODULE_PATH
     "${PROJECT_SOURCE_DIR}/cmake"
     "${CMAKE_MODULE_PATH}" )

SET( TARGET_NAME        "Bomberman3DServer" )
SET( TARGET_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include" )
SET( TARGET_SOURCE_DIR  "${PROJECT_SOURCE_DIR}/src" )

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package( Boost COMPONENTS thread )

if(Boost_FOUND)
   include_directories(${Boost_INCLUDE_DIRS})
endif()

add_definitions(-lboost_thread-mt -lpthread)

ADD_SUBDIRECTORY( "src" )
