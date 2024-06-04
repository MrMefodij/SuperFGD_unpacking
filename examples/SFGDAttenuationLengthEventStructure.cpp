//
// Created by amefodev on 24.05.2024.
//
#include "MDargumentHandler.h"
#include "EventDisplay.h"
#include "PrintCrates.h"
#include "Hit.h"
#include "Track.h"
#include <TSpectrum.h>
#include <TProfile.h>
#include <thread>
#include <TROOT.h>

std::mutex mtx;

void Attenuation(std::string filename, TTree *eventTree, std::vector<Hit> *eventsHits, const Connection_Map& connectionMap,
                 AttLengthCalculation::AttenuationPlots& attenuationLength, AttLengthCalculation::AllEvents& allEvents){
    for (unsigned int j = 0; j < eventTree->GetEntries(); ++j) {
        eventTree->GetEntry(j);
        std::map< int, std::vector<Hit>> eventStructure;
        int tDigits = 50;
        if (eventsHits->size() > 150) {
            for (auto i = 0; i < eventsHits->size(); ++i) {
                int time_slot = eventsHits->at(i)._timeFromGateStart / tDigits;
                if (eventStructure.find(time_slot) == eventStructure.end())
                    eventStructure.insert({time_slot, {eventsHits->at(i)}});
                else {
                    eventStructure[time_slot].push_back(eventsHits->at(i));
                }
            }

            for (int i = 0; i <= eventStructure.rbegin()->first; ++i) {
                if (eventStructure[i].size() > 150) {
                    std::vector<Hit> selectedTrack;
                    if (i > 0 && eventStructure[i-1].size() > 0)
                        selectedTrack.insert(selectedTrack.end(), eventStructure.at(i-1).begin(), eventStructure.at(i-1).end());
                    selectedTrack.insert(selectedTrack.end(), eventStructure.at(i).begin(), eventStructure.at(i).end());
                    if (i != eventStructure.rbegin()->first && eventStructure[++i].size() > 0)
                        selectedTrack.insert(selectedTrack.end(), eventStructure.at(i).begin(), eventStructure.at(i).end());

                    sort(selectedTrack.begin(), selectedTrack.end(),[&](auto l_hit, auto r_hit){
                        return l_hit._y < r_hit._y;
                    });
                    AttLengthCalculation::Track track(selectedTrack);
                    track.CalculateAngles();
                    for (int y = 0; y < Y_SIZE; ++y) {
                        if (!std::isnan(track._hUpstream.position_[y]) && !std::isnan(track._hSide.position_[y])){
                            attenuationLength.AttenuationLength->Fill(track._hSide.position_[y],track._hUpstream.norm_LY_[y]);
                            attenuationLength.AttenuationLengthUpstream->Fill(track._hSide.position_[y],track._hUpstream.norm_LY_[y]);
                            if (track._hSide.position_[y] < 95) {
                                attenuationLength.AttenuationLength->Fill(track._hUpstream.position_[y], track._hSide.norm_LY_[y]);
                                attenuationLength.AttenuationLengthSide->Fill(track._hUpstream.position_[y], track._hSide.norm_LY_[y]);
                            } else {
                                attenuationLength.AttenuationLength->Fill(X_SIZE - track._hUpstream.position_[y], track._hSide.norm_LY_[y]);
                                attenuationLength.AttenuationLengthSide->Fill(X_SIZE - track._hUpstream.position_[y], track._hSide.norm_LY_[y]);
                            }
                        }
                    }
                    for (int k = 0; k < selectedTrack.size(); ++k) {
                        if (!std::isnan(selectedTrack.at(k)._x) && !std::isnan(selectedTrack.at(k)._y)) {
                            allEvents.event_XY->Fill(selectedTrack.at(k)._y, selectedTrack.at(k)._x, 1);
                            if(selectedTrack.at(k)._highGainADC > 1)
                                allEvents.event_XY_digit->Fill(selectedTrack.at(k)._y, selectedTrack.at(k)._x, 1);
                        }
                        if (!std::isnan(selectedTrack.at(k)._x) && !std::isnan(selectedTrack.at(k)._z)) {
                            allEvents.event_XZ->Fill(selectedTrack.at(k)._z, selectedTrack.at(k)._x, 1);
                            if(selectedTrack.at(k)._highGainADC > 1)
                                allEvents.event_XZ_digit->Fill(selectedTrack.at(k)._z, selectedTrack.at(k)._x, 1);
                        }
                        if (!std::isnan(selectedTrack.at(k)._z) && !std::isnan(selectedTrack.at(k)._y)) {
                            allEvents.event_YZ->Fill(selectedTrack.at(k)._z, selectedTrack.at(k)._y, 1);
                            if(selectedTrack.at(k)._highGainADC > 1)
                                allEvents.event_YZ_digit->Fill(selectedTrack.at(k)._z, selectedTrack.at(k)._y,1);
                        }
                    }
                }
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
    vector <string> vFileNames = argh.GetDataFiles(stringBuf, ".root", "EventStructure");
    std::sort(vFileNames.begin(), vFileNames.end());

    if (vFileNames.empty()) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    size_t pos = stringBuf.find("EventStructure");
    std::string rootFileOutput;
    if (pos != string::npos)
        rootFileOutput = stringBuf.substr(0, pos);
    else {
        rootFileOutput = stringBuf;
    }
    rootFileOutput += "AttenuationLengthThreads.root";
    std::cout << rootFileOutput << std::endl;
    std::string mapFile = "../connection_map/SFG_Geometry_Map_v18.csv";
    if (getenv("UNPACKING_ROOT") != nullptr) {
        mapFile = (string) getenv("UNPACKING_ROOT") + "/connection_map/SFG_Geometry_Map_v18.csv";
    }

    Connection_Map connectionMap(mapFile);
    try {
        connectionMap.Init();
    } catch (const exception &e) {
        std::cerr << "Unable to open file " << mapFile << std::endl;
        exit(1);
    }
    ROOT::EnableThreadSafety();
    AttLengthCalculation::AttenuationPlots attenuationLength;
    AttLengthCalculation::AllEvents allEvents;
    size_t  threadsNum = std::fmin(UNPACKING_PTHREADS,vFileNames.size());
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
            th[i] = std::thread(&Attenuation,  vFileNames[i + j], std::ref(eventTree[i]), std::ref(eventsHits[i]), std::ref(connectionMap), std::ref(attenuationLength), std::ref(allEvents));

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

    {
        PrintCrates pc;
        pc.Write2DMapsCanvas(*wfile, *allEvents.event_YZ, *allEvents.event_XZ, *allEvents.event_XY, "Entries");
        allEvents.event_XZ->Delete();
        allEvents.event_XY->Delete();
        allEvents.event_YZ->Delete();
    }
    {
        PrintCrates pc2;
        pc2.Write2DMapsCanvas(*wfile, *allEvents.event_YZ_digit, *allEvents.event_XZ_digit, *allEvents.event_XY_digit,
                              "Digitized_channels");
        allEvents.event_XZ_digit->Delete();
        allEvents.event_XY_digit->Delete();
        allEvents.event_YZ_digit->Delete();
    }
    attenuationLength.AttenuationLength->Write();
    attenuationLength.AttenuationLengthSide->Write();
    attenuationLength.AttenuationLengthUpstream->Write();

    wfile->Close();
}