message(STATUS "hitStructure")
AUX_SOURCE_DIRECTORY(.                 SOURCE_FILES)
#message(STATUS "${SOURCE_FILES}")


ROOT_GENERATE_DICTIONARY(SFGDHitEventDict Hit.h LINKDEF LinkDef.h )

ADD_LIBRARY(SFGDHitStructure      SHARED            ${SOURCE_FILES}  SFGDHitEventDict.cxx)
TARGET_LINK_LIBRARIES(SFGDHitStructure     PUBLIC    ${ROOT_LIBRARIES})

#add_custom_command(
#        TARGET SFGDHitStructure
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target install
#)