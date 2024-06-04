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
// Created by Maria on 24.04.2024 kolupanova@inr.ru
//

#include "MDargumentHandler.h"
#include "EventDisplay.h"
#include "PrintCrates.h"
#include "Hit.h"
#include <TF1.h>
#include <TSpectrum.h>
#include <TProfile.h>
#include <numeric>
#include <TROOT.h>
#include <thread>

namespace timing {
    struct Position {
        unsigned int _par1{0};
        unsigned int _par2{0};
        bool operator<(const Position &other) const {
            if (_par1 == other._par1)
                return _par2 < other._par2;
            return _par1 < other._par1;
        }

        bool operator==(const Position &other) const {
            return _par1 == other._par1 && _par2 == other._par2;
        }
    };
}
struct OutputPlots{
    TH2D* FebChannelVsTime = new TH2D("Channel vs Time", "Channel vs Time", SFGD_FEB_NCHANNELS , 0, SFGD_FEB_NCHANNELS, 75, -40, 10);
    TH2D* AsicChannelVsTime = new TH2D("Asic vs Time", "Asic vs Time", SFGD_CHANNELS_in_ASIC, 0, SFGD_CHANNELS_in_ASIC, 75, -40, 10);
    TH2D* FebSlotVsTime = new TH2D("Feb Slot vs Time", "Feb Slot  vs Time", SFGD_SLOT, 0, SFGD_SLOT, 75, -40, 10);
    TH2D* CrateVsTime = new TH2D("Crate vs Time", "Crate vs Time", SFGD_CRATES_NUM, 0, SFGD_CRATES_NUM, 75, -40, 10);
    TH2D* aFebChannelVsTime = new TH2D("a feb Channel vs Time", "a feb Channel vs Time", SFGD_FEB_NCHANNELS , 0, SFGD_FEB_NCHANNELS, 75, -40, 10);

    TH2D* Crate3FebSlotVsTime = new TH2D("Crate3 Feb Slot vs Time", "Feb Slot  vs Time", SFGD_SLOT, 0, SFGD_SLOT, 75, -40, 10);
    TH2D* Crate5FebSlotVsTime = new TH2D("Crate5 Feb Slot vs Time", "Feb Slot  vs Time", SFGD_SLOT, 0, SFGD_SLOT, 75, -40, 10);

};

void FebChannelCalibration(std::string& filename, Connection_Map& connectionMap,  OutputPlots& output){
    TFile *fileInput = new TFile((filename).c_str());
    if (!fileInput->IsOpen()) {
        cerr << "Can not open file " << filename << endl;
    }
    TTree *eventTree = fileInput->Get<TTree>("EventsHits");
    std::vector<Hit> *eventsHits = {nullptr};
    unsigned int eventNumber{0};
    try {
        if (eventTree->GetBranch("Events")) {
            eventTree->SetBranchAddress("Events", &eventsHits);
        }
        if (eventTree->GetBranch("EventNumber")) {
            eventTree->SetBranchAddress("EventNumber", &eventNumber);
        }
    } catch (...) {
        std::cerr << "Error in file " << filename << std::endl;
    }
    for (unsigned int j = 0; j < eventTree->GetEntries(); ++j) {
        eventTree->GetEntry(j);
        std::map<int, std::vector<Hit>> eventStructure;
        std::vector<Hit> selectedTrack;
        unsigned int tDigits = 50;
        if(eventsHits->size() > 150) {
            for (auto i = 0; i < eventsHits->size(); ++i) {
                auto time_slot = eventsHits->at(i)._timeFromGateStart / tDigits;
                if (eventStructure.find(time_slot) == eventStructure.end())
                    eventStructure.insert({(int)time_slot, {eventsHits->at(i)}});
                else {
                    eventStructure[time_slot].push_back(eventsHits->at(i));
                }
            }

            int calculated_entry = 0;
            std::map<timing::Position, std::vector<double>> hSide;
            std::map<unsigned int, std::vector<double>> hUpstream;
            for (int i = 0; i <= eventStructure.rbegin()->first; ++i) {
                if (hSide.empty() && hUpstream.empty()) {
                    sort(eventStructure[i].begin(), eventStructure[i].end());
                    if (eventStructure[i].size() > 150) {
                        unsigned int leftBorder = std::max(std::max(0, i - 1), calculated_entry);
                        unsigned int rightBorder = std::min(i + 1, eventStructure.rbegin()->first);
                        for (int j = leftBorder; j <= rightBorder; ++j) {
                            if (eventStructure.find(j) != eventStructure.end()) {
                                for (auto hit: eventStructure[j]) {
                                    auto cableLength = connectionMap.GetGlobalGeomPositionPtr(
                                            hit._global_ch_id)->cablelength_;
                                    auto globalChannel = hit._global_ch_id;
                                    if (!std::isnan(hit._x) && !std::isnan(hit._y) && hit._LY_PE ) {
                                        if (hUpstream.find(hit._y) == hUpstream.end())
                                            hUpstream[(unsigned int) hit._y] = {
                                                    (double)hit._timeFromGateStart + 1.0 / (SFGDFibersSpeedOfLight * 1.25) * hit._x};
                                        else
                                            hUpstream[(unsigned int) hit._y].push_back(
                                                    (double)hit._timeFromGateStart + 1.0 / (SFGDFibersSpeedOfLight * 1.25) * hit._x);
                                    }
                                    if (!std::isnan(hit._z) && !std::isnan(hit._y) && hit._LY_PE ) {
                                        if (hit._z < 95 ) {
                                            if (hSide.find({(unsigned int) (hit._y), globalChannel}) ==
                                                hSide.end()) {
                                                hSide[{(unsigned int) (hit._y), globalChannel}] =
                                                        {double(hit._timeFromGateStart) +
                                                         1.0 / (SFGDFibersSpeedOfLight * 1.25) * hit._z -
                                                         0.1 / (SFGDCableLengthSpeedOfLight * 1.25) * cableLength};
                                            } else
                                                hSide[{(unsigned int) (hit._y), globalChannel}].push_back(
                                                        {double(hit._timeFromGateStart) +
                                                         1 / (SFGDFibersSpeedOfLight * 1.25) * hit._z -
                                                         0.1 / (SFGDCableLengthSpeedOfLight * 1.25) * cableLength});
                                        }
                                    }
                                }
                            }
                        }
                        calculated_entry = i + 2;
                    }
                    std::map<unsigned int, std::vector<double>> timeWalkCalibration;
                    if (!hSide.empty() && !hUpstream.empty()) {
                        for (auto side: hSide) {
                            auto it = hUpstream.find(side.first._par1);
                            unsigned int T1{0};
                            if (it != hUpstream.end() && !it->second.empty()) {
                                T1 = (double) std::accumulate(it->second.begin(), it->second.end(), 0.0) /
                                     it->second.size();
                            }
                            auto T0 = (double) std::accumulate(side.second.begin(), side.second.end(), 0.0) /
                                      side.second.size();
                            if (T1 != 0) {
                                if (timeWalkCalibration.find(side.first._par2) == timeWalkCalibration.end())
                                    timeWalkCalibration[side.first._par2] = {T0 - T1};
                                else
                                    timeWalkCalibration[side.first._par2].push_back({T0 - T1});
                            }


                            for (auto timewalk: timeWalkCalibration) {
                                auto time =
                                        (double) std::accumulate(timewalk.second.begin(), timewalk.second.end(),
                                                                 0.0) / timewalk.second.size();

                                output.FebChannelVsTime->Fill(connectionMap.GetGlobalChannelPositionPtr(timewalk.first)->ch256_, time, 1);
                                output.AsicChannelVsTime->Fill(connectionMap.GetGlobalChannelPositionPtr(timewalk.first)->ch32_, time, 1);
                                output.FebSlotVsTime->Fill(connectionMap.GetGlobalChannelPositionPtr(timewalk.first)->slot_, time, 1);
                                output.CrateVsTime->Fill(connectionMap.GetGlobalChannelPositionPtr(timewalk.first)->crate_, time, 1);
                                if (connectionMap.GetGlobalChannelPositionPtr(timewalk.first)->crate_ == 5) {
                                    output.Crate5FebSlotVsTime->Fill(connectionMap.GetGlobalChannelPositionPtr(timewalk.first)->slot_, time, 1);
                                    if (connectionMap.GetGlobalChannelPositionPtr(timewalk.first)->slot_ == 0)
                                        output.aFebChannelVsTime->Fill(connectionMap.GetGlobalChannelPositionPtr(timewalk.first)->ch256_, time,1);
                                } else {
                                    output.Crate3FebSlotVsTime->Fill(connectionMap.GetGlobalChannelPositionPtr(timewalk.first)->slot_, time, 1);
                                }
                            }
                        }
                    }
                    hSide.clear();
                    hUpstream.clear();
                }
            }
        }

    }
    fileInput->Close();
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
    string txtFileOutput = rootFileOutput + "febChannelCalibration.txt";
    rootFileOutput += "febChannelCalibration.root";
    std::cout << "ROOT output: " << rootFileOutput << endl;
    std::cout << "TXT output:  " << txtFileOutput << endl;

    std::ofstream fout(txtFileOutput.c_str());

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

    ROOT::EnableThreadSafety();
    OutputPlots output;

    size_t  threadsNum = std::fmin(UNPACKING_PTHREADS,vFileNames.size());
    for (auto file = 0; file < vFileNames.size(); file+=threadsNum) {
        threadsNum = std::fmax(1, std::min(threadsNum, vFileNames.size() - file));
        std::thread th[threadsNum];
        for (int i = 0; i < threadsNum; ++i) {
            th[i] = std::thread(&FebChannelCalibration,std::ref(vFileNames.at(file)), std::ref(connectionMap), std::ref(output));
        }
        for (int i = 0; i < threadsNum; ++i) {
            th[i].join();
        }
    }

    TFile* wfile = new TFile(rootFileOutput.c_str(), "recreate");
    wfile->cd();
    output.FebChannelVsTime->SetStats(0);
    output.FebChannelVsTime->Write();
    TProfile *prof = output.FebChannelVsTime->ProfileX();
    prof->GetXaxis()->SetTitle("FEB channel");
    prof->GetYaxis()->SetTitle("Time [1.25 ns] ");
    auto fit = new TF1("fit", "[0]");
    prof->Fit(fit);
    double avr_time =  fit->GetParameter(0);
    for (int ch = 0; ch < SFGD_FEB_NCHANNELS; ++ch) {
        fout << ch << " " << prof->GetBinContent(ch + 1) - avr_time << std::endl;
    }
    prof->Write();
    prof->Delete();

    output.AsicChannelVsTime->Write();
    output.FebSlotVsTime->Write();
    output.CrateVsTime->Write();
    output.aFebChannelVsTime->Write();
    output.Crate3FebSlotVsTime->Write();
    output.Crate5FebSlotVsTime->Write();

    prof = output.Crate3FebSlotVsTime->ProfileX();
    prof->GetXaxis()->SetTitle("FEB channel");
    prof->GetYaxis()->SetTitle("Time [1.25 ns] ");
    prof->Fit(fit);
    prof->Write();

    prof = output.Crate5FebSlotVsTime->ProfileX();
    prof->GetXaxis()->SetTitle("FEB channel");
    prof->GetYaxis()->SetTitle("Time [1.25 ns] ");
    prof->Fit(fit);
    prof->Write();

    auto f = new TF1("Gaus fit", "gaus");
    TObjArray aSlices;
    output.FebChannelVsTime->FitSlicesY(f, 0, -1, 10, "Q", &aSlices);
    output.FebChannelVsTime->Delete();
    auto m = (TH1D *) aSlices[1];

    m->Fit(fit);
    avr_time =  fit->GetParameter(0);
    double avr_error = fit->GetParError(0);
    std::cout << 1e-1 / (std::abs(avr_time) * 1.25) << " +/- " << avr_error * 1e-1 / (avr_time * avr_time * 1.25) << std::endl;
    m->Write();
    m->Delete();
    prof->Write();
    prof->Delete();
    wfile->Close();
    wfile->Delete();
    delete f;
    return 0;
}