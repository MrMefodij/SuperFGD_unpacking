//
// Created by amefodev on 14.09.2023.
//

#include <string>
#include <TPaveStats.h>
#include "EventDisplay.h"
#include "ToaEventDummy.h"
#include "TSFGDigit.hxx"

EventDisplay::EventDisplay(const std::string& evNumString) : evNumString_(evNumString){

    eventXY.GetYaxis()->SetTitle("Y [cm]");
    eventXY.GetXaxis()->SetTitle("X [cm]");
    eventXY.GetZaxis()->SetTitle("ToT [1.25 ns]");

    eventYZ.GetYaxis()->SetTitle("Y [cm]");
    eventYZ.GetXaxis()->SetTitle("Z [cm]");
    eventYZ.GetZaxis()->SetTitle("ToT [1.25 ns]");

    eventXZ.GetYaxis()->SetTitle("Z [cm]");
    eventXZ.GetXaxis()->SetTitle("X [cm]");
    eventXZ.GetZaxis()->SetTitle("ToT [1.25 ns]");

    eventHG_XY.GetYaxis()->SetTitle("Y [cm]");
    eventHG_XY.GetXaxis()->SetTitle("X [cm]");
    eventHG_XY.GetZaxis()->SetTitle("HG [ADC]");

    eventHG_YZ.GetYaxis()->SetTitle("Y [cm]");
    eventHG_YZ.GetXaxis()->SetTitle("Z [cm]");
    eventHG_YZ.GetZaxis()->SetTitle("HG [ADC]");

    eventHG_XZ.GetYaxis()->SetTitle("Z [cm]");
    eventHG_XZ.GetXaxis()->SetTitle("X [cm]");
    eventHG_XZ.GetZaxis()->SetTitle("HG [ADC]");

    eventLG_XY.GetYaxis()->SetTitle("Y [cm]");
    eventLG_XY.GetXaxis()->SetTitle("X [cm]");
    eventLG_XY.GetZaxis()->SetTitle("LG [ADC]");

    eventLG_YZ.GetYaxis()->SetTitle("Y [cm]");
    eventLG_YZ.GetXaxis()->SetTitle("Z [cm]");
    eventLG_YZ.GetZaxis()->SetTitle("LG [ADC]");

    eventLG_XZ.GetYaxis()->SetTitle("Z [cm]");
    eventLG_XZ.GetXaxis()->SetTitle("X [cm]");
    eventLG_XZ.GetZaxis()->SetTitle("LG [ADC]");

    eventXY_pe.GetYaxis()->SetTitle("Y [cm]");
    eventXY_pe.GetXaxis()->SetTitle("X [cm]");
    eventXY_pe.GetZaxis()->SetTitle("Ampl [pe]");
    eventXY_pe.SetStats(0);

    eventYZ_pe.GetYaxis()->SetTitle("Y [cm]");
    eventYZ_pe.GetXaxis()->SetTitle("Z [cm]");
    eventYZ_pe.GetZaxis()->SetTitle("Ampl [pe]");
    eventYZ_pe.SetStats(0);

    eventXZ_pe.GetYaxis()->SetTitle("Z [cm]");
    eventXZ_pe.GetXaxis()->SetTitle("X [cm]");
    eventXZ_pe.GetZaxis()->SetTitle("Ampl [pe]");
    eventXZ_pe.SetStats(0);

    eventTime.GetXaxis()->SetTitle("Time [1.25ns]");
    eventTime.GetYaxis()->SetTitle("N");

    gtsSlot.GetXaxis()->SetTitle("GTS time");
    gtsSlot.GetYaxis()->SetTitle("N");
}

void EventDisplay::AddEvents(const std::vector<FebData>& crateData, const Connection_Map& connectionMap, const Reader& gainResults, const Reader&  pedResults, TH1F & bunchStructure){
    unsigned int tDigits = 401;
    std::vector<std::pair<unsigned int, TSFGDigit>> eventsTime[tDigits];
    for (const auto& feb : crateData) {
//        std::cout << feb.febNumber_<< std::endl;

        for (int j = 0; j < feb.eventsHits.size(); ++j) {
//            std::cout << j << std::endl;
            eventsTime[(unsigned int) ((feb.eventsHits[j].GetRisingEdgeTDC() /*+ feb.eventsHits[j].GetGTSCounter()*8000 */) / 20)].push_back({feb.febNumber_, feb.eventsHits[j]});
        }
    }
    for (int i = 0; i < tDigits; ++i) {
        if (eventsTime[i].size() > 150) {
            eventExist_ = true;
            for (int j = 0; j < eventsTime[i].size(); ++j) {
                int timeDif =
                        eventsTime[i].at(j).second.GetFallingEdgeTDC() - eventsTime[i].at(j).second.GetRisingEdgeTDC();
                auto position = connectionMap.GetGlobalGeomPositionPtr(eventsTime[i].at(j).first,
                                                                       eventsTime[i].at(j).second.GetChannelNumber());
                if (position != NULL) {
                    auto gain = gainResults.GetOnlyValue(connectionMap.GetGlobalChannel(eventsTime[i].at(j).first,
                                                                                        eventsTime[i].at(
                                                                                                j).second.GetChannelNumber()));
                    auto pedestal = pedResults.GetOnlyValue(connectionMap.GetGlobalPedestalChannel(eventsTime[i].at(j).first,
                                                                                           eventsTime[i].at(
                                                                                                   j).second.GetChannelNumber()));
                    std::string pcbPosition = connectionMap.GetPcbPosition(eventsTime[i].at(j).first);
                    if (pcbPosition.find("U-") != pcbPosition.npos) {
                        if (eventXY.GetBinContent(position->x_ + 1, position->y_ + 1) == 0) {
                            gtsSlot.Fill(eventsTime[i].at(j).second.GetGTSCounter());
                            eventTime.Fill(eventsTime[i].at(j).second.GetRisingEdgeTDC());
                            bunchStructure.Fill(eventsTime[i].at(j).second.GetTimeFromGateStart());
                            eventXY.Fill(position->x_, position->y_, timeDif);
                            eventHG_XY.Fill(position->x_, position->y_, eventsTime[i].at(j).second.GetHighGainADC());
                            eventLG_XY.Fill(position->x_, position->y_, eventsTime[i].at(j).second.GetLowGainADC());
                            if (eventsTime[i].at(j).second.GetHighGainADC() > 0 &&
                                eventsTime[i].at(j).second.GetHighGainADC() > -pedestal) {
                                eventXY_pe.Fill(position->x_, position->y_,
                                                (eventsTime[i].at(j).second.GetHighGainADC() - pedestal) / gain);
                            }
                            eventXY_time.Fill(position->x_, position->y_,
                                              eventsTime[i].at(j).second.GetTimeFromGateStart());
                        }
                    } else if (pcbPosition.find("L-") != pcbPosition.npos ||
                               pcbPosition.find("R-") != pcbPosition.npos) {
                        if (eventYZ.GetBinContent(position->z_ + 1, position->y_ + 1) == 0) {
                            gtsSlot.Fill(eventsTime[i].at(j).second.GetGTSCounter());
                            eventTime.Fill(eventsTime[i].at(j).second.GetRisingEdgeTDC());
                            bunchStructure.Fill(eventsTime[i].at(j).second.GetTimeFromGateStart());
                            eventYZ.Fill(position->z_, position->y_, timeDif);
                            eventHG_YZ.Fill(position->z_, position->y_, eventsTime[i].at(j).second.GetHighGainADC());
                            eventLG_YZ.Fill(position->z_, position->y_, eventsTime[i].at(j).second.GetLowGainADC());
                            if (eventsTime[i].at(j).second.GetHighGainADC() > 0 &&
                                eventsTime[i].at(j).second.GetHighGainADC() > -pedestal) {
                                eventYZ_pe.Fill(position->z_, position->y_,
                                                (eventsTime[i].at(j).second.GetHighGainADC() - pedestal) / gain);
                            }
                            eventYZ_time.Fill(position->z_, position->y_,
                                              eventsTime[i].at(j).second.GetTimeFromGateStart());
                        }
                    } else if (pcbPosition.find("T-") != pcbPosition.npos) {
                        if (eventXZ.GetBinContent(position->x_ + 1, position->z_ + 1) == 0) {
                            gtsSlot.Fill(eventsTime[i].at(j).second.GetGTSCounter());
                            eventTime.Fill(eventsTime[i].at(j).second.GetRisingEdgeTDC());
                            bunchStructure.Fill(eventsTime[i].at(j).second.GetTimeFromGateStart());
                            eventXZ.Fill(position->x_, position->z_, timeDif);
                            eventHG_XZ.Fill(position->x_, position->z_, eventsTime[i].at(j).second.GetHighGainADC());
                            eventLG_XZ.Fill(position->x_, position->z_, eventsTime[i].at(j).second.GetLowGainADC());
                            if (eventsTime[i].at(j).second.GetHighGainADC() > 0 &&
                                eventsTime[i].at(j).second.GetHighGainADC() > -pedestal) {
                                eventXZ_pe.Fill(position->x_, position->z_,
                                                (eventsTime[i].at(j).second.GetHighGainADC() - pedestal) / gain);
                            }
                            eventXZ_time.Fill(position->x_, position->z_,
                                              eventsTime[i].at(j).second.GetTimeFromGateStart());
                        }
                    }
                } else {
//                    std::cerr << " Problem in connection map : FEB " << eventsTime[i].at(j).first <<
//                              " channel " << eventsTime[i].at(j).second.GetChannelNumber() << std::endl;
                }
            }
            if (i > 0) {
                --i;
                for (int j = 0; j < eventsTime[i].size(); ++j) {
                    int timeDif = eventsTime[i].at(j).second.GetFallingEdgeTDC() -
                                  eventsTime[i].at(j).second.GetRisingEdgeTDC();
                    auto position = connectionMap.GetGlobalGeomPositionPtr(eventsTime[i].at(j).first, eventsTime[i].at(
                            j).second.GetChannelNumber());
                    if (position != NULL) {
                        auto gain = gainResults.GetOnlyValue(connectionMap.GetGlobalChannel(eventsTime[i].at(j).first,
                                                                                            eventsTime[i].at(
                                                                                                    j).second.GetChannelNumber()));
                        auto pedestal = pedResults.GetOnlyValue(
                                connectionMap.GetGlobalPedestalChannel(eventsTime[i].at(j).first,
                                                               eventsTime[i].at(
                                                                       j).second.GetChannelNumber()));
                        std::string pcbPosition = connectionMap.GetPcbPosition(eventsTime[i].at(j).first);
                        if (pcbPosition.find("U-") != pcbPosition.npos) {
                            if (eventXY.GetBinContent(position->x_ + 1, position->y_ + 1) < 1) {
                                gtsSlot.Fill(eventsTime[i].at(j).second.GetGTSCounter());
                                eventTime.Fill(eventsTime[i].at(j).second.GetRisingEdgeTDC());
                                bunchStructure.Fill(eventsTime[i].at(j).second.GetTimeFromGateStart());
                                eventXY.Fill(position->x_, position->y_, timeDif);
                                eventHG_XY.Fill(position->x_, position->y_,
                                                eventsTime[i].at(j).second.GetHighGainADC());
                                eventLG_XY.Fill(position->x_, position->y_, eventsTime[i].at(j).second.GetLowGainADC());
                                if (eventsTime[i].at(j).second.GetHighGainADC() > 0 &&
                                    eventsTime[i].at(j).second.GetHighGainADC() > -pedestal) {
                                    eventXY_pe.Fill(position->x_, position->y_,
                                                    (eventsTime[i].at(j).second.GetHighGainADC() - pedestal) / gain);
                                }
                                eventXY_time.Fill(position->x_, position->y_,
                                                  eventsTime[i].at(j).second.GetTimeFromGateStart());
                            }
                        } else if (pcbPosition.find("L-") != pcbPosition.npos ||
                                   pcbPosition.find("R-") != pcbPosition.npos) {
                            if (eventYZ.GetBinContent(position->z_ + 1, position->y_ + 1) < 1) {
                                gtsSlot.Fill(eventsTime[i].at(j).second.GetGTSCounter());
                                eventTime.Fill(eventsTime[i].at(j).second.GetRisingEdgeTDC());
                                bunchStructure.Fill(eventsTime[i].at(j).second.GetTimeFromGateStart());
                                eventYZ.Fill(position->z_, position->y_, timeDif);
                                eventHG_YZ.Fill(position->z_, position->y_,
                                                eventsTime[i].at(j).second.GetHighGainADC());
                                eventLG_YZ.Fill(position->z_, position->y_, eventsTime[i].at(j).second.GetLowGainADC());
                                if (eventsTime[i].at(j).second.GetHighGainADC() > 0 &&
                                    eventsTime[i].at(j).second.GetHighGainADC() > -pedestal) {
                                    eventYZ_pe.Fill(position->z_, position->y_,
                                                    (eventsTime[i].at(j).second.GetHighGainADC() - pedestal) / gain);
                                }
                                eventYZ_time.Fill(position->z_, position->y_,
                                                  eventsTime[i].at(j).second.GetTimeFromGateStart());
                            }
                        } else if (pcbPosition.find("T-") != pcbPosition.npos) {
                            if (eventXZ.GetBinContent(position->x_ + 1, position->z_ + 1) < 1) {
                                gtsSlot.Fill(eventsTime[i].at(j).second.GetGTSCounter());
                                eventTime.Fill(eventsTime[i].at(j).second.GetRisingEdgeTDC());
                                bunchStructure.Fill(eventsTime[i].at(j).second.GetTimeFromGateStart());
                                eventXZ.Fill(position->x_, position->z_, timeDif);
                                eventHG_XZ.Fill(position->x_, position->z_,
                                                eventsTime[i].at(j).second.GetHighGainADC());
                                eventLG_XZ.Fill(position->x_, position->z_, eventsTime[i].at(j).second.GetLowGainADC());
                                if (eventsTime[i].at(j).second.GetHighGainADC() > 0 &&
                                    eventsTime[i].at(j).second.GetHighGainADC() > -pedestal) {
                                    eventXZ_pe.Fill(position->x_, position->z_,
                                                    (eventsTime[i].at(j).second.GetHighGainADC() - pedestal) / gain);
                                }
                                eventXZ_time.Fill(position->x_, position->z_,
                                                  eventsTime[i].at(j).second.GetTimeFromGateStart());
                            }
                        }
                    } else {
//                        std::cerr << " Problem in connection map : FEB " << eventsTime[i].at(j).first <<
//                                  " channel " << eventsTime[i].at(j).second.GetChannelNumber() << std::endl;
                    }
                }
                ++i;
            }
            if (i < tDigits - 1) {
                ++i;
                for (int j = 0; j < eventsTime[i].size(); ++j) {
                    int timeDif = eventsTime[i].at(j).second.GetFallingEdgeTDC() -
                                  eventsTime[i].at(j).second.GetRisingEdgeTDC();
                    auto position = connectionMap.GetGlobalGeomPositionPtr(eventsTime[i].at(j).first, eventsTime[i].at(
                            j).second.GetChannelNumber());
                    if (position != NULL) {
                        auto gain = gainResults.GetOnlyValue(connectionMap.GetGlobalChannel(eventsTime[i].at(j).first,
                                                                                            eventsTime[i].at(
                                                                                                    j).second.GetChannelNumber()));
                        auto pedestal = pedResults.GetOnlyValue(
                                connectionMap.GetGlobalPedestalChannel(eventsTime[i].at(j).first,
                                                               eventsTime[i].at(
                                                                       j).second.GetChannelNumber()));
                        std::string pcbPosition = connectionMap.GetPcbPosition(eventsTime[i].at(j).first);
                        if (pcbPosition.find("U-") != pcbPosition.npos) {
                            if (eventXY.GetBinContent(position->x_ + 1, position->y_ + 1) == 0) {
                                gtsSlot.Fill(eventsTime[i].at(j).second.GetGTSCounter());
                                eventTime.Fill(eventsTime[i].at(j).second.GetRisingEdgeTDC());
                                bunchStructure.Fill(eventsTime[i].at(j).second.GetTimeFromGateStart());
                                eventXY.Fill(position->x_, position->y_, timeDif);
                                eventHG_XY.Fill(position->x_, position->y_,
                                                eventsTime[i].at(j).second.GetHighGainADC());
                                eventLG_XY.Fill(position->x_, position->y_, eventsTime[i].at(j).second.GetLowGainADC());
                                if (eventsTime[i].at(j).second.GetHighGainADC() > 0 &&
                                    eventsTime[i].at(j).second.GetHighGainADC() > -pedestal) {
                                    eventXY_pe.Fill(position->x_, position->y_,
                                                    (eventsTime[i].at(j).second.GetHighGainADC() - pedestal) / gain);
                                }
                                eventXY_time.Fill(position->x_, position->y_,
                                                  eventsTime[i].at(j).second.GetTimeFromGateStart());
                            }
                        } else if (pcbPosition.find("L-") != pcbPosition.npos ||
                                   pcbPosition.find("R-") != pcbPosition.npos) {
                            if (eventYZ.GetBinContent(position->z_ + 1, position->y_ + 1) == 0) {
                                gtsSlot.Fill(eventsTime[i].at(j).second.GetGTSCounter());
                                eventTime.Fill(eventsTime[i].at(j).second.GetRisingEdgeTDC());
                                bunchStructure.Fill(eventsTime[i].at(j).second.GetTimeFromGateStart());
                                eventYZ.Fill(position->z_, position->y_, timeDif);
                                eventHG_YZ.Fill(position->z_, position->y_,
                                                eventsTime[i].at(j).second.GetHighGainADC());
                                eventLG_YZ.Fill(position->z_, position->y_, eventsTime[i].at(j).second.GetLowGainADC());
                                if (eventsTime[i].at(j).second.GetHighGainADC() > 0 &&
                                    eventsTime[i].at(j).second.GetHighGainADC() > -pedestal) {
                                    eventYZ_pe.Fill(position->z_, position->y_,
                                                    (eventsTime[i].at(j).second.GetHighGainADC() - pedestal) / gain);
                                }
                                eventYZ_time.Fill(position->z_, position->y_,
                                                  eventsTime[i].at(j).second.GetTimeFromGateStart());
                            }
                        } else if (pcbPosition.find("T-") != pcbPosition.npos) {
                            if (eventXZ.GetBinContent(position->x_ + 1, position->z_ + 1) == 0) {
                                gtsSlot.Fill(eventsTime[i].at(j).second.GetGTSCounter());
                                eventTime.Fill(eventsTime[i].at(j).second.GetRisingEdgeTDC());
                                bunchStructure.Fill(eventsTime[i].at(j).second.GetTimeFromGateStart());
                                eventXZ.Fill(position->x_, position->z_, timeDif);
                                eventHG_XZ.Fill(position->x_, position->z_,
                                                eventsTime[i].at(j).second.GetHighGainADC());
                                eventLG_XZ.Fill(position->x_, position->z_, eventsTime[i].at(j).second.GetLowGainADC());
                                if (eventsTime[i].at(j).second.GetHighGainADC() > 0 &&
                                    eventsTime[i].at(j).second.GetHighGainADC() > -pedestal) {
                                    eventXZ_pe.Fill(position->x_, position->z_,
                                                    (eventsTime[i].at(j).second.GetHighGainADC() - pedestal) / gain);
                                }
                                eventXZ_time.Fill(position->x_, position->z_,
                                                  eventsTime[i].at(j).second.GetTimeFromGateStart());
                            }
                        }
                    } else {
//                        std::cerr << " Problem in connection map : FEB " << eventsTime[i].at(j).first <<
//                                  " channel " << eventsTime[i].at(j).second.GetChannelNumber() << std::endl;
                    }
                }
                --i;
            }
        }
    }
}

EventTime EventDisplay::GetPictures(){
    return { &gtsSlot, &eventTime, eventExist_ };
}

EventMap3D EventDisplay::GetPictures(const std::string& ampl){
    if (ampl == "ToT"){
        return {&eventXY, &eventYZ, &eventXZ};
    } else  if (ampl == "HG"){
        return {&eventHG_XY, &eventHG_YZ, &eventHG_XZ};
    } else if (ampl == "LG"){
        return {&eventLG_XY, &eventLG_YZ, &eventLG_XZ};
    } else if (ampl == "pe"){
        return {&eventXY_pe, &eventYZ_pe, &eventXZ_pe};
    } else if (ampl == "time") {
        return {&eventXY_time, &eventYZ_time, &eventXZ_time};
    }
    std::cerr << "error in ampl type" <<std::endl;
    return {nullptr, nullptr, nullptr};
}

EventDisplay::~EventDisplay(){
    eventXY.Delete();
    eventYZ.Delete();
    eventXZ.Delete();

    eventHG_XY.Delete();
    eventHG_YZ.Delete();
    eventHG_XZ.Delete();

    eventLG_XY.Delete();
    eventLG_YZ.Delete();
    eventLG_XZ.Delete();

    eventXY_pe.Delete();
    eventYZ_pe.Delete();
    eventXZ_pe.Delete();

    eventXZ_time.Delete();
    eventYZ_time.Delete();
    eventXZ_time.Delete();

    gtsSlot.Delete();
    eventTime.Delete();
}