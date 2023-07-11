cmake_minimum_required(VERSION 3.21)
project(unpacking)
set(CMAKE_CXX_STANDARD 17)
# Check if cmake has the required version
#cmake_minimum_required(VERSION 2.9.0 FATAL_ERROR)

# INCLUDE(${CMAKE_BINARY_DIR}/FindUNPACKING.cmake)
INCLUDE(${CMAKE_BINARY_DIR}/FindROOT.cmake)
set(CMAKE_CXX_FLAGS "-pthread")
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -std=c++17")

INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_SOURCE_DIR}/src)
#INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_SOURCE_DIR}/oaEvent/inc)
INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_SOURCE_DIR}/oaEventDummy/)
INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_SOURCE_DIR}/commissioning/)
LINK_DIRECTORIES(     ${CMAKE_CURRENT_SOURCE_DIR}/lib)


if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    cmake_policy(SET CMP0042 NEW)
endif(APPLE)

if    (ROOT_FOUND)
    INCLUDE_DIRECTORIES(${ROOT_INCLUDE_DIR})
    LINK_DIRECTORIES   (${ROOT_LIBRARY_DIR})
endif (ROOT_FOUND)

#find_package(RapidXML REQUIRED)


message(STATUS "C flags      : " ${CMAKE_C_FLAGS})
message(STATUS "CXX flags    : " ${CMAKE_CXX_FLAGS})
message(STATUS "Linker flags : " ${CMAKE_EXE_LINKER_FLAGS}\n)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/oaEvent)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/oaEventDummy)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/commissioning)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)

#install(FILES ${CMAKE_BINARY_DIR}/oaEventDummy/libSFGDoaEventDummyDict_rdict.pcm
#        ${CMAKE_BINARY_DIR}/oaEventDummy/libSFGDoaEventDummyDict.rootmap
#        ${CMAKE_BINARY_DIR}/oaEventDummy/SFGDoaEventDummyDict.cxx
#        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib)