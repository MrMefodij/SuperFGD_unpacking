message(STATUS "src ... oaEventDummy")

AUX_SOURCE_DIRECTORY(.                 SOURCE_FILES)
#message(STATUS "${SOURCE_FILES}")
INCLUDE_DIRECTORIES(. )

message(STATUS "Looking for ROOT")
find_package(ROOT REQUIRED COMPONENTS RIO)
message(STATUS "ROOT ${ROOT_VERSION} found at ${ROOT_BINDIR}")

ROOT_GENERATE_DICTIONARY(SFGDoaEventDummyDict ToaEventDummy.h TDigitDummy.h TSFGDigit.hxx  LINKDEF LinkDef.h )


ADD_LIBRARY(SFGDoaEventDummy      SHARED            ${SOURCE_FILES} SFGDoaEventDummyDict.cxx)
TARGET_LINK_LIBRARIES(SFGDoaEventDummy              ${ROOT_LIBRARIES})
