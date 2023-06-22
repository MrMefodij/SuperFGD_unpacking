message(STATUS "commissioning ... ")

AUX_SOURCE_DIRECTORY(.                 SOURCE_FILES)

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

ADD_LIBRARY(SfgdCommissioning      SHARED            ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(SfgdCommissioning              ${ROOT_LIBRARIES} )
