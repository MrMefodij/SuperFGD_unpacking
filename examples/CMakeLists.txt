message(STATUS "examples ... ")

ADD_EXECUTABLE(dump                                     SFGDDataDump.cpp)
TARGET_LINK_LIBRARIES(dump                              SFGDUnpack)

ADD_EXECUTABLE(ufe-monitor                              ufe_monitor.cpp)
TARGET_LINK_LIBRARIES(ufe-monitor                       SFGDUnpack  ${ROOT_LIBRARIES})

ADD_EXECUTABLE(unpack                                   SFGDUnpack.cpp)
TARGET_LINK_LIBRARIES(unpack                            SFGDUnpack SFGDoaEventDummy ${ROOT_LIBRARIES})

ADD_EXECUTABLE(calibration                              SFGDCalibration.cpp)
TARGET_LINK_LIBRARIES(calibration                       SFGDUnpack ${ROOT_LIBRARIES} -lSpectrum)

ADD_EXECUTABLE(printEvents                              SFGDEventDisplay.cpp)
TARGET_LINK_LIBRARIES(printEvents                       SFGDUnpack SFGDoaEventDummy ${ROOT_LIBRARIES})

ADD_EXECUTABLE(baseline                            		SFGDBaseLine.cpp)
TARGET_LINK_LIBRARIES(baseline                     		SfgdCommissioning SFGDUnpack ${ROOT_LIBRARIES} -lSpectrum)

ADD_EXECUTABLE(readXml                            		SFGDReadXml.cpp)
TARGET_LINK_LIBRARIES(readXml                     		SfgdCommissioning)

ADD_EXECUTABLE(thresholdStudy                           SFGDThresholdStudy.cpp)
TARGET_LINK_LIBRARIES(thresholdStudy              		SfgdCommissioning ${ROOT_LIBRARIES} SFGDoaEventDummy SFGDUnpack)