cmake_minimum_required (VERSION 2.6)

#Create options
option(BUILD_STATIC "Tell if the static library should be compiled" ON)
option(BUILD_DYNAMIC "Tell if the dynamic library should be compiled" ON)
option(BUILD_DEBUG "Should we add debug flags?" OFF)
option(BUILD_DOC "Tell if the doc target should be added" ON)

set(ClanLib_MAJOR_VERSION 2)
set(ClanLib_MINOR_VERSION 3)

# C++ 2011 flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
if(BUILD_DEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
else(BUILD_DEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif(BUILD_DEBUG)

#C 99 flag
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
if(BUILD_DEBUG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb")
else(BUILD_DEBUG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif(BUILD_DEBUG)

# Add modules used by this project to the module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#ClanPgsql stuff
set(ClanPgsql_LIBRARY_NAME "clan${ClanLib_MAJOR_VERSION}${ClanLib_MINOR_VERSION}Pgsql")
set(ClanPgsql_INCLURE_DIR "ClanLib-${ClanLib_MAJOR_VERSION}.${ClanLib_MINOR_VERSION}")

#Includes
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)

#Check for clanlib installed
find_package(ClanLib COMPONENTS Core Database REQUIRED)
include_directories(${ClanLib_INCLUDE_DIRS})

# Make libraries
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
set(INCLUDE_FILES_DIR ${PROJECT_SOURCE_DIR}/include/ClanLib/)


if(BUILD_STATIC OR BUILD_DYNAMIC)
  install(DIRECTORY ${INCLUDE_FILES_DIR} DESTINATION
    "include/${ClanPgsql_INCLURE_DIR}/ClanLib")
endif()

if(BUILD_STATIC)
  add_library(ClanPgsql_Static STATIC ${SOURCE_FILES})
  set_target_properties(ClanPgsql_Static PROPERTIES OUTPUT_NAME ${ClanPgsql_LIBRARY_NAME})

  install(TARGETS ClanPgsql_Static DESTINATION lib)
endif(BUILD_STATIC)

if(BUILD_DYNAMIC)
  add_library(ClanPgsql_Dyn SHARED ${SOURCE_FILES})
  set_target_properties(ClanPgsql_Dyn PROPERTIES OUTPUT_NAME ${ClanPgsql_LIBRARY_NAME})

  install(TARGETS ClanPgsql_Dyn DESTINATION lib)
endif(BUILD_DYNAMIC)

#doxygen
if(BUILD_DOC)
  add_subdirectory(doc)
endif(BUILD_DOC)

