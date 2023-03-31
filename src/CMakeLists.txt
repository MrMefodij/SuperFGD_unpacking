message(STATUS "src ... ")

AUX_SOURCE_DIRECTORY(.                 SOURCE_FILES)
#message(STATUS "${SOURCE_FILES}")

ADD_LIBRARY(SFGDUnpack      SHARED            ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(SFGDUnpack              ${ROOT_LIBRARIES} -lSpectrum)
