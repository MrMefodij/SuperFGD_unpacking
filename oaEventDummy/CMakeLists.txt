message(STATUS "src ... oaEventDummy")

AUX_SOURCE_DIRECTORY(.                 SOURCE_FILES)
#message(STATUS "${SOURCE_FILES}")
INCLUDE_DIRECTORIES(. )
set (EVENT_DICT_HEADERFILES
        ./*.h
        )

message(STATUS "${EVENT_DICT_HEADERFILES}")

ROOT_GENERATE_DICTIONARY(SFGDoaEventDummyDict ToaEventDummy.h  LINKDEF LinkDef.h )
#ROOT_GENERATE_DICTIONARY(oaEvents_dict twoints.hpp TChannelId.hxx MODULE readwrite_twoints LINKDEF LinkDef.h)

ADD_LIBRARY(SFGDoaEventDummy      SHARED            ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(SFGDoaEventDummy              ${ROOT_LIBRARIES})
