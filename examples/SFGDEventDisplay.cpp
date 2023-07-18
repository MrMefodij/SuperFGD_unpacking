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
#include "Connection_Map.h"
#include "Files_Reader.h"
#include "SFGD_defines.h"


std::tuple<TH1F,TH1F,TH2F> GetPictures(const ostringstream& evNumString, const std::vector<TSFGDigit>& hitsFEB, const unsigned int feb,const Connection_Map& connectionMap){
    ostringstream febStr;
    febStr<<feb;
    TH1F gtsSlot(("GtsSlot_" + evNumString.str() + "_" + febStr.str()).c_str(),("GtsSlot_" + evNumString.str() + "_" + febStr.str()).c_str(),5,0,5 );
    TH1F eventTime(("EventTime_" + evNumString.str() + "_" + febStr.str()).c_str(), ("EventTime_" + evNumString.str() + "_" + febStr.str()).c_str(),4000, 0 ,4000);
//    TH2F event(("Event_" + evNumString.str() + "_" + febStr.str()).c_str(), ("Event_" + evNumString.str() + "_" + febStr.str()).c_str(),16, 0, 15, 16, 55, 70);
    TH2F event;
    if (feb == 247){
        event = TH2F(("Event_" + evNumString.str() + "_" + febStr.str()).c_str(), ("Event_" + evNumString.str() + "_" + febStr.str()).c_str(),
                     16, 0, 16, 16, 40, 56);
        event.GetYaxis()->SetTitle("Y [cm]");
        event.GetXaxis()->SetTitle("X [cm]");
        event.GetZaxis()->SetTitle("ToT [2.5 ns]");
    } else if (feb == 251){
        event = TH2F(("Event_" + evNumString.str() + "_" + febStr.str()).c_str(), ("Event_" + evNumString.str() + "_" + febStr.str()).c_str(),
                     16, 55, 71, 16, 40, 56);
        event.GetYaxis()->SetTitle("Y [cm]");
        event.GetXaxis()->SetTitle("Z [cm]");
        event.GetZaxis()->SetTitle("ToT [2.5 ns]");
    } else {
        event = TH2F(("Event_" + evNumString.str() + "_" + febStr.str()).c_str(), ("Event_" + evNumString.str() + "_" + febStr.str()).c_str(),
                     16, 0, 16, 16, 55, 71);
        event.GetYaxis()->SetTitle("Z [cm]");
        event.GetXaxis()->SetTitle("X [cm]");
        event.GetZaxis()->SetTitle("ToT [2.5 ns]");
    }
    const unsigned int tDigits = 400;
    std::vector<TSFGDigit> eventsTime[tDigits];

    for (int j = 0; j < hitsFEB.size(); ++j) {
        eventsTime[(unsigned int)(hitsFEB[j].GetRisingEdgeTDC()/10)].push_back(hitsFEB[j]);
    }

    for (int i = 0; i < tDigits; ++i) {
        if (eventsTime[i].size() > 10){
            for (int j = 0; j < eventsTime[i].size(); ++j) {
                GlGeomPosition position = connectionMap.GetGlobalGeomPosition(feb, eventsTime[i][j].GetChannelNumber());
                int timeDif = eventsTime[i][j].GetFallingEdgeTDC() - eventsTime[i][j].GetRisingEdgeTDC();
                gtsSlot.Fill(eventsTime[i][j].GetGTSCounter());
                if (feb == 247){
                    event.Fill(position.x_, position.y_, timeDif);
                } else if (feb == 251){
                    event.Fill(position.z_, position.y_, timeDif);
                } else {
                    event.Fill(position.x_, position.z_, timeDif);
                }
                eventTime.Fill(eventsTime[i][j].GetRisingEdgeTDC());
            }
            if (i > 0) {
                for (int j = 0; j < eventsTime[i - 1].size(); ++j) {
                    GlGeomPosition position = connectionMap.GetGlobalGeomPosition(feb, eventsTime[i - 1][j].GetChannelNumber());
                    int timeDif = eventsTime[i - 1][j].GetFallingEdgeTDC() - eventsTime[i - 1][j].GetRisingEdgeTDC();
                    gtsSlot.Fill(eventsTime[i - 1][j].GetGTSCounter());
                    if (feb == 247) {
                        event.Fill(position.x_, position.y_, timeDif);
                    } else if (feb == 251) {
                        event.Fill(position.y_, position.z_, timeDif);
                    } else {
                        event.Fill(position.x_, position.z_, timeDif);
                    }
                    eventTime.Fill(eventsTime[i - 1][j].GetRisingEdgeTDC());
                }
            }
            if (i < tDigits) {
                for (int j = 0; j < eventsTime[i + 1].size(); ++j) {
                    GlGeomPosition position = connectionMap.GetGlobalGeomPosition(feb, eventsTime[i + 1][j].GetChannelNumber());
                    int timeDif = eventsTime[i + 1][j].GetFallingEdgeTDC() - eventsTime[i + 1][j].GetRisingEdgeTDC();
                    gtsSlot.Fill(eventsTime[i + 1][j].GetGTSCounter());
                    if (feb == 247) {
                        event.Fill(position.x_, position.y_, timeDif);
                    } else if (feb == 251) {
                        event.Fill(position.y_, position.z_, timeDif);
                    } else {
                        event.Fill(position.x_, position.z_, timeDif);
                    }
                    eventTime.Fill(eventsTime[i + 1][j].GetRisingEdgeTDC());
                }
            }
        }
    }

    return {gtsSlot,eventTime,event};
}

int main( int argc, char **argv ) {
    std::string stringBuf;
    int    intBuf;

    // The following shows how to use the MDargumentHandler class
    // to deal with the main arguments
    // Define the arguments
    MDargumentHandler argh("Example of unpacking application.");
    argh.Init();

    // Check the user arguments consistancy
    // All mandatory arguments should be provided and
    // There should be no extra arguments
    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}

    // Treat arguments, obtain values to be used later
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}

    if ( argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
    string filename = stringBuf;


    TFile *FileInput = new TFile((filename).c_str());
    if (!FileInput->IsOpen()){
        cerr << "Can not open file " << filename << endl;
        return 1;
    }

    string rootFileOutput=GetLocation((filename).c_str(), "_plots.root");
    rootFileOutput+="_events.root";
    cout << rootFileOutput << endl;
    TFile wfile(rootFileOutput.c_str(), "recreate");

    string mapFile = "../connection_map/connectionMap.txt";
    Connection_Map connectionMap(mapFile);
    try {
        connectionMap.Init();
    } catch (const exception& e) {
        cerr << "Unable to open file " << mapFile << endl;
        exit(1);
    }

    std::vector<ToaEventDummy>* FEBs[SFGD_FEBS_NUM];
    for (Int_t i=0;i<SFGD_FEBS_NUM;++i) {
        FEBs[i]=0;
    }

    TTree *AllEvents = (TTree*)FileInput->Get("AllEvents");
    ostringstream sFEBnum;
    string sFEB;
    for (int ih = 0; ih < SFGD_FEBS_NUM; ++ih) {
        sFEBnum.str("");
        sFEBnum << ih;
        sFEB = "FEB_"+sFEBnum.str();
        auto* br = AllEvents->GetListOfBranches()->FindObject((sFEB).c_str());
        if (br){
            cout << "Branch " <<(sFEB).c_str() <<" found."<<endl;
            AllEvents->SetBranchAddress((sFEB).c_str(), &FEBs[ih]);
        }
    }
    int nEntries = AllEvents->GetEntries();
    std::cout << "entries at Feb  is " << nEntries <<std::endl;

    TH1F eventLY247("EventsLY_247", "EventsLY_247",4000, 0 ,4000);
    TH1F eventLY251("EventsLY_251", "EventsLY_251",4000, 0 ,4000);
    TH1F eventLY253("EventsLY_253", "EventsLY_253",4000, 0 ,4000);

    for (int i = 0; i < nEntries -1; ++i) {
        AllEvents->GetEntry(i);

        std::vector<TSFGDigit> temp253 =  FEBs[253]->back().GetHits();
        unsigned int evNum = FEBs[253]->back().GetEventNumber();
        ostringstream evNumString;
        evNumString << evNum;
//        if (temp253.size()>16 ){
            unsigned int cosmicHits253 = 0;
            for (int j = 0; j < temp253.size(); ++j) {
                int timeDif = temp253[j].GetFallingEdgeTDC() - temp253[j].GetRisingEdgeTDC();
                if (timeDif > 5){
                    ++cosmicHits253;
                }
            }
            if (cosmicHits253 >10) {
                TDirectory *hitDir = wfile.mkdir((evNumString.str()).c_str());
                hitDir->cd();
                {
                    std::tuple<TH1F, TH1F, TH2F> tempTH = GetPictures(evNumString, temp253, 253, connectionMap);
                    std::get<0>(tempTH).Write();
                    std::get<0>(tempTH).Delete();
                    std::get<1>(tempTH).Write();
                    std::get<1>(tempTH).Delete();
                    std::get<2>(tempTH).Write();
                    std::get<2>(tempTH).Delete();
                }
                for (int j = 0; j < temp253.size(); ++j) {
                    if (temp253[j].GetHighGainADC()>0){
                        eventLY253.Fill(temp253[j].GetHighGainADC());
                    }
                }
            }
            unsigned int cosmicHits247 = 0;
            std::vector<TSFGDigit> temp247 = FEBs[247]->back().GetHits();
            for (int j = 0; j < temp247.size(); ++j) {
                int timeDif = temp247[j].GetFallingEdgeTDC() - temp247[j].GetRisingEdgeTDC();
                if (timeDif > 5){
                    ++cosmicHits247;
                }
            }
            if (cosmicHits247 >10) {
                {
                    std::tuple<TH1F, TH1F, TH2F> tempTH = GetPictures(evNumString, temp247, 247, connectionMap);
                    std::get<0>(tempTH).Write();
                    std::get<0>(tempTH).Delete();
                    std::get<1>(tempTH).Write();
                    std::get<1>(tempTH).Delete();
                    std::get<2>(tempTH).Write();
                    std::get<2>(tempTH).Delete();
                }
                for (int j = 0; j < temp247.size(); ++j) {
                    if (temp247[j].GetHighGainADC()>0){
                        eventLY247.Fill(temp247[j].GetHighGainADC());
                    }
                }
            }
            unsigned int cosmicHits251 = 0;
            std::vector<TSFGDigit> temp251 = FEBs[251]->back().GetHits();
            for (int j = 0; j < temp251.size(); ++j) {
                int timeDif = temp251[j].GetFallingEdgeTDC() - temp251[j].GetRisingEdgeTDC();
                if (timeDif > 5){
                    ++cosmicHits251;
                }
            }
            if (cosmicHits251 >10) {
                {
                    std::tuple<TH1F, TH1F, TH2F> tempTH = GetPictures(evNumString, temp251, 251, connectionMap);
                    std::get<0>(tempTH).Write();
                    std::get<0>(tempTH).Delete();
                    std::get<1>(tempTH).Write();
                    std::get<1>(tempTH).Delete();
                    std::get<2>(tempTH).Write();
                    std::get<2>(tempTH).Delete();
                }
                for (int j = 0; j < temp251.size(); ++j) {
                    if (temp251[j].GetHighGainADC()>0){
                        eventLY251.Fill(temp251[j].GetHighGainADC());
                    }
                }
            }

//        }
//        cout << temp247.size() << " " << temp251.size() << " " << temp253.size()<<endl;
        temp253.clear();
    }
    wfile.cd();
    eventLY247.Write();
    eventLY251.Write();
    eventLY253.Write();
    FileInput->Close();
    wfile.Write();
    wfile.Close();
    return 0;
}