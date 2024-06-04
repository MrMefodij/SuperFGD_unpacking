message(STATUS "examples ... ")

ADD_EXECUTABLE(dump                                     SFGDDataDump.cpp)
TARGET_LINK_LIBRARIES(dump                              SFGDUnpack)

ADD_EXECUTABLE(ufe-monitor                              ufe_monitor.cpp)
TARGET_LINK_LIBRARIES(ufe-monitor                       SFGDUnpack  ${ROOT_LIBRARIES})

ADD_EXECUTABLE(unpack                                   SFGDUnpack.cpp)
TARGET_LINK_LIBRARIES(unpack                            SFGDUnpack SFGDoaEventDummy ${ROOT_LIBRARIES})

ADD_EXECUTABLE(calibration                              SFGDCalibration.cpp)
TARGET_LINK_LIBRARIES(calibration                       SfgdCommissioning SFGDUnpack ${ROOT_LIBRARIES} -lSpectrum)

ADD_EXECUTABLE(printEvents                              SFGDEventDisplay.cpp)
TARGET_LINK_LIBRARIES(printEvents                       SFGDUnpack SFGDoaEventDummy SfgdCommissioning ${ROOT_LIBRARIES})

ADD_EXECUTABLE(baseline                            		SFGDBaseLine.cpp)
TARGET_LINK_LIBRARIES(baseline                     		SfgdCommissioning SFGDUnpack ${ROOT_LIBRARIES})

ADD_EXECUTABLE(read_gain                            	SFGD_Gain_Reader.cpp)
TARGET_LINK_LIBRARIES(read_gain                     	SfgdCommissioning  SFGDUnpack ${ROOT_LIBRARIES})

ADD_EXECUTABLE(thresholdReading                          SFGDThresholdReading.cpp)
TARGET_LINK_LIBRARIES(thresholdReading              	SfgdCommissioning ${ROOT_LIBRARIES} SFGDoaEventDummy SFGDUnpack)

ADD_EXECUTABLE(thresholdStudy                           SFGDThresholdStudy.cpp)
TARGET_LINK_LIBRARIES(thresholdStudy              		SfgdCommissioning ${ROOT_LIBRARIES} SFGDoaEventDummy SFGDUnpack)

ADD_EXECUTABLE(timeThresholdStudyReading                SFGDTimeThresholdStudy.cpp)
TARGET_LINK_LIBRARIES(timeThresholdStudyReading         SfgdCommissioning ${ROOT_LIBRARIES} SFGDoaEventDummy SFGDUnpack)

ADD_EXECUTABLE(timeThresholdStudyCalibration            SFGDTimeThresholdStudyCalibration.cpp)
TARGET_LINK_LIBRARIES(timeThresholdStudyCalibration     SfgdCommissioning ${ROOT_LIBRARIES} SFGDoaEventDummy SFGDUnpack)

ADD_EXECUTABLE(dataCheck                                SFGDDataCheck.cpp)
TARGET_LINK_LIBRARIES(dataCheck                         SfgdCommissioning SFGDoaEventDummy SFGDUnpack ${ROOT_LIBRARIES} )

ADD_EXECUTABLE(attenuationLength                        SFGDAttenuationLength.cpp)
TARGET_LINK_LIBRARIES(attenuationLength                 SfgdCommissioning ${ROOT_LIBRARIES} SFGDUnpack)

ADD_EXECUTABLE(attenuationLengthVertical                SFGDAttenuationLengthVertical.cpp)
TARGET_LINK_LIBRARIES(attenuationLengthVertical         SfgdCommissioning ${ROOT_LIBRARIES} SFGDHitStructure)

ADD_EXECUTABLE(timingCalibration                        SFGDTimingCalibration.cpp)
TARGET_LINK_LIBRARIES(timingCalibration                 SfgdCommissioning ${ROOT_LIBRARIES} SFGDHitStructure)

ADD_EXECUTABLE(cableLengthCalibration                   SFGDCableLengthCalibration.cpp)
TARGET_LINK_LIBRARIES(cableLengthCalibration            SfgdCommissioning ${ROOT_LIBRARIES} SFGDHitStructure)

ADD_EXECUTABLE(febChannelTimingCalibration              SFGDTimeFebChannel.cpp)
TARGET_LINK_LIBRARIES(febChannelTimingCalibration       SfgdCommissioning ${ROOT_LIBRARIES} SFGDHitStructure)

ADD_EXECUTABLE(timeWalkCalibration                      SFGDTimeWalkCalibration.cpp)
TARGET_LINK_LIBRARIES(timeWalkCalibration               SfgdCommissioning ${ROOT_LIBRARIES} SFGDHitStructure)

ADD_EXECUTABLE(timeResolution                           SFGDTimeResolutionStudy.cpp)
TARGET_LINK_LIBRARIES(timeResolution                    SfgdCommissioning ${ROOT_LIBRARIES} SFGDHitStructure)

ADD_EXECUTABLE(attenuationLengthEvStructure             SFGDAttenuationLengthEventStructure.cpp)
TARGET_LINK_LIBRARIES(attenuationLengthEvStructure      SfgdCommissioning ${ROOT_LIBRARIES} SFGDHitStructure)

ADD_EXECUTABLE(fitHistogram                             SFGDFitHistogram.cpp)
TARGET_LINK_LIBRARIES(fitHistogram                      SfgdCommissioning ${ROOT_LIBRARIES} SFGDHitStructure)

ADD_EXECUTABLE(hgLgTotCalibration                       SFGDHG_LG_ToT_calibration.cpp)
TARGET_LINK_LIBRARIES(hgLgTotCalibration                SfgdCommissioning ${ROOT_LIBRARIES} SFGDUnpack)

ADD_EXECUTABLE(sandMuons                                SFGDSandMuons.cpp)
TARGET_LINK_LIBRARIES(sandMuons                         SFGDHitStructure SfgdCommissioning ${ROOT_LIBRARIES} SFGDUnpack)

ADD_EXECUTABLE(eventStructure                           SFGDEventStructure.cpp)
TARGET_LINK_LIBRARIES(eventStructure                    SFGDHitStructure SFGDUnpack SFGDoaEventDummy ${ROOT_LIBRARIES})

ADD_EXECUTABLE(readStructure                            SFGDReadStructure.cpp)
TARGET_LINK_LIBRARIES(readStructure                     ${ROOT_LIBRARIES}  SfgdCommissioning SFGDHitStructure SFGDUnpack)