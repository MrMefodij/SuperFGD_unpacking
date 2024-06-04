//
// Created by Maria on 28.09.2023.
//


#include <string>
#include <fstream>
#include <numeric>
#include <TFile.h>
#include <TTree.h>
#include "TDirectory.h"
#include <tuple>
#include <set>
#include "TF1.h"
#include "MDargumentHandler.h"
#include "ToaEventDummy.h"
#include "Connection_Map.h"
#include "SFGD_defines.h"
#include "EventDisplay.h"
#include "CrateSlotAsic_missingChs.h"
#include <TProfile.h>

#define NUM_EVENTS_from_AVARAGE 0.5

double fit(double *x, double *par) {
    return par[0] + par[1]*x[0] + par[2]*x[0]*x[0] +par[3]*x[0]*x[0]*x[0] +par[4]*x[0]*x[0]*x[0]*x[0];
}

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
    std::sort(vFileNames.begin(), vFileNames.end());

    if ( vFileNames.empty() ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    unsigned int cratesCount = vFileNames.size();
    vector<DataFile> allFiles(cratesCount);
    std::ofstream fout_entries((stringBuf+"/DataCheck.txt").c_str());
    std::ofstream fout_missing_ch((stringBuf+"/Missing_Channels.txt").c_str());
    string rootFileOutput = stringBuf + "/DataCheck.root";
    cout << rootFileOutput << endl;
    TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");

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

    string missingFile = "../connection_map/crateSlotAsic_missingChs.txt";
    CrateSlotAsic_missingChs crateSlotAsicMissingChs(missingFile);
    crateSlotAsicMissingChs.Init();

    std::vector<ToaEventDummy> *FEBs[SFGD_FEBS_NUM];
    for (Int_t i = 0; i < SFGD_FEBS_NUM; ++i) {
        FEBs[i] = 0;
    }

    for(auto file = 0; file < vFileNames.size(); ++file) {
        allFiles[file].fileName_ = vFileNames.at(file);
        allFiles[file].FileInput_ = new TFile((vFileNames.at(file)).c_str());
        if (!allFiles[file].FileInput_->IsOpen()) {
            cerr << "Can not open file " << vFileNames.at(file) << endl;
            return 1;
        }
        allFiles[file].AllEvents_ = (TTree *) allFiles[file].FileInput_->Get("AllEvents");
        ostringstream sFEBnum;
        string sFEB;
        vector<unsigned int> availableFebs;
        for (auto ih = 0; ih < SFGD_FEBS_NUM; ++ih) {
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
            std::cout << "entries at Crate " << allFiles[file].availableCrates_.crateNumber_ <<" is " << nEntries << std::endl;
        }
    }
    unsigned int nEvents = 0;
    for (const auto & a : allFiles){
        nEvents += a.availableCrates_.nEntries_;
    }

    vector<TH1F*> hFEBCH_channel (SFGD_FEBS_NUM);
    vector<TH1F*> hFEBCH_channel_time (SFGD_FEBS_NUM);
    vector<TH1F*> hFEBCH_LowGain (SFGD_FEBS_NUM);
    vector<TH1F*> hFEBCH_HighGain (SFGD_FEBS_NUM);


    TH1F* hFEBCH = new TH1F( "Number of channels with Ampl","Number of channels with Ampl", SFGD_FEB_NCHANNELS  , 0, SFGD_FEB_NCHANNELS);
    hFEBCH->GetYaxis()->SetTitle("Entries");
    hFEBCH->GetXaxis()->SetTitle("Channel");
    TH1F* hFEBCH_time = new TH1F( "Number of channels with Time","Number of channels with Time", SFGD_FEB_NCHANNELS  , 0, SFGD_FEB_NCHANNELS);
    hFEBCH_time->GetYaxis()->SetTitle("Entries");
    hFEBCH_time->GetXaxis()->SetTitle("Channel");

    TH1F* HitCount = new TH1F( "Number of hits per Gate","Number of hits per Gate", 1000  , 0, 1000);
    HitCount->GetYaxis()->SetTitle("Entries");
    HitCount->GetXaxis()->SetTitle("N [fired channels/gate]");

    TH2F *EventMap_XZ = new TH2F("Events_map_Ampl_XZ", "Events_map_Ampl_XZ", 192, 0, 192, 182, 0, 182);
    TH2F *EventMap_XY = new TH2F("Events_map_Ampl_XY", "Events_map_Ampl_XY", 192, 0, 192, 56, 0, 56);
    TH2F *EventMap_ZY = new TH2F("Events_map_Ampl_ZY", "Events_map_Ampl_ZY", 182, 0, 182, 56, 0, 56);

    TH2F *HG_vs_ToT = new TH2F("HG_vs_ToT", "HG_vs_ToT",  150, 0, 150, 4096, 0, 4096);
    TH2F *LG_vs_ToT= new TH2F("LG_vs_ToT", "LG_vs_ToT", 150, 0, 150, 4096, 0, 4096);
    TH2F *LG_vs_HG = new TH2F("LG_vs_HG", "LG_vs_HG", 256, 0, 4096, 256, 0, 4096);

    TH2F *EventMapTime_XZ = new TH2F("Events_map_Time_XZ", "Events_map_Time_XZ", 192, 0, 192, 182, 0, 182);
    TH2F *EventMapTime_XY = new TH2F("Events_map_Time_XY", "Events_map_Time_XY", 192, 0, 192, 56, 0, 56);
    TH2F *EventMapTime_ZY = new TH2F("Events_map_Time_ZY", "Events_map_Time_ZY", 182, 0, 182, 56, 0, 56);
    TH2F *GTS_Missynchronization = new TH2F("GTS_Missynchronization", "GTS_Missynchronization", 256, 0, 256, 20, -10,10 );
    TH2F *TimefromGTS_Missynchronization = new TH2F("TimefromGTS_Missynchronization", "TimefromGTS_Missynchronization", 256, 0, 256, 7, -3.5,3.5);
    TH2F *GetEntriesInFEBs = new TH2F("GetEntriesInFEBs","GetEntriesInFEBs",256,0,256,8,0,8);
    for(unsigned int i = 0; i < SFGD_FEBS_NUM; i++){
        std::string sCh = "FEB_"+std::to_string(i);
        hFEBCH_channel[i] = new TH1F( sCh.c_str(),sCh.c_str(), SFGD_FEB_NCHANNELS  , 0, SFGD_FEB_NCHANNELS);
        hFEBCH_channel[i]->GetYaxis()->SetTitle("Entries");
        hFEBCH_channel[i]->GetXaxis()->SetTitle("Channel");
        std::string sChTime = "FEB_Time_"+std::to_string(i);
        hFEBCH_channel_time[i] = new TH1F( sChTime.c_str(),sChTime.c_str(), SFGD_FEB_NCHANNELS  , 0, SFGD_FEB_NCHANNELS);
        hFEBCH_channel_time[i]->GetYaxis()->SetTitle("Entries");
        hFEBCH_channel_time[i]->GetXaxis()->SetTitle("Channel");
        hFEBCH_LowGain[i] = new TH1F((sCh+"_lowGain").c_str(),(sCh+"_lowGain").c_str(), nEvents , 0, nEvents);
        hFEBCH_LowGain[i]->GetYaxis()->SetTitle("Entries");
        hFEBCH_LowGain[i]->GetXaxis()->SetTitle("Event");
        hFEBCH_HighGain[i] = new TH1F((sCh+"_highGain").c_str(),(sCh+"_highGain").c_str(), nEvents  , 0, nEvents);
        hFEBCH_HighGain[i]->GetYaxis()->SetTitle("Entries");
        hFEBCH_HighGain[i]->GetXaxis()->SetTitle("Event");
    }
    wfile->cd();
    std::string s = "FEB"+to_string(1);
    std::cout << nEvents<<"\n";
    unsigned int sum_ev = 0;
    for (const auto &file: allFiles) {
        std::cout << file.fileName_ << std::endl;
        auto evNums = file.availableCrates_.nEntries_;
        bool gateIsOk = true;
        for (auto evNum = 0; evNum < evNums; evNum++) {
            sum_ev += 1;
            try {
                file.AllEvents_->GetEntry(evNum);
            } catch (...) {
//                cerr << "Error in file " << a.fileName_ << " entry " << evNum << endl;
            }
            map<int, vector<unsigned int>> timefromGTS;
            map<int, vector<unsigned int>> gtsTime;
            for (unsigned int boardId: file.availableCrates_.availableFebs_) {
                if (gateIsOk) {
                    try {
                        auto temp = FEBs[boardId]->at(0);
                        if (timefromGTS.find(temp.GetGateTimeFrGts()) != timefromGTS.end())
                            timefromGTS[temp.GetGateTimeFrGts()].push_back(boardId);
                        else {
                            timefromGTS[temp.GetGateTimeFrGts()] = {boardId};
                        }
                        auto hits = temp.GetHits();
                    }
                    catch (...) {
                        gateIsOk = false;
                        cerr << "Error on OCB event Number: \t" << evNum + 1 << "\t FEB#: " << boardId << endl;
                        break;
                    }

                    if (gateIsOk) {
                        auto hits = FEBs[boardId]->at(0).GetHits();
                        if (!hits.empty()) {
                            auto currentGTSCounter = hits[0].GetGTSCounter();
                            if (gtsTime.find(currentGTSCounter) != gtsTime.end())
                                gtsTime[currentGTSCounter].push_back(boardId);
                            else {
                                gtsTime[currentGTSCounter] = {boardId};
                            }
                            HitCount->Fill(hits.size());
                            for (const auto &hit: hits) {
                                auto ch256 = hit.GetChannelNumber();
                                hFEBCH_time->Fill(ch256);
                                hFEBCH_channel_time[boardId]->Fill(ch256);
                                auto HG_value = hit.GetHighGainADC();
                                auto LG_value = hit.GetLowGainADC();
                                auto ToT_value = hit.GetTimeOverThreshold();
                                if (HG_value > 1 && LG_value > 1 && ToT_value > 12) {
                                    LG_vs_HG->Fill(HG_value, LG_value, 1);
                                    HG_vs_ToT->Fill(ToT_value, HG_value, 1);
                                    LG_vs_ToT->Fill(ToT_value, LG_value, 1);
                                }
                                if (LG_value != 0) {
                                    hFEBCH_LowGain[boardId]->Fill(sum_ev);
                                }
                                if (HG_value != 0)
                                    hFEBCH_HighGain[boardId]->Fill(sum_ev);
                                if (HG_value != 0 && LG_value != 0) {
                                    hFEBCH_channel[boardId]->Fill(ch256);
                                    hFEBCH->Fill(ch256);
                                }
                            }
                        }
                    }
                }
            }
            auto pr = std::max_element
                    (
                            std::begin(gtsTime), std::end(gtsTime),
                            [](const auto &p1, const auto &p2) {
                                return p1.second.size() < p2.second.size();
                            }
                    );

            for (const auto &i: gtsTime) {
                for (const auto &j: i.second) {
                    GTS_Missynchronization->Fill(j, i.first - pr->first, 1);
                }
            }

            pr = std::max_element
                    (
                            std::begin(timefromGTS), std::end(timefromGTS),
                            [](const auto &p1, const auto &p2) {
                                return p1.second.size() < p2.second.size();
                            }
                    );

            for (const auto &i: timefromGTS) {
                for (const auto &j: i.second) {
                    TimefromGTS_Missynchronization->Fill(j, i.first - pr->first, 1);
                }
            }
        }
    }
    wfile->cd();
    hFEBCH->Write();
    hFEBCH_time->Write();
    delete hFEBCH;

    vector<unsigned int> entries_for_Crate[SFGD_CRATES_NUM];
    for (unsigned int boardId = 0; boardId < SFGD_FEBS_NUM; ++boardId) {
        vector<unsigned int> entries[SFGD_ASIC_in_FEB];
        unsigned int slot_id = boardId & 0x0f;
        unsigned int crate_number = boardId >> 4;
        if(hFEBCH_channel[boardId]->GetEntries()!=0) {
            auto bins = hFEBCH_channel[boardId]->GetNbinsX();
            for(unsigned int ch256 = 0; ch256 < bins; ch256++){
                Mapping map = {crate_number, slot_id ,ch256 / 32};
                if(!crateSlotAsicMissingChs.Is_Missing_Chs(map,ch256) && !crateSlotAsicMissingChs.Is_Missing_FEB(boardId) && !crateSlotAsicMissingChs.Is_Missing_ASIC(map)) {
                    auto geomPosition = connectionMap.GetGlobalGeomPositionPtr(boardId, ch256);
                    auto num_events_time =  hFEBCH_channel_time[boardId]->GetBinContent(ch256+1);
                    if(num_events_time == 0){
                        unsigned int crate = boardId >> 4;
                        fout_missing_ch <<"Dead channel before digitizing: Crate "<<crate << ", FEB "
                                        << boardId<<", Channel "<<ch256<<std::endl;
                    }
                    else{
                        entries[ch256/32].push_back(num_events_time);
                        entries_for_Crate[boardId >> 4].push_back(num_events_time);
                    }

                    EventMapTime_XZ->Fill(geomPosition->x_, geomPosition->z_, num_events_time /*/ nEvents*/);
                    EventMapTime_XY->Fill(geomPosition->x_, geomPosition->y_, num_events_time /*/ nEvents*/);
                    EventMapTime_ZY->Fill(geomPosition->z_, geomPosition->y_, num_events_time /*/ nEvents*/);

                    auto num_events =  hFEBCH_channel[boardId]->GetBinContent(ch256+1);
                    if(num_events == 0){
                        unsigned int crate = boardId >> 4;
                        fout_missing_ch <<"Dead channel after digitizing: Crate "<<crate << ", FEB " << boardId<<", Channel "<<ch256<<std::endl;
                    }
                    EventMap_XZ->Fill(geomPosition->x_, geomPosition->z_, num_events /*/ nEvents*/);
                    EventMap_XY->Fill(geomPosition->x_, geomPosition->y_, num_events /*/ nEvents*/);
                    EventMap_ZY->Fill(geomPosition->z_, geomPosition->y_, num_events /*/ nEvents*/);
                }
            }
        }
        for(auto i=0; i<SFGD_ASIC_in_FEB; i++){
            double sum = std::accumulate(std::begin(entries[i]), std::end(entries[i]), 0.0);
            if(!entries[i].empty())
                GetEntriesInFEBs->Fill(boardId,i,sum / entries[i].size());
        }
    }

    for(auto board_id = 0; board_id < SFGD_FEBS_NUM; board_id++){
        if(hFEBCH_HighGain[board_id]->GetEntries() > 0) {
            TDirectory *FEBdir = wfile->mkdir(("FEB_" + to_string(board_id)).c_str());
            FEBdir->cd();
            hFEBCH_HighGain[board_id]->Write();
            if (hFEBCH_LowGain[board_id]->GetEntries() > 0)
                hFEBCH_LowGain[board_id]->Write();
            if (hFEBCH_channel[board_id]->GetEntries() > 0)
                hFEBCH_channel[board_id]->Write();
            for(auto asic_id = 0; asic_id < SFGD_ASIC_in_FEB; ++asic_id){
                auto sum = std::accumulate(std::begin(entries_for_Crate[board_id >> 4]), std::end(entries_for_Crate[board_id >> 4]), 0.0);
                if(!entries_for_Crate[board_id >> 4].empty() && GetEntriesInFEBs->GetBinContent(board_id+1,asic_id+1) < NUM_EVENTS_from_AVARAGE * (sum / entries_for_Crate[board_id >> 4].size()))
                    fout_entries<<"Crate "<<(board_id>>4)<<" FEB "<<board_id<<" ASIC "<<asic_id<<" lack of entries"<<std::endl;
            }
            FEBdir->Delete();
        }
        delete hFEBCH_HighGain[board_id];
        delete hFEBCH_LowGain[board_id];
        delete hFEBCH_channel[board_id];
        delete hFEBCH_channel_time[board_id];
    }
    wfile->cd();
    TimefromGTS_Missynchronization->Write();
    delete TimefromGTS_Missynchronization;
    GTS_Missynchronization->Write();
    delete GTS_Missynchronization;
    EventMap_XZ->GetYaxis()->SetTitle("Z");
    EventMap_XZ->GetXaxis()->SetTitle("X");
    EventMap_XZ->Write();
    EventMap_ZY->GetYaxis()->SetTitle("Y");
    EventMap_ZY->GetXaxis()->SetTitle("Z");
    EventMap_ZY->Write();
    EventMap_XY->GetYaxis()->SetTitle("Y");
    EventMap_XY->GetXaxis()->SetTitle("X");
    EventMap_XY->Write();

    EventMapTime_XZ->GetYaxis()->SetTitle("Z");
    EventMapTime_XZ->GetXaxis()->SetTitle("X");
    EventMapTime_XZ->Write();
    EventMapTime_ZY->GetYaxis()->SetTitle("Y");
    EventMapTime_ZY->GetXaxis()->SetTitle("Z");
    EventMapTime_ZY->Write();
    EventMapTime_XY->GetYaxis()->SetTitle("Y");
    EventMapTime_XY->GetXaxis()->SetTitle("X");
    EventMapTime_XY->Write();

    delete EventMap_XZ;
    delete EventMap_XY;
    delete EventMap_ZY;
    delete EventMapTime_XZ;
    delete EventMapTime_XY;
    delete EventMapTime_ZY;

    HitCount->Write();
    HitCount->Delete();

    GetEntriesInFEBs->GetYaxis()->SetTitle("ASIC");
    GetEntriesInFEBs->GetXaxis()->SetTitle("FEB");
    GetEntriesInFEBs->Write();
    delete GetEntriesInFEBs;

    TProfile *prof = HG_vs_ToT->ProfileX();
    prof->GetYaxis()->SetTitle("HG [ADC]");
    prof->GetXaxis()->SetTitle("Time over Threshold [1.25 ns]");
    TF1 *f = new TF1("f",fit,12,30,5);
    prof->Fit("f","","",12,30);
    prof->Write();
    HG_vs_ToT->Write();
    delete HG_vs_ToT;

    prof = LG_vs_ToT->ProfileX();
    prof->GetYaxis()->SetTitle("LG [ADC]");
    prof->GetXaxis()->SetTitle("Time over Threshold [1.25 ns]");
    prof->Fit("f","MER","",12,35);
    prof->Write();
    LG_vs_ToT->Write();
    delete LG_vs_ToT;

    prof = LG_vs_HG->ProfileX();
    prof->GetYaxis()->SetTitle("LG [ADC]");
    prof->GetXaxis()->SetTitle("HG [ADC]");
    prof->Write();
    LG_vs_HG->Write();
    delete LG_vs_HG;
    prof->Delete();
    wfile->Close();
    delete f;
    return 0;
}

