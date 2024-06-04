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
    rootFileOutput += "CableCalibration.root";
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

    TH2D* cableLengthVsTime = new TH2D("cableLength vs Time","cableLength vs Time",  100,(int)700,(int)2300, 2500,-100,150);

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
                std::map<timing::Position, std::vector<double>> cableLengthMap;
                std::map<unsigned int, std::vector<unsigned int>> hUpstream;
                for (int i = 0; i <= eventStructure.rbegin()->first; ++i) {
                    if (cableLengthMap.empty() && hUpstream.empty()) {
                        sort(eventStructure[i].begin(), eventStructure[i].end());
                        if (eventStructure[i].size() > 150) {
                            unsigned int leftBorder = std::max(std::max(0, i - 1), calculated_entry);
                            unsigned int rightBorder = std::min(i + 1, eventStructure.rbegin()->first);
                            for (int j = leftBorder; j <= rightBorder; ++j) {
                                if (eventStructure.find(j) != eventStructure.end()) {
                                    for (auto hit: eventStructure[j]) {
                                        auto cableLength = connectionMap.GetGlobalGeomPositionPtr(
                                                hit._global_ch_id)->cablelength_;
                                        if (!std::isnan(hit._x) && !std::isnan(hit._y) && !std::isnan(hit._LY_PE)) {
                                            if (hit._x > 88 && hit._x < 104 && hit._LY_PE > 25) {

                                                if (hUpstream.find(hit._y) == hUpstream.end())
                                                    hUpstream[(unsigned int) hit._y] = {hit._timeFromGateStart};
                                                else
                                                    hUpstream[(unsigned int) hit._y].push_back(hit._timeFromGateStart);
                                            }
                                        }
                                        if (!std::isnan(hit._z) && !std::isnan(hit._y)) {
                                            if ((hit._z < 135 || (hit._z >= 143 && hit._z < 151) ||
                                                 (hit._z >= 159 && hit._z < 167)) && hit._LY_PE > 25) {
                                                if (cableLengthMap.find({(unsigned int) (hit._y), cableLength}) ==
                                                    cableLengthMap.end()) {
                                                    cableLengthMap[{(unsigned int) (hit._y), cableLength}] = {
                                                            double(hit._timeFromGateStart) +
                                                            1 / (SFGDFibersSpeedOfLight * 1.25) * hit._z};
                                                } else
                                                    cableLengthMap[{(unsigned int) (hit._y), cableLength}].push_back(
                                                            {double(hit._timeFromGateStart) +
                                                             1 / (SFGDFibersSpeedOfLight * 1.25) * hit._z});
                                            }
                                        }
                                    }
                                }
                            }
                            calculated_entry = i + 2;
                        }
                        std::map<unsigned int, std::vector<double>> cableLengthCalibration;
                        if (!cableLengthMap.empty() && !hUpstream.empty()) {
                            for (auto cableMap: cableLengthMap) {
                                auto it = hUpstream.find(cableMap.first._par1);
                                unsigned int T0{0};
                                if (it != hUpstream.end() && !it->second.empty()) {
                                    T0 = (double) std::accumulate(it->second.begin(), it->second.end(), 0.0) /
                                         it->second.size();
                                }
                                auto T1 =
                                        (double) std::accumulate(cableMap.second.begin(), cableMap.second.end(), 0.0) /
                                        cableMap.second.size();
                                if (T0 != 0) {
                                    if (cableLengthCalibration.find(cableMap.first._par2) ==
                                        cableLengthCalibration.end())
                                        cableLengthCalibration[cableMap.first._par2] = {T1 - T0};
                                    else
                                        cableLengthCalibration[cableMap.first._par2].push_back({T1 - T0});
                                }


                                for (auto length: cableLengthCalibration) {
                                    auto time =
                                            (double) std::accumulate(length.second.begin(), length.second.end(), 0.0) /
                                            length.second.size();

                                    cableLengthVsTime->Fill(length.first, time, 1);
                                }
                            }
                        }
                    }
                    cableLengthMap.clear();
                    hUpstream.clear();
                }
            }
        }
        fileInput->Close();
    }

    TFile* wfile = new TFile(rootFileOutput.c_str(), "recreate");
    wfile->cd();
    cableLengthVsTime->SetStats(0);
    cableLengthVsTime->Write();
    TProfile *prof = cableLengthVsTime->ProfileX();
    prof->GetXaxis()->SetTitle("Cable Length [mm]");
    prof->GetYaxis()->SetTitle("Time [1.25 ns] ");
    auto f = new TF1("Gaus fit", "gaus");
    TObjArray aSlices;
    cableLengthVsTime->FitSlicesY(f, 0, -1, 10, "Q", &aSlices);
    cableLengthVsTime->Delete();
    auto m = (TH1D *) aSlices[1];
    auto fit = new TF1("fit", "[0]*x+[1]");
    m->Fit(fit);
    double angle =  fit->GetParameter(0);
    double angle_error = fit->GetParError(0);
    std::cout << 1e-1 / (std::abs(angle) * 1.25) << " +/- "<< angle_error * 1e-1/ (angle * angle * 1.25)<< " cm/ns"<<std::endl;
    m->Write();
    m->Delete();
    prof->Write();
    prof->Delete();
    wfile->Close();
    wfile->Delete();
    delete f;
    return 0;
}