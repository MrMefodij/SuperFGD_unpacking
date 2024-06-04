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
// Created by Maria on 27.04.2024 kolupanova@inr.ru
//

#include "MDargumentHandler.h"
#include "EventDisplay.h"
#include "PrintCrates.h"
#include "Hit.h"
#include <TF1.h>
#include <TSpectrum.h>
#include <TProfile.h>
#include <numeric>
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

    std::map<unsigned int, double> febTimeChannel;
    std::string febTimeCalib = "../timing_calibration/febChannelCalibration.txt";
    std::ifstream febChannel(febTimeCalib.c_str());

    unsigned int chCalib = 0;
    double chTime = 0;
    while (!febChannel.eof()){
        febChannel >> chCalib >> chTime;
        febTimeChannel[chCalib] = chTime;
    }
    rootFileOutput += "TimeWalkCalibration.root";
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

    TH2D* timeWalk = new TH2D("p.e. vs Time","p.e. vs Time",  500,(int)0,(int)500, 500,-50,50);

    for (auto file = 0; file < vFileNames.size(); ++file) {
        std::string filename = vFileNames.at(file);
        TFile *fileInput = new TFile((filename).c_str());
        if (!fileInput->IsOpen()) {
            cerr << "Can not open file " << filename << endl;
            return 1;
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
            std::cerr << "Error in file " << vFileNames[file] << std::endl;
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
                        eventStructure.insert({time_slot, {eventsHits->at(i)}});
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
                                        auto ch = hit._channel_id;
                                        if (!std::isnan(hit._x) && !std::isnan(hit._y) && !std::isnan(hit._LY_PE)) {
                                                if (hUpstream.find(hit._y) == hUpstream.end())
                                                    hUpstream[(unsigned int) hit._y] = {(double)hit._timeFromGateStart +
                                                                                        1 / (SFGDFibersSpeedOfLight * 1.25) * hit._x - febTimeChannel[ch]};
                                                else
                                                    hUpstream[(unsigned int) hit._y].push_back((double)hit._timeFromGateStart+
                                                                                               1 / (SFGDFibersSpeedOfLight * 1.25) * hit._x -febTimeChannel[ch]);
                                        }
                                        if (!std::isnan(hit._z) && !std::isnan(hit._y)) {
                                            if (hit._z < 95 ) {
                                                if (hSide.find({(unsigned int) (hit._y), cableLength}) ==
                                                    hSide.end()) {
                                                    hSide[{(unsigned int) (hit._y), (unsigned int) (hit._LY_PE)}] =
                                                            {double(hit._timeFromGateStart) +
                                                             1 / (SFGDFibersSpeedOfLight * 1.25) * hit._z -
                                                             1e-1 / (SFGDCableLengthSpeedOfLight * 1.25) * cableLength - febTimeChannel[ch]};
                                                } else
                                                    hSide[{(unsigned int) (hit._y),
                                                           (unsigned int) (hit._LY_PE)}].push_back(
                                                            {double(hit._timeFromGateStart) +
                                                             1 / (SFGDFibersSpeedOfLight * 1.25) * hit._z -
                                                             1e-1 / (SFGDCableLengthSpeedOfLight * 1.25) * cableLength - febTimeChannel[ch]});
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

                                    timeWalk->Fill(timewalk.first, time, 1);
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

    TFile* wfile = new TFile(rootFileOutput.c_str(), "recreate");
    wfile->cd();
    timeWalk->GetXaxis()->SetTitle("Light yield [p.e.]");
    timeWalk->GetYaxis()->SetTitle("Time [1.25 ns] ");
    timeWalk->SetStats(0);
    timeWalk->Write();
    TProfile *prof = timeWalk->ProfileX();
    prof->GetXaxis()->SetTitle("Light yield [p.e.]");
    prof->GetYaxis()->SetTitle("Time [1.25 ns] ");

    auto walkFit1 = new TF1("walkFit1", "[0] - [1] * log( [2] - x * [3] - x * x *[4])",6,125);

    walkFit1->SetParLimits(0,1,100);
    walkFit1->SetParName(0, "T0");
    walkFit1->SetParLimits(1,1,4);
    walkFit1->SetParName(1, "Tau");
    walkFit1->SetParName(2, "C1");
    walkFit1->SetParName(3, "LY_MIN");
    walkFit1->SetParName(4, "LY_MIN_2");

    auto walkFit2 = new TF1("walkFit2", "[0] + [1]*x",120,400);
    walkFit2->SetParName(0, "C2");
    walkFit2->SetParName(1, "P2");

    auto walkFit0 = new TF1("walkFit0", "[0] + [1]*x",0,8);
    walkFit0->SetParName(0, "C0");
    walkFit0->SetParName(1, "P0");

    prof->Fit("walkFit0","MER+", "");
    prof->Fit("walkFit1","MER+", "");
    prof->Fit("walkFit2","MER+", "");

    prof->Draw();
    auto f = new TF1("Gaus fit", "gaus");
    TObjArray aSlices;
    timeWalk->FitSlicesY(f, 0, -1, 10, "Q", &aSlices);
    timeWalk->Delete();
    auto m = (TH1D *) aSlices[1];
    m->GetXaxis()->SetTitle("Light yield [p.e.]");
    m->GetYaxis()->SetTitle("Time [1.25 ns] ");
    m->Fit("walkFit0","MER+", "");
    m->Fit("walkFit1","MER+", "");
    m->Fit("walkFit2","MER+", "");
    m->Draw();
    m->Write("p.e. vs Time Gaus fit");
    m->Delete();
    prof->Write();
    prof->Delete();
    wfile->Close();
    wfile->Delete();
    delete f;
    return 0;
}
