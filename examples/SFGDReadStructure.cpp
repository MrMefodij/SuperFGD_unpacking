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
#include "EventDisplay.h"
#include <TF1.h>
#include <TSpectrum.h>
#include "PrintCrates.h"
#include "Hit.h"



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
    string txtFileOutput = rootFileOutput + "GateTime_Number.txt";
    std::ofstream fout(txtFileOutput.c_str());
    rootFileOutput += "EventReader.root";
    std::cout << rootFileOutput << std::endl;
    cout << rootFileOutput << endl;
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

    TFile* wfile = new TFile(rootFileOutput.c_str(), "recreate");
    TH2F* event_XY = new TH2F("Event_XY", "",Y_SIZE, 0, Y_SIZE, X_SIZE, 0, X_SIZE);
    TH2F* event_YZ = new TH2F("Event_YZ", "",Z_SIZE, 0, Z_SIZE,Y_SIZE, 0, Y_SIZE);
    TH2F* event_XZ = new TH2F("Event_XZ", "",Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);
    TH2F* event_XY_digit = new TH2F("Event_XY_digit", "",Y_SIZE, 0, Y_SIZE, X_SIZE, 0, X_SIZE);
    TH2F* event_YZ_digit = new TH2F("Event_YZ_digit", "",Z_SIZE, 0, Z_SIZE,Y_SIZE, 0, Y_SIZE);
    TH2F* event_XZ_digit = new TH2F("Event_XZ_digit", "",Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);

    for (auto file = 0; file < vFileNames.size(); ++file) {
        std::string filename = vFileNames.at(file);
        TFile *fileInput = new TFile((filename).c_str());
        if (!fileInput->IsOpen()) {
            cerr << "Can not open file " << filename << endl;
            return 1;
        }
        TTree *eventTree = fileInput->Get<TTree>("EventsHits");
        std::vector<Hit> *eventsHits = {nullptr};
        unsigned long long int gateTime{0};
        unsigned int evNum = 0;
        try {
            if (eventTree->GetBranch("Events")) {
                eventTree->SetBranchAddress("Events", &eventsHits);
            }
            if (eventTree->GetBranch("EventTime")) {
                eventTree->SetBranchAddress("EventTime", &gateTime);
            }
            if (eventTree->GetBranch("EventNumber")) {
                eventTree->SetBranchAddress("EventNumber", &evNum);
            }
        } catch (...) {
            std::cerr << "Error in file " << vFileNames[file] << std::endl;
        }
        for (unsigned int j = 0; j < eventTree->GetEntries(); ++j) {
            eventTree->GetEntry(j);
            fout << gateTime << " " <<  evNum << std::endl;
            for (auto i = 0; i < eventsHits->size(); ++i) {
                if (!std::isnan(eventsHits->at(i)._x) && !std::isnan(eventsHits->at(i)._y)) {
                    event_XY->Fill(eventsHits->at(i)._y, eventsHits->at(i)._x, 1);
                    if(eventsHits->at(i)._highGainADC > 1)
                    event_XY_digit->Fill(eventsHits->at(i)._y, eventsHits->at(i)._x, 1);
                }
                if (!std::isnan(eventsHits->at(i)._x) && !std::isnan(eventsHits->at(i)._z)) {
                    event_XZ->Fill(eventsHits->at(i)._z, eventsHits->at(i)._x, 1);
                    if(eventsHits->at(i)._highGainADC > 1)
                    event_XZ_digit->Fill(eventsHits->at(i)._z, eventsHits->at(i)._x, 1);
                }
                if (!std::isnan(eventsHits->at(i)._z) && !std::isnan(eventsHits->at(i)._y)) {
                    event_YZ->Fill(eventsHits->at(i)._z, eventsHits->at(i)._y, 1);
                    if(eventsHits->at(i)._highGainADC > 1)
                        event_YZ_digit->Fill(eventsHits->at(i)._z, eventsHits->at(i)._y,1);
                }
            }
        }
    }

    wfile->cd();

    PrintCrates pc;
    pc.Write2DMapsCanvas(*wfile, *event_YZ, *event_XZ, *event_XY,"Entries");
    event_XZ->Delete();
    event_XY->Delete();
    event_YZ->Delete();
    pc.Write2DMapsCanvas(*wfile, *event_YZ_digit, *event_XZ_digit, *event_XY_digit, "Digitized_channels");
    event_XZ_digit->Delete();
    event_XY_digit->Delete();
    event_YZ_digit->Delete();
    wfile->Close();
    wfile->Delete();

    return 0;
}