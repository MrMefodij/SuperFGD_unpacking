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
// Created by Maria on 15.05.2024 kolupanova@inr.ru
//

#include "MDargumentHandler.h"
#include "EventDisplay.h"
#include "PrintCrates.h"
#include "Hit.h"
#include <TF1.h>
#include <TSpectrum.h>
#include <TProfile.h>
#include <numeric>
#include <thread>

std::mutex mtx;

double TimeWalkCalibration(double LY,
                           double C0 = defaultC0, double P0 = defaultP0,
                           double T0 = defaultT0, double tau = defaultTau, double C =defaultC, double LY_min = defaultLY_MIN, double LY_min_2 = defaultLY_MIN2,
                           double C2 = defaultC2, double P2 = defaultP2){
    if (LY < 10) {
        return C0 + P0*LY;
    } else if (LY < 126) {
        return T0 - tau * log(C - LY_min * LY -  LY_min_2 * LY * LY);
    } else {
        return C2 + P2*LY;
    }
}

namespace timing {
    template<typename T>
    struct Position {
        T _par1{0}; // Light yield p.e.
        T _par2{0}; // time
        int _par3{0}; // x or z
        int _par4{0}; // y
        bool operator<(const Position &other) const {
            if (_par4 == other._par4)
                return _par3 < other._par3;
            return _par4 < other._par4;
        }

        bool operator==(const Position &other) const {
            return _par3 == other._par3 && _par4 == other._par4;
        }

    };
}

void TimeResolution(std::string filename, TTree *eventTree, std::vector<Hit> *eventsHits, const Connection_Map& connectionMap,TH2D* timeResolutionStudy, TH1D* timeDistributionHits, TH1D* timeDistributionHitsPECut, const std::map<unsigned int, double>& febTimeChannel, TH1D* timeDistributionHitsSide, TH1D* timeDistributionHitsUp,TH1D* timeDistributionHitsTop, TH2D* timeResolutionStudySide, TH2D* timeResolutionStudyUp, TH2D* timeResolutionStudyTop/*TFile* wfile*/){
    for (unsigned int elem = 0; elem < eventTree->GetEntries(); ++elem) {
        eventTree->GetEntry(elem);
        std::map<int, std::vector<Hit>> eventStructure;
        std::vector<Hit> selectedTrack;
        unsigned int tDigits = 50;
        if (eventsHits->size() > 150) {
            for (auto i = 0; i < eventsHits->size(); ++i) {
                int time_slot = eventsHits->at(i)._timeFromGateStart / tDigits;
                if (eventStructure.find(time_slot) == eventStructure.end())
                    eventStructure.insert({time_slot, {eventsHits->at(i)}});
                else {
                    eventStructure[time_slot].push_back(eventsHits->at(i));
                }
            }

            int calculated_entry = 0;
            for (int i = 0; i <= eventStructure.rbegin()->first; ++i) {
                std::vector<timing::Position<double>> hSide;
                std::vector<timing::Position<double>> hUpstream;
                std::vector<timing::Position<double>> hTop;
//                TH2F *event_XY = new TH2F(("Event_XY_"+ to_string(i)).c_str(), "", Y_SIZE, 0, Y_SIZE, X_SIZE, 0, X_SIZE);
//                TH2F *event_YZ = new TH2F(("Event_YZ_"+ to_string(i)).c_str(),"", Z_SIZE, 0, Z_SIZE, Y_SIZE, 0, Y_SIZE);
//                TH2F *event_XZ = new TH2F(("Event_XZ_"+ to_string(i)).c_str(), "", Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);
//                TH2F *event_XY_cut = new TH2F(("Event_XY_cut_"+ to_string(i)).c_str(), "", Y_SIZE, 0, Y_SIZE, X_SIZE, 0, X_SIZE);
//                TH2F *event_YZ_cut = new TH2F(("Event_YZ_cut_"+ to_string(i)).c_str(),"", Z_SIZE, 0, Z_SIZE, Y_SIZE, 0, Y_SIZE);
//                TH2F *event_XZ_cut = new TH2F(("Event_XZ_cut_"+ to_string(i)).c_str(), "", Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);
                sort(eventStructure[i].begin(), eventStructure[i].end());
                if (eventStructure[i].size() > 150) {
                    unsigned int leftBorder = std::max(std::max(0, i - 1), calculated_entry);
                    unsigned int rightBorder = std::min(i + 1, eventStructure.rbegin()->first);
                    for (int j = leftBorder; j <= rightBorder; ++j) {
                        if (eventStructure.find(j) != eventStructure.end()) {
                            for (auto hit: eventStructure[j]) {
                                if (hit._LY_PE != NAN  && hit._LY_PE > 3) {
                                    auto cableLength = connectionMap.GetGlobalGeomPositionPtr(
                                            hit._global_ch_id)->cablelength_;
                                    auto ch = hit._channel_id;
                                    auto time = double(hit._timeFromGateStart) -
                                                1e-1 / (SFGDCableLengthSpeedOfLight * 1.25) * cableLength
                                                    - TimeWalkCalibration(hit._LY_PE) - febTimeChannel.at(ch);

                                    if (!std::isnan(hit._z) && !std::isnan(hit._y)) {
                                        if ((hit._z < 135 || (hit._z >= 143 && hit._z < 151) ||
                                             (hit._z >= 159 && hit._z < 167))) {
                                            hSide.push_back({hit._LY_PE, time, int(hit._z), int(Y_SIZE - hit._y - 1)});
//                                            event_YZ->Fill(int(hit._z),  int(hit._y), hit._LY_PE);
                                        }
                                    }
                                    if (!std::isnan(hit._z) && !std::isnan(hit._x)) {
                                        hTop.push_back({hit._LY_PE, time, int(hit._x), int(hit._z)});
//                                        event_XZ->Fill(int(hit._z), int(hit._x), hit._LY_PE);
                                    }
                                    if (!std::isnan(hit._y) && !std::isnan(hit._x)) {
                                        hUpstream.push_back({hit._LY_PE, time, int(hit._x), int(Y_SIZE - hit._y - 1)});
//                                        event_XY->Fill(int(hit._y), int(hit._x), hit._LY_PE);
                                    }
                                }
                            }
                        }
                    }
                    calculated_entry = i + 2;
                }
                std::map<double, double> timeResolution;
                std::map<double, double> timeResolutionSide;
                std::map<double, double> timeResolutionTop;
                std::map<double, double> timeResolutionUp;
                std::map<double, double> timeResolutionPECut;
                std::pair<unsigned int, double> timeDistribution = {0.0, 0.0};
                std::sort(hSide.begin(), hSide.end());
                std::sort(hUpstream.begin(), hUpstream.end());
                if(hSide.size() > 40 && hUpstream.size() > 40 && (hUpstream.back()._par4 - hUpstream[0]._par4) > 50 && (hSide.back()._par4 - hSide[0]._par4) > 50 && std::abs(hUpstream.back()._par3 - hUpstream[0]._par3) > 10 && std::abs(hSide.back()._par3 - hSide[0]._par3) > 10 && std::abs(hUpstream.back()._par3 - hUpstream[0]._par3) < 40 && std::abs(hSide.back()._par3 - hSide[0]._par3) < 40 ) {

                    for (auto side: hSide) {
                        for (auto up: hUpstream) {
                            if (up._par4 == side._par4) {
                                auto iterUpstream = std::find_if(hUpstream.begin(), hUpstream.end(),
                                                                 [&up](const timing::Position<double> value) {
                                                                     return value._par4 == up._par4 + 1 &&
                                                                            (value._par3 == up._par3 + 1 ||
                                                                             value._par3 == up._par3 - 1 || value._par3 == up._par3);
                                                                 });
                                auto iterSide = std::find_if(hSide.begin(), hSide.end(),
                                                             [&side](const timing::Position<double> value) {
                                                                 return  value._par4 == side._par4 + 1 &&
                                                                        (value._par3 == side._par3 + 1 ||
                                                                         value._par3 == side._par3 - 1 || value._par3 == side._par3);
                                                             });
                                auto iterTop = std::find_if(hTop.begin(), hTop.end(),
                                                            [&side, &up](const timing::Position<double> value) {
                                                                return value._par4 == side._par3  && value._par3 == up._par3;});
                                if (iterSide != hSide.end() && iterUpstream != hSide.end() && iterTop != hTop.end() ) {
//                                    event_XY_cut->Fill(Y_SIZE -int(up._par4), int(up._par3), up._par1);
//                                    event_XZ_cut->Fill(int(iterTop->_par4), int(iterTop->_par3), iterTop->_par1);
//                                    event_YZ_cut->Fill( int(side._par3), Y_SIZE - int(side._par4), side._par1);
                                    double timeSide;
                                    if (side._par3 < 95) {
                                        timeSide = side._par2 - 1 / (SFGDFibersSpeedOfLight * 1.25) * up._par3;
                                    } else {
                                        timeSide =
                                                side._par2 - 1 / (SFGDFibersSpeedOfLight * 1.25) * (X_SIZE - up._par3);
                                    }
                                    double timeUp = up._par2 - 1 / (SFGDFibersSpeedOfLight * 1.25) * side._par3;
                                    auto iterTopTimeSide = std::find_if(hSide.begin(), hSide.end(),
                                                                [&side](const timing::Position<double> value) {
                                                                    return value._par4 == side._par3;});
                                    auto iterTopTimeUp = std::find_if(hUpstream.begin(), hUpstream.end(),
                                                                        [&up](const timing::Position<double> value) {
                                                                            return value._par3 == up._par3;});
                                    double timeTop = iterTop->_par2 - 1 / (SFGDFibersSpeedOfLight * 1.25) * std::min(iterTopTimeSide->_par4,iterTopTimeUp->_par4);
                                    timeResolutionSide.insert({side._par1,timeSide});
                                    timeResolutionUp.insert({up._par1,timeUp});
                                    timeResolutionTop.insert({iterTop->_par1,timeTop});
                                    timeDistribution.first += 3;
                                    timeDistribution.second += timeSide + timeUp + timeTop;
                                    timeResolution.insert({up._par1 + side._par1 + iterTop->_par1, (timeSide + timeUp + timeTop) / 3});
                                    if (up._par1 > 40 && side._par1 > 40)
                                        timeResolutionPECut.insert({up._par1 + side._par1 + iterTop->_par1, (timeSide + timeUp +  timeTop) / 3});
                                }
                            }
                        }
                    }
                }
                if (timeResolution.size() > 100) {
                    auto T0 =timeDistribution.second / timeDistribution.first;
                    if (abs(T0 - timeResolution.begin()->second) < 150) {
//                        PrintCrates pc;
//                        pc.Write2DMapsCanvas(*wfile, *event_YZ, *event_XZ, *event_XY, "Entries_" + std::to_string(i));
//                        pc.Write2DMapsCanvas(*wfile, *event_YZ_cut, *event_XZ_cut, *event_XY_cut, "Entries_cut" + std::to_string(i));
                        for (auto calibration: timeResolution) {
                            std::lock_guard<std::mutex> guard(mtx);
                            timeResolutionStudy->Fill(calibration.first, calibration.second - T0, 1);
                            timeDistributionHits->Fill(calibration.second - T0);
                        }
                        for (auto calibration: timeResolutionSide) {
                            std::lock_guard<std::mutex> guard(mtx);
                            timeResolutionStudySide->Fill(calibration.first, calibration.second - T0, 1);
                            timeDistributionHitsSide->Fill(calibration.second - T0);
                        }
                        for (auto calibration: timeResolutionUp) {
                            std::lock_guard<std::mutex> guard(mtx);
                            timeResolutionStudyUp->Fill(calibration.first, calibration.second - T0, 1);
                            timeDistributionHitsUp->Fill(calibration.second - T0);
                        }
                        for (auto calibration: timeResolutionTop) {
                            std::lock_guard<std::mutex> guard(mtx);
                            timeResolutionStudyTop->Fill(calibration.first, calibration.second - T0, 1);
                            timeDistributionHitsTop->Fill(calibration.second - T0);
                        }
                        for (auto calibration: timeResolutionPECut) {
                            std::lock_guard<std::mutex> guard(mtx);
                            timeDistributionHitsPECut->Fill(calibration.second - T0);
                        }
                    }
                }
//                delete event_XY;
//                delete event_YZ;
//                delete event_XZ;
//                delete event_XY_cut;
//                delete event_YZ_cut;
//                delete event_XZ_cut;
            }
        }
    }
}


int main( int argc, char **argv ) {
    std::string stringBuf;
    // The following shows how to use the MDargumentHandler class
    // to deal with the main arguments
    // Define the arguments
    MDargumentHandler argh("Example of unpacking application.");
    argh.Init();
    if (argh.ProcessArguments(argc, argv)) {
        argh.Usage();
        return -1;
    }
    if (argh.GetValue("help")) {
        argh.Usage();
        return 0;
    }

    if (argh.GetMode() == "f") {
        if (argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else if (argh.GetMode() == "d") {
        if (argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else {
        return -1;
    }
    vector<string> vFileNames = argh.GetDataFiles(stringBuf, ".root", "EventStructure");
    std::sort(vFileNames.begin(), vFileNames.end());

    if (vFileNames.empty()) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    size_t pos = stringBuf.find("EventStructure");
    std::string rootFileOutput;
    if(pos != string::npos)
        rootFileOutput = stringBuf.substr(0, pos);
    else{
        rootFileOutput = stringBuf;
    }
    rootFileOutput += "TimeResolutionThreads.root";
    std::cout << rootFileOutput << std::endl;
    std::string mapFile = "../connection_map/SFG_Geometry_Map_v18.csv";
    if(getenv("UNPACKING_ROOT")!= nullptr){
        mapFile = (string)getenv("UNPACKING_ROOT") + "/connection_map/SFG_Geometry_Map_v18.csv";
    }

    Connection_Map connectionMap(mapFile);
    try {
        connectionMap.Init();
    } catch (const exception& e) {
        std::cerr << "Unable to open file " << mapFile << std::endl;
        exit(1);
    }

    std::map<unsigned int, double> febTimeChannel;
    std::string febTimeCalib = "../timing_calibration/febChannelCalibration.txt";
    std::ifstream febChannel(febTimeCalib.c_str());

    unsigned int chCalib = 0;
    double chTime = 0;
    while (!febChannel.eof()){
        febChannel >> chCalib >> chTime;
        febTimeChannel[chCalib] = chTime;
    }
    TH2D* timeResolutionStudy = new TH2D("p.e. vs Time","p.e. vs Time",  500,0,500, 1000,-50,50);
    TH2D* timeResolutionStudySide = new TH2D("p.e. vs Time(Side)","p.e. vs Time(Side)",  500,0,500, 1000,-50,50);
    TH2D* timeResolutionStudyUp = new TH2D("p.e. vs Time (Up)","p.e. vs Time(Up)",  500,0,500, 1000,-50,50);
    TH2D* timeResolutionStudyTop = new TH2D("p.e. vs Time (Top)","p.e. vs Time(Top)",  500,0,500, 1000,-50,50);
    TH1D* timeDistributionHitsSide = new TH1D("Time distribution of hits after calibration (Side)", "Time distribution of hits after calibration (Side)", 600, -15, 15);
    TH1D* timeDistributionHitsUp = new TH1D("Time distribution of hits after calibration (Upstream)", "Time distribution of hits after calibration (Upstream)", 600, -15, 15);
    TH1D* timeDistributionHitsTop = new TH1D("Time distribution of hits after calibration (Top)", "Time distribution of hits after calibration (Top)", 600, -15, 15);
    TH1D* timeDistributionHits = new TH1D("Time distribution of hits after calibration", "Time distribution of hits after calibration", 600, -15, 15);
    TH1D* timeDistributionHitsPECut = new TH1D("Time distribution of hits after calibration, p.e. for each fiber > 40", "Time distribution of hits after calibration, p.e. for each fiber > 40", 500, -15, 15);
    size_t  threadsNum = std::fmin(14,vFileNames.size());
//    TFile* wfile = new TFile(rootFileOutput.c_str(), "recreate");
    for(auto j = 0; j < vFileNames.size(); j+=threadsNum) {
        threadsNum = std::fmax(1, std::min(threadsNum, vFileNames.size() - j));
        std::thread th[threadsNum];
        TFile *fileInput[threadsNum];
        TTree *eventTree[threadsNum];
        std::vector<std::vector<Hit> *> eventsHits(threadsNum, {nullptr});
        unsigned int eventNumber[threadsNum];
        for (int i = 0; i < threadsNum; ++i) {
            fileInput[i] = new TFile(vFileNames[i + j].c_str());
            {
                std::lock_guard<std::mutex> guard(mtx);
                std::cout << vFileNames[i + j] << std::endl;
                if (!fileInput[i]->IsOpen()) {
                    std::cerr << "Can not open file " << vFileNames[i + j] << std::endl;
                    return 1;
                }
                eventTree[i] = fileInput[i]->Get<TTree>("EventsHits");
                try {
                    if (eventTree[i]->GetBranch("Events")) {
                        eventTree[i]->SetBranchAddress("Events", &eventsHits[i]);
                    }
                    if (eventTree[i]->GetBranch("EventNumber")) {
                        eventTree[i]->SetBranchAddress("EventNumber", &eventNumber[i]);
                    }
                } catch (...) {
                    std::cerr << "Error in file " << vFileNames[i + j] << std::endl;
                }
            }
            th[i] = std::thread(&TimeResolution,  vFileNames[i + j], std::ref(eventTree[i]), std::ref(eventsHits[i]), std::ref(connectionMap), std::ref(timeResolutionStudy), std::ref(timeDistributionHits), std::ref(timeDistributionHitsPECut), std::ref(febTimeChannel), std::ref(timeDistributionHitsSide), std::ref(timeDistributionHitsUp), std::ref(timeDistributionHitsTop), std::ref(timeResolutionStudySide), std::ref(timeResolutionStudyUp), std::ref(timeResolutionStudyTop));

        }
        for (int i = 0; i < threadsNum; ++i) {
            th[i].join();
            eventTree[i]->Delete();
            fileInput[i]->Close();
            fileInput[i]->Delete();
        }
    }
    TFile* wfile = new TFile(rootFileOutput.c_str(), "recreate");
    wfile->cd();
    TF1 *fit = new TF1("fit", "gaus");
    timeDistributionHits->Fit("fit", "Q", "");
    timeDistributionHits->GetXaxis()->SetTitle("Time [1.25 ns] ");
    timeDistributionHits->GetYaxis()->SetTitle("N");
    timeDistributionHits->Write();
    timeDistributionHits->Delete();

    timeDistributionHitsSide->Fit("fit", "Q", "");
    timeDistributionHitsSide->GetXaxis()->SetTitle("Time [1.25 ns] ");
    timeDistributionHitsSide->GetYaxis()->SetTitle("N");
    timeDistributionHitsSide->Write();
    timeDistributionHitsSide->Delete();

    timeDistributionHitsUp->Fit("fit", "Q", "");
    timeDistributionHitsUp->GetXaxis()->SetTitle("Time [1.25 ns] ");
    timeDistributionHitsUp->GetYaxis()->SetTitle("N");
    timeDistributionHitsUp->Write();
    timeDistributionHitsUp->Delete();

    timeDistributionHitsTop->Fit("fit", "Q", "");
    timeDistributionHitsTop->GetXaxis()->SetTitle("Time [1.25 ns] ");
    timeDistributionHitsTop->GetYaxis()->SetTitle("N");
    timeDistributionHitsTop->Write();
    timeDistributionHitsTop->Delete();

    timeDistributionHitsPECut->Fit("fit", "Q", "");
    timeDistributionHitsPECut->GetXaxis()->SetTitle("Time [1.25 ns] ");
    timeDistributionHitsPECut->GetYaxis()->SetTitle("N");
    timeDistributionHitsPECut->Write();
    timeDistributionHitsPECut->Delete();
    delete fit;
    timeResolutionStudy->SetStats(0);
    timeResolutionStudy->Write();
    TProfile *prof = timeResolutionStudy->ProfileX();
    prof->GetXaxis()->SetTitle("Light yield [p.e.]");
    prof->GetYaxis()->SetTitle("Time [1.25 ns] ");
    auto f = new TF1("Gaus fit", "gaus");
    TObjArray aSlices;
    timeResolutionStudy->FitSlicesY(f, 0, -1, 10, "Q", &aSlices);
    timeResolutionStudy->Delete();
    auto m = (TH1D *) aSlices[1];
    m->Write();
    m->Clear();
    prof->Write();
    prof->Delete();

    TObjArray aSlicesSide;
    timeResolutionStudySide->FitSlicesY(f, 0, -1, 10, "Q", &aSlicesSide);
    timeResolutionStudySide->Write();
    timeResolutionStudySide->Delete();
    m = (TH1D *) aSlicesSide[1];
    m->Write();
    m->Clear();

    TObjArray aSlicesUp;
    timeResolutionStudyUp->FitSlicesY(f, 0, -1, 10, "Q", &aSlicesUp);
    timeResolutionStudyUp->Write();
    timeResolutionStudyUp->Delete();
    m = (TH1D *) aSlicesUp[1];
    m->Write();
    m->Clear();

    TObjArray aSlicesSideTop;
    timeResolutionStudyTop->FitSlicesY(f, 0, -1, 10, "Q", &aSlicesSideTop);
    timeResolutionStudyTop->Write();
    timeResolutionStudyTop->Delete();
    m = (TH1D *) aSlicesSideTop[1];
    m->Write();
    m->Delete();
    wfile->Close();
    wfile->Delete();
    delete f;
    return 0;
}
