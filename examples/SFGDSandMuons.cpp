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
// Created by Maria on 29.02.2024 kolupanova@inr.ru
//
#include "MDargumentHandler.h"
#include "EventDisplay.h"
#include <TH3F.h>
#include <TF1.h>
#include "PrintCrates.h"
#include "Hit.h"

namespace straight {
    struct Position {
        double _par_1;
        double _par_2;

        bool operator<(const Position &other) const {
            if (_par_1 == other._par_1) {
                return _par_2 < other._par_2;
            }
            return _par_1 < other._par_1;
        }

        bool const operator==(const Position &other) const {
            return _par_1 == other._par_1 && _par_2 == other._par_2;
        }
    };
}


void plot_3D_from_2D(TDirectory* dir,TH3F*& hist3D,TH2F*& hXZ,TH2F*& hYZ,TH2F*& hXY, TH1F*& hT) {

    /// Open the ROOT file containing 2D histograms

    /// Check if the file is opened successfully


    string dirName = dir->GetName();
    string histNameXY = "Event_pe_"+dirName+"_XY";
    string histNameYZ = "Event_pe_"+dirName+"_YZ";
    string histNameXZ = "Event_pe_"+dirName+"_XZ";
    string histNameT  = "EventTime_"+dirName;

    /// Get the TH2F histograms from the file
    hXY = dynamic_cast<TH2F*>(dir->Get(histNameXY.c_str()));
    hYZ = dynamic_cast<TH2F*>(dir->Get(histNameYZ.c_str()));
    hXZ = dynamic_cast<TH2F*>(dir->Get(histNameXZ.c_str()));
    hT  = dynamic_cast<TH1F*>(dir->Get(histNameT.c_str()));

    ///Z is X3D, X is Y3D, Y is Z3D

    /// Create the TH3F histogram
    std::string s = "Event3d_" + dirName;
    hist3D = new TH3F(s.c_str() , s.c_str(),
                            hYZ->GetNbinsX(), hYZ->GetXaxis()->GetXmin(), hYZ->GetXaxis()->GetXmax(),
                            hXY->GetNbinsX(), hXY->GetXaxis()->GetXmin(), hXY->GetXaxis()->GetXmax(),
                            hXY->GetNbinsY(), hXY->GetYaxis()->GetXmin(), hXY->GetYaxis()->GetXmax());

    /// Fill the TH3F histogram with content from the TH2F histograms
    for (Int_t binX = 1; binX <= hist3D->GetNbinsY(); ++binX) {
        for (Int_t binY = 1; binY <= hist3D->GetNbinsZ(); ++binY) {
            for (Int_t binZ = 1; binZ <= hist3D->GetNbinsX(); ++binZ) {
                Double_t limit = 0.0;
                Double_t content = hXY->GetBinContent(binX, binY) +
                                   hYZ->GetBinContent(binZ, binY) +
                                   hXZ->GetBinContent(binX, binZ);
                if (hXY->GetBinContent(binX, binY)<=limit || hYZ->GetBinContent(binZ, binY)<=limit || hXZ->GetBinContent(binX, binZ)<=limit){
                    content = 0.0;
                } //comment this out if statement out for event display with 'fibers'
                if (content>limit){
                    hist3D->Fill(binZ, binX, binY, content);
                }

            }
        }
    }

    hist3D->GetXaxis()->SetTitle("Z [cm]");
    hist3D->GetYaxis()->SetTitle("X [cm]");
    hist3D->GetZaxis()->SetTitle("Y [cm]");
    /// Close the input file
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
    rootFileOutput += "StraightTracks";
    std::cout << rootFileOutput + ".root" << std::endl;
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

    unsigned int m {0};
    std::vector<Hit> straightEvent;
    double y_angle {0.0};
    double x_angle {0.0};
    unsigned int evNum;
    TFile* wfile = new TFile((rootFileOutput + ".root").c_str(), "recreate");
    TFile* wfile_examples = new TFile((rootFileOutput+"Tree.root").c_str(), "recreate");
    TTree EventsHits("EventsHits", "The ROOT tree of events");
    EventsHits.Branch("Events", "std::vector<Hit>", &straightEvent);
    EventsHits.Branch("x_angle", &x_angle, "x_angle/D");
    EventsHits.Branch("y_angle", &y_angle, "y_angle/D");
//    EventsHits.Branch("EventNumber", &evNum, "EventNumber/i");

    for (auto file = 0; file < vFileNames.size(); ++file) {
        std::cout << vFileNames[file] <<std::endl;
        std::string filename = vFileNames.at(file);
        TFile *fileInput = new TFile((filename).c_str());
        if (!fileInput->IsOpen()) {
            cerr << "Can not open file " << filename << endl;
            return 1;
        }
        TTree *eventTree = fileInput->Get<TTree>("EventsHits");
        std::vector<Hit> *eventsHits = {nullptr};
        try {
            if (eventTree->GetBranch("Events")) {
                eventTree->SetBranchAddress("Events", &eventsHits);
            }
            if (eventTree->GetBranch("EventNumber")) {
                eventTree->SetBranchAddress("EventNumber", &evNum);
            }
        } catch (...) {
            std::cerr << "Error in file " << vFileNames[file] << std::endl;
        }
        for (unsigned int j = 0; j < eventTree->GetEntries(); ++j) {
            eventTree->GetEntry(j);
            std::map<straight::Position, double> event_map_YZ;
            std::map<straight::Position, double> event_map_XZ;
            TGraph event_map_YZ_gr;
            TGraph event_map_XZ_gr;
            TH2F *event_XY = new TH2F(("Event_XY_"+ to_string(evNum)).c_str(), "", Y_SIZE, 0, Y_SIZE, X_SIZE, 0, X_SIZE);
            TH2F *event_YZ = new TH2F(("Event_YZ_"+ to_string(evNum)).c_str(),"", Z_SIZE, 0, Z_SIZE, Y_SIZE, 0, Y_SIZE);
            TH2F *event_XZ = new TH2F(("Event_XZ_"+ to_string(evNum)).c_str(), "", Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);
            TH1F *event_time = new TH1F(("Event_time"+ to_string(evNum)).c_str(), "",8000, 0 ,8000);
//            for (auto i = 0; i < eventsHits->size(); ++i) {
//                event_time->Fill(eventsHits->at(i)._timeFromGateStart);
//            }
//            unsigned int max_Time_bin =  event_time->GetMaximumBin();
            double max_LY {0.0};
            double z_min = Z_SIZE;
            double z_max = 0;
            double y_min = Y_SIZE;
            double y_max = 0;
            double x_min = X_SIZE;
            double x_max = 0;
            for (auto i = 0; i < eventsHits->size(); ++i) {
                if (/*eventsHits->at(i)._timeFromGateStart > max_Time_bin - 50 &&
                    eventsHits->at(i)._timeFromGateStart < max_Time_bin + 50 &&*/ !std::isnan(eventsHits->at(i)._LY_PE) && eventsHits->at(i)._LY_PE > 10) {
                    max_LY = std::max(max_LY,eventsHits->at(i)._LY_PE);
                    straightEvent.push_back(eventsHits->at(i));
                    event_time->Fill(eventsHits->at(i)._timeFromGateStart);
                    if (!std::isnan(eventsHits->at(i)._x) && !std::isnan(eventsHits->at(i)._y)) {
                        event_XY->Fill(eventsHits->at(i)._y, eventsHits->at(i)._x, eventsHits->at(i)._LY_PE);
                        x_max = std::max(x_max, eventsHits->at(i)._x);
                        y_max = std::max(y_max, eventsHits->at(i)._y);
                        x_min = std::min(x_min, eventsHits->at(i)._x);
                        y_min = std::min(y_min, eventsHits->at(i)._y);
                    }
                    if (!std::isnan(eventsHits->at(i)._x) && !std::isnan(eventsHits->at(i)._z)) {
                        event_XZ->Fill(eventsHits->at(i)._z, eventsHits->at(i)._x, eventsHits->at(i)._LY_PE);
                        event_map_XZ_gr.AddPoint(eventsHits->at(i)._z, eventsHits->at(i)._x);
                        event_map_XZ.insert({{eventsHits->at(i)._z, eventsHits->at(i)._x}, eventsHits->at(i)._LY_PE});
                        x_max = std::max(x_max, eventsHits->at(i)._x);
                        z_max = std::max(z_max, eventsHits->at(i)._z);
                        x_min = std::min(x_min, eventsHits->at(i)._x);
                        z_min = std::min(z_min, eventsHits->at(i)._z);
                    }
                    if (!std::isnan(eventsHits->at(i)._z) && !std::isnan(eventsHits->at(i)._y)) {
                        event_YZ->Fill(eventsHits->at(i)._z, eventsHits->at(i)._y, eventsHits->at(i)._LY_PE);
                        event_map_YZ_gr.AddPoint(eventsHits->at(i)._z, eventsHits->at(i)._y);
                        event_map_YZ.insert({{eventsHits->at(i)._z, eventsHits->at(i)._y}, eventsHits->at(i)._LY_PE});
                        z_max = std::max(z_max, eventsHits->at(i)._z);
                        y_max = std::max(y_max, eventsHits->at(i)._y);
                        z_min = std::min(z_min, eventsHits->at(i)._z);
                        y_min = std::min(y_min, eventsHits->at(i)._y);
                    }
                }
            }
            if (event_map_YZ.size() > 30 && event_map_XZ.size() > 30) {
                /// Use linear fit to check if track bend in magnetic field/ multiple particles on the graph or not
                TF1 *fit = new TF1("fit", "[0]*x+[1]");
                event_map_XZ_gr.Fit("fit", "Q");
                x_angle = fit->GetParameter(0);
                auto another_track_cubes = std::count_if(event_map_XZ.begin(), event_map_XZ.end(),
                                                         [fit](const std::pair<straight::Position, double> &t) -> bool {
                                                             return abs(fit->GetParameter(0) * t.first._par_1 +
                                                                        fit->GetParameter(1) -
                                                                        t.first._par_2) > 3 && t.second > 25;
                                                         });
                bool straight_track = another_track_cubes < 2;
                event_map_YZ_gr.Fit("fit", "Q");
                another_track_cubes = std::count_if(event_map_YZ.begin(), event_map_YZ.end(),
                                                    [fit](const std::pair<straight::Position, double> &t) -> bool {
                                                        return abs(fit->GetParameter(0) * t.first._par_1 +
                                                                   fit->GetParameter(1) - t.first._par_2) > 3 &&
                                                               t.second > 25;
                                                    });
                straight_track = straight_track && another_track_cubes < 2;
                ///  If still straight track, check that solid angle < pi / 12
                y_angle = fit->GetParameter(0);
                double theta = (std::pow(x_angle, 2) + std::pow(y_angle, 2)) /
                               std::sqrt(1 + std::pow(x_angle, 2) + std::pow(y_angle, 2));
                straight_track = straight_track && theta < 1. / 24;
                unsigned int num = 0;
                if (x_min < 2)
                    ++num;
                if (z_min < 2)
                    ++num;
                if (y_min < 2)
                    ++num;
                if (x_max > X_SIZE - 4)
                    ++num;
                if (z_max > Z_SIZE - 4)
                    ++num;
                if (y_max > Y_SIZE - 3)
                    ++num;
                if (straight_track && num > 1) {
                    /// If still straight track, check that the partiple hasn't stopped (2 * amplitude in the beginning > amplitude in the end of the track)

                    auto it_start_x = event_map_XZ.begin();
                    std::advance(it_start_x, 10);
                    auto it_finish_x = event_map_XZ.begin();
                    std::advance(it_finish_x, event_map_XZ.size() - 5);
                    auto it_start_y = event_map_YZ.begin();
                    std::advance(it_start_y, 10);
                    auto it_finish_y = event_map_YZ.begin();
                    std::advance(it_finish_y, event_map_YZ.size() - 5);
                    straight_track = 2 * std::max_element(event_map_XZ.begin(), it_start_x,
                                                          [](const pair<straight::Position, double> &p1,
                                                             const pair<straight::Position, double> &p2) {
                                                              return p1.second < p2.second;
                                                          })->second
                                     > std::max_element(it_finish_x, event_map_XZ.end(),
                                                        [](const pair<straight::Position, double> &p1,
                                                           const pair<straight::Position, double> &p2) {
                                                            return p1.second < p2.second;
                                                        })->second
                                     && 2 * std::max_element(event_map_YZ.begin(), it_start_y,
                                                             [](const pair<straight::Position, double> &p1,
                                                                const pair<straight::Position, double> &p2) {
                                                                 return p1.second < p2.second;
                                                             })->second
                                        > std::max_element(it_finish_y, event_map_YZ.end(),
                                                           [](const pair<straight::Position, double> &p1,
                                                              const pair<straight::Position, double> &p2) {
                                                               return p1.second < p2.second;
                                                           })->second;

                }
                if (straight_track && max_LY > 20) {
                    EventsHits.Fill();
                    PrintCrates pc;
                    pc.Write2DMapsCanvas(*wfile, *event_YZ, *event_XZ, *event_XY, "Entries_" + std::to_string(m));
                    ++m;
                }


            }
            delete event_YZ;
            delete event_XZ;
            delete event_XY;
            delete event_time;
            event_map_YZ_gr.Clear();
            event_map_XZ_gr.Clear();
            straightEvent.clear();
        }
        delete eventsHits;
        delete eventTree;
        delete fileInput;

    }

    wfile_examples->cd();
    EventsHits.Write("", TObject::kOverwrite);
    wfile->Close();
    wfile->Delete();
    wfile_examples->Close();
    wfile_examples->Delete();
    std::cout <<"Number of straight tracks "<< m <<std::endl;
    return 0;
}