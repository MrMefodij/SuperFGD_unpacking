message(STATUS "src ... ")

AUX_SOURCE_DIRECTORY(.                 SOURCE_FILES)
#message(STATUS "${SOURCE_FILES}")

ADD_LIBRARY(SfgdCommissioning      SHARED            ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(SfgdCommissioning              ${ROOT_LIBRARIES})
