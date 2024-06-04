/* This file is part of SuperFGD software package. This software
 * package is designed for internal use for the SuperFGD detector
 * collaboration and is tailored for this use primarily.
 *
 * Unpacking is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Unpacking is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SuperFGD Unpacking.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
//
// Created by Maria on 22.03.2024 kolupanova@inr.ru
//

#include "MDargumentHandler.h"
#include "ToaEventDummy.h"
#include "SFGD_defines.h"
#include "EventDisplay.h"
#include "OutputHandler.h"
#include "Hit.h"
#include <set>
#include <TTree.h>
#include <TFile.h>

int main( int argc, char **argv ) {
    std::string stringBuf;
    // The following shows how to use the MDargumentHandler class
    // to deal with the main arguments
    // Define the arguments
    MDargumentHandler argh("Example of unpacking application.");
    argh.Init();

    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}

    if (argh.GetMode() == "f") {
        if (argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else if (argh.GetMode() == "d") {
        if (argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else {
        return -1;
    }

    std::vector<std::string> vFileNames  = argh.GetDataFiles(stringBuf,".root","_plots");
    std::sort(vFileNames.begin(), vFileNames.end(), [](const auto& itL, const  auto& itR){return itL > itR;});

    if (vFileNames.empty()) {
        std::cerr << "Can not open directory " << stringBuf << std::endl;
        return 1;
    }

//    size_t pos = stringBuf.find("_plots");
//    std::string rootFileOutput;
//    if(pos != string::npos)
//        rootFileOutput = stringBuf.substr(0, pos) + "_";
//    else{
//        rootFileOutput = stringBuf;
//    }
//    rootFileOutput += "EventStructure.root";
//    std::cout << rootFileOutput << std::endl;
//
//    TFile wfile(rootFileOutput.c_str(), "recreate");
    std::string mapFile = "../connection_map/SFG_Geometry_Map_v18.csv";
    if(getenv("UNPACKING_ROOT")!=NULL){
        mapFile = (std::string)getenv("UNPACKING_ROOT") + "/connection_map/SFG_Geometry_Map_v18.csv";
    }
    Connection_Map connectionMap(mapFile);
    try {
        connectionMap.Init();
    } catch (const std::exception& e) {
        std::cerr << "Unable to open file " << mapFile << std::endl;
        exit(1);
    }

    std::vector<std::string> vGainResult = argh.GetDirectoryFiles("../calibration_results/4795/", ".txt");
    std::sort(vGainResult.begin(), vGainResult.end());
    if (vGainResult.empty()) {
        std::cerr << "Can not open directory " << stringBuf << std::endl;
        return 1;
    }
    GainReader calib_res(vGainResult);
    calib_res.Init();

    std::vector<std::string> vLgHgResult = argh.GetDirectoryFiles("../hg_lg_calibration_results/hg40_lg56/", ".txt");
    std::sort(vLgHgResult.begin(), vLgHgResult.end());
    if (vLgHgResult.empty()) {
        std::cerr << "Can not open directory " << stringBuf << std::endl;
        return 1;
    }
    LgHgReader LgHg_res(vLgHgResult);
    LgHg_res.Init();

    std::vector<std::string> vPedResult = argh.GetDirectoryFiles("../pedmap_results/", ".dat");
    std::sort(vPedResult.begin(), vPedResult.end());
    if (vPedResult.empty()) {
        std::cerr << "Can not open directory " << stringBuf << std::endl;
        return 1;
    }
    PedMapReader pedResult(vPedResult);
    pedResult.Init();

    std::vector<ToaEventDummy> *FEBs[SFGD_FEBS_NUM];
    for (Int_t i = 0; i < SFGD_FEBS_NUM; ++i) {
        FEBs[i] = 0;
    }

    unsigned int oaEventDummySize;
    OutputHandler outh;
    std::string outputdir=stringBuf;
    for(unsigned int file = 0; file < vFileNames.size(); ++file) {
        DataFile allFiles;
        std::string outputFilePrefix = outh.GetOutputFilePrefix(vFileNames[file], outputdir);
        size_t pos = outputFilePrefix.find("_plots");
        if(pos != std::string::npos)
            outputFilePrefix = outputFilePrefix.substr(0, pos);
        std::string rootFileOutput = outputFilePrefix + "_EventStructure.root";
        std::cout << "ROOT output: " << rootFileOutput << std::endl;
        TFile rfile(rootFileOutput.c_str(), "recreate");
        allFiles.fileName_ = vFileNames.at(file);
        allFiles.FileInput_ = new TFile((vFileNames.at(file)).c_str());
        if (!allFiles.FileInput_->IsOpen()) {
            std::cerr << "Can not open file " << vFileNames.at(file) << std::endl;
            return 1;
        }

        std::vector<unsigned int> availableFebs;
        allFiles.AllEvents_ = (TTree *) allFiles.FileInput_->Get("AllEvents");
        std::ostringstream sFEBnum;
        std::string sFEB;
        for (unsigned int ih = 0; ih < SFGD_FEBS_NUM; ++ih) {
            sFEBnum.str("");
            sFEBnum << ih;
            sFEB = "FEB_" + sFEBnum.str();
            if (allFiles.AllEvents_->GetBranch((sFEB).c_str())) {
                allFiles.AllEvents_->SetBranchAddress((sFEB).c_str(), &FEBs[ih]);
                availableFebs.push_back(ih);
            }
        }
        if (!availableFebs.empty()) {
            unsigned int nEntries = allFiles.AllEvents_->GetEntries();
            allFiles.availableCrates_ = (CrateGates{(availableFebs.back() >> 4), std::move(availableFebs),
                                                    nEntries});

        }
        allFiles.AllEvents_->SetBranchAddress("oaEventDummySize", &oaEventDummySize);

        unsigned int evNum = 0;
        unsigned long long int gateTime = 0;
        bool gateIsOk = false;
        std::vector<Hit> eventStructure;
        rfile.cd();
        TTree EventsHits("EventsHits", "The ROOT tree of events");
        EventsHits.Branch("Events", "std::vector<Hit>", &eventStructure);
        EventsHits.Branch("EventNumber", &evNum, "EventNumber/i");
        EventsHits.Branch("EventTime", &gateTime, "EventTime/l");
        const auto &a = allFiles;
        std::cout << "entries in file " << a.fileName_ << " is " << a.AllEvents_->GetEntries() << std::endl;
        for (unsigned int i = 0; i < a.AllEvents_->GetEntries(); ++i) {
            try {
                a.AllEvents_->GetEntry(i);
            } catch (...) {
                std::cerr << "Error in file " << a.fileName_ << " entry " << i << std::endl;
            }
            gateIsOk = true;
            if (oaEventDummySize < Event_Min_Size)
                gateIsOk = false;
            for (const auto feb: a.availableCrates_.availableFebs_) {
                if (gateIsOk) {
                    try {
//                        cout << AS_KV(feb) << endl;
                        std::vector<TSFGDigit> temp = FEBs[feb]->at(0).GetHits();
                    } catch (...) {
                        gateIsOk = false;
                        std::cerr << "Error on OCB event Number: \t" << evNum + 1 << "\t FEB#: " << feb
                                  << std::endl;
                        break;
                    }
                }
            }
            if (gateIsOk) {
                evNum = FEBs[a.availableCrates_.availableFebs_.at(0)]->back().GetEventNumber();
                gateTime = FEBs[a.availableCrates_.availableFebs_.at(0)]->back().GetGateTime();
                size_t maxEntries = oaEventDummySize;
                eventStructure.reserve(maxEntries);
                rfile.cd();
                for (const auto &febNum: a.availableCrates_.availableFebs_) {
                    if (FEBs[febNum]->at(0).GetGateType() != 3) {// || FEBs[febNum]->at(0).GetGateType() == 2) {
                        for (auto hit: FEBs[febNum]->at(0).GetHits()) {
                            auto glGeomPosition = connectionMap.GetGlobalGeomPositionPtr(febNum,
                                                                                         hit.GetChannelNumber());
                            if (glGeomPosition == nullptr) {
//                                std::cerr << "Not in the map. Skipped: feb " << febNum << " channel " << hit.GetChannelNumber() << std::endl;
                                continue;
                            }
                            std::string pcbPosition = connectionMap.GetPcbPosition(febNum);
                            /// In cause of different PCBPosition z/x/y value is set to NAN in order not to be added into the tree

                            Hit eventHit;
                            if (pcbPosition.find("U-") != pcbPosition.npos) {
                                eventHit._x = glGeomPosition->x_;
                                eventHit._y = glGeomPosition->y_;
                                eventHit._z = NAN;
                            } else if (pcbPosition.find("L-") != pcbPosition.npos ||
                                       pcbPosition.find("R-") != pcbPosition.npos) {
                                eventHit._z = glGeomPosition->z_;
                                eventHit._y = glGeomPosition->y_;
                                eventHit._x = NAN;
                            } else if (pcbPosition.find("T-") != pcbPosition.npos) {
                                eventHit._z = glGeomPosition->z_;
                                eventHit._x = glGeomPosition->x_;
                                eventHit._y = NAN;
                            } else {
                                std::cerr << " Problem in connection map : FEB " << febNum <<
                                          " channel " << hit.GetChannelNumber() << std::endl;
                            }
                            eventHit._board_id = febNum;
                            eventHit._channel_id = hit.GetChannelNumber();
                            eventHit._global_ch_id = connectionMap.GetGlobalChannel(febNum, hit.GetChannelNumber());
                            eventHit._timeFromGateStart = hit.GetTimeFromGateStart();
                            eventHit._highGainADC = hit.GetHighGainADC();
                            eventHit._lowGainADC = hit.GetLowGainADC();
                            eventHit._ToT = hit.GetTimeOverThreshold();
                            auto gain = calib_res.GetOnlyValue(eventHit._global_ch_id);
                            auto pedestal = pedResult.GetOnlyValue(connectionMap.GetGlobalPedestalChannel(febNum, hit.GetChannelNumber()));
                            auto HgLg = LgHg_res.GetOnlyValue(eventHit._global_ch_id);
                            auto LgPedestal = LgHg_res.GetOnly2dPedestal(eventHit._global_ch_id);
                            if (hit.GetHighGainADC() > pedestal)
                                if(hit.GetHighGainADC() < HGMAXValue || HgLg == NAN || LgPedestal == NAN)
                                    eventHit._LY_PE = (hit.GetHighGainADC() - pedestal) / gain;
                                else
                                    eventHit._LY_PE = ((hit.GetLowGainADC() - LgPedestal )/ HgLg - pedestal) / gain;
                            else {
                                eventHit._LY_PE = NAN;
                            }
                            eventStructure.push_back(eventHit);

                        }
                    }
                }
                std::sort(eventStructure.begin(), eventStructure.end());
                EventsHits.Fill();
                eventStructure.clear();
            }
        }
        EventsHits.Write("", TObject::kOverwrite);
        rfile.cd();
        rfile.Close();
        rfile.Delete();
    }
    return 0;
}