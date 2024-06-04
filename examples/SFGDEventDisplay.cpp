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
// Created by amefodev on 13.06.2023. mrmefodij@gmail.com
//

#include <string>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TH2.h"
#include <tuple>

#include "MDargumentHandler.h"
#include "ToaEventDummy.h"
#include "TSFGDigit.hxx"
#include "Connection_Map.h"
#include "SFGD_defines.h"
#include "EventDisplay.h"
#include "Reader.h"

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

    vector<string> vFileNames  = argh.GetDataFiles(stringBuf,".root","_plots");
    std::sort(vFileNames.begin(), vFileNames.end(), [](const auto& itL, const  auto& itR){return itL > itR;});

    if ( vFileNames.size() == 0 ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    unsigned int cratesCount = vFileNames.size();
    vector<DataFile> allFiles(cratesCount);
    size_t pos = stringBuf.find("_plots");
    std::string rootFileOutput;
    if(pos != string::npos)
        rootFileOutput = stringBuf.substr(0, pos) + "_";
    else{
        rootFileOutput = stringBuf;
    }
    rootFileOutput += "Events.root";
    cout << rootFileOutput << endl;
    TFile wfile(rootFileOutput.c_str(), "recreate");
    TH1F eventTime = TH1F ("Events_Time", "EventTime",8000, 0 ,80000);

    string mapFile = "../connection_map/SFG_Geometry_Map_v18.csv";
    if(getenv("UNPACKING_ROOT")!=NULL){
        mapFile = (string)getenv("UNPACKING_ROOT") + "/connection_map/SFG_Geometry_Map_v18.csv";
    }
    Connection_Map connectionMap(mapFile);
    try {
        connectionMap.Init();
    } catch (const exception& e) {
        cerr << "Unable to open file " << mapFile << endl;
        exit(1);
    }

    vector<string> vGainResult = argh.GetDirectoryFiles("../calibration_results/4795/", ".txt");
    std::sort(vGainResult.begin(), vGainResult.end());
    if (vGainResult.empty()) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }
    GainReader calib_res(vGainResult);
    calib_res.Init();

    vector<string> vPedResult = argh.GetDirectoryFiles("../pedmap_results/", ".dat");
    std::sort(vPedResult.begin(), vPedResult.end());
    if (vPedResult.empty()) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }
    PedMapReader pedResult(vPedResult);
    pedResult.Init();

    std::vector<ToaEventDummy> *FEBs[SFGD_FEBS_NUM];
    for (Int_t i = 0; i < SFGD_FEBS_NUM; ++i) {
        FEBs[i] = 0;
    }


    for(unsigned int file = 0; file < vFileNames.size(); ++file) {
        allFiles[file].fileName_ = vFileNames.at(file);
        allFiles[file].FileInput_ = new TFile((vFileNames.at(file)).c_str());
        if (!allFiles[file].FileInput_->IsOpen()) {
            cerr << "Can not open file " << vFileNames.at(file) << endl;
            return 1;
        }
        vector<unsigned int> availableFebs;
        allFiles[file].AllEvents_ = (TTree *) allFiles[file].FileInput_->Get("AllEvents");
        ostringstream sFEBnum;
        string sFEB;
        for (unsigned int ih = 0; ih < SFGD_FEBS_NUM; ++ih) {
            sFEBnum.str("");
            sFEBnum << ih;
            sFEB = "FEB_" + sFEBnum.str();
            if (allFiles[file].AllEvents_->GetBranch((sFEB).c_str())) {
                allFiles[file].AllEvents_->SetBranchAddress((sFEB).c_str(), &FEBs[ih]);
                availableFebs.push_back(ih);
            }
        }
        if (!availableFebs.empty()) {
            unsigned int nEntries = allFiles[file].AllEvents_->GetEntries();
            allFiles[file].availableCrates_= (CrateGates{(availableFebs.back() >> 4), std::move(availableFebs), nEntries});

        }
    }

    unsigned int evNum = 0;
    bool gateIsOk = false;
    for (int iPos = allFiles.size() ; iPos > 0; --iPos) {
        const auto &a = allFiles.at(iPos-1);
        std::cout << "entries in file " << a.fileName_ <<" is " << a.AllEvents_->GetEntries() << std::endl;
        for (unsigned int i = 0; i < a.AllEvents_->GetEntries(); ++i) {
            try {
                a.AllEvents_->GetEntry(i);
            } catch (...) {
                cerr << "Error in file " << a.fileName_ << " entry " << i << endl;
            }

            gateIsOk = true;
            for (const auto feb: a.availableCrates_.availableFebs_) {
                if (gateIsOk) {
                    try {
//                        cout << AS_KV(feb) << endl;
                        std::vector<TSFGDigit> temp = FEBs[feb]->at(0).GetHits();
                    } catch (...) {
                        gateIsOk = false;
                        cerr << "Error on OCB event Number: \t" << evNum + 1 << "\t FEB#: " << feb << endl;
                        break;
                    }
                }
            }
            if (gateIsOk) {
                evNum = FEBs[a.availableCrates_.availableFebs_.at(0)]->back().GetEventNumber();
//            std::cout << evNum << endl;
                ostringstream evNumString;
                evNumString << evNum;
                EventDisplay ev(evNumString.str());
                std::vector<FebData> data;
                for (const auto &febNum: a.availableCrates_.availableFebs_) {
                    data.push_back({febNum, FEBs[febNum]->at(0).GetHits()});
                }
                ev.AddEvents(data, connectionMap, calib_res, pedResult, eventTime);
                auto hist = ev.GetPictures();
                bool eventExist = std::get<2>(hist);
                if (eventExist) {
                    TDirectory *hitDir = wfile.mkdir((evNumString.str()).c_str());
                    hitDir->cd();
                    std::get<0>(hist)->Write();
                    std::get<0>(hist)->Delete();
                    std::get<1>(hist)->Write();
                    std::get<1>(hist)->Delete();

                    string list[] = {"ToT", "HG", "LG", "pe", "time"};
                    for (const auto &ampl: list) {
                        auto hist3D = ev.GetPictures(ampl);
                        std::get<0>(hist3D)->Write();
                        std::get<0>(hist3D)->Delete();
                        std::get<1>(hist3D)->Write();
                        std::get<1>(hist3D)->Delete();
                        std::get<2>(hist3D)->Write();
                        std::get<2>(hist3D)->Delete();
                    }
                    hitDir->Delete();
                }
            }
        }
        allFiles.pop_back();
    }
    wfile.cd();
    eventTime.Write();
    eventTime.Delete();
    wfile.Close();
    return 0;
}
