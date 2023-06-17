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
 * along with BabyMINDdaq.  If not, see <http://www.gnu.org/licenses/>.
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

#define SFGD_FEBS_NUM 280


std::tuple<TH1F,TH1F,TH2F> GetPictures(const ostringstream& evNumString, const std::vector<TSFGDigit>& hitsFEB, const unsigned int feb,const Connection_Map& map){
    ostringstream febStr;
    febStr<<feb;
    TH1F gtsSlot(("GtsSlot_" + evNumString.str() + "_" + febStr.str()).c_str(),("GtsSlot_" + evNumString.str() + "_" + febStr.str()).c_str(),5,0,5 );
    TH1F eventTime(("EventTime_" + evNumString.str() + "_" + febStr.str()).c_str(), ("EventTime_" + evNumString.str() + "_" + febStr.str()).c_str(),4000, 0 ,4000);
//    TH2F event(("Event_" + evNumString.str() + "_" + febStr.str()).c_str(), ("Event_" + evNumString.str() + "_" + febStr.str()).c_str(),16, 0, 15, 16, 55, 70);
    TH2F event;
    if (feb == 247){
        event = TH2F(("Event_" + evNumString.str() + "_" + febStr.str()).c_str(), ("Event_" + evNumString.str() + "_" + febStr.str()).c_str(),
                     16, 0, 15, 16, 40, 55);
    } else if (feb == 251){
        event = TH2F(("Event_" + evNumString.str() + "_" + febStr.str()).c_str(), ("Event_" + evNumString.str() + "_" + febStr.str()).c_str(),
                     16, 40, 55, 16, 55, 70);
    } else {
        event = TH2F(("Event_" + evNumString.str() + "_" + febStr.str()).c_str(), ("Event_" + evNumString.str() + "_" + febStr.str()).c_str(),
                     16, 0, 15, 16, 55, 70);
    }
    const unsigned int tDigits = 400;
    std::vector<TSFGDigit> eventsTime[tDigits];

    for (int j = 0; j < hitsFEB.size(); ++j) {
        eventsTime[(unsigned int)(hitsFEB[j].GetRisingEdgeTDC()/10)].push_back(hitsFEB[j]);
    }

    for (int i = 0; i < tDigits; ++i) {
        if (eventsTime[i].size() > 10){
            for (int j = 0; j < eventsTime[i].size(); ++j) {
                GlGeomPosition position = map.GetGlobalGeomPosition(feb, eventsTime[i][j].GetChannelNumber());
                int timeDif = eventsTime[i][j].GetFallingEdgeTDC() - eventsTime[i][j].GetRisingEdgeTDC();
                gtsSlot.Fill(eventsTime[i][j].GetGTSCounter());
                if (feb == 247){
                    event.Fill(position.x_, position.y_, timeDif);
                } else if (feb == 251){
                    event.Fill(position.y_, position.z_, timeDif);
                } else {
                    event.Fill(position.x_, position.z_, timeDif);
                }
                eventTime.Fill(eventsTime[i][j].GetRisingEdgeTDC());
            }
            if (i > 0) {
                for (int j = 0; j < eventsTime[i - 1].size(); ++j) {
                    GlGeomPosition position = map.GetGlobalGeomPosition(feb, eventsTime[i - 1][j].GetChannelNumber());
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
                    GlGeomPosition position = map.GetGlobalGeomPosition(feb, eventsTime[i + 1][j].GetChannelNumber());
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
    argh.AddArgument("help","print this message","h");
    argh.AddArgument("directory","Path for a data file","d","<string>","." );
    argh.AddArgument("file","Name of a data file","f","<string>","mandatory");
    argh.AddArgument("begin","Initial position in input file","b","<int>","0");
    argh.AddArgument("nwords","Number of data words to be processed","n","<int>","0");

    // Check the user arguments consistancy
    // All mandatory arguments should be provided and
    // There should be no extra arguments
    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}

    // Treat arguments, obtain values to be used later
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}

    if ( argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
    string filename = stringBuf;

    if ( argh.GetValue("begin", intBuf) != MDARGUMENT_STATUS_OK ) return -1;
    int pos = intBuf*4;

    if ( argh.GetValue("nwords", intBuf) != MDARGUMENT_STATUS_OK ) return -1;
    int nWords = intBuf;


    TFile *FileInput = new TFile((filename).c_str());
    if (!FileInput->IsOpen()){
        cerr << "Can not open file " << filename << endl;
        return 1;
    }

    string rootFileOutput=GetLocation((filename).c_str(), "_plots.root");
    cout << rootFileOutput<<endl;
    rootFileOutput+="_events.root";
    TFile wfile(rootFileOutput.c_str(), "recreate");

    string mapFile = "../connection_map/map.txt";
    Connection_Map map(mapFile);
    try {
        map.Init();
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
        AllEvents->SetBranchAddress((sFEB).c_str(), &FEBs[ih]);
    }
    int nEntries = AllEvents->GetEntries();
    std::cout << "entries at Feb  is " << nEntries <<std::endl;

    for (int i = 0; i < nEntries -1; ++i) {
        AllEvents->GetEntry(i);

        std::vector<TSFGDigit> temp253 =  FEBs[253]->back().GetHits();
        unsigned int evNum = FEBs[253]->back().GetEventNumber();
        ostringstream evNumString;
        evNumString << evNum;
        if (temp253.size()>16 ){
            unsigned int cosmicHits = 0;
            for (int j = 0; j < temp253.size(); ++j) {
                int timeDif = temp253[j].GetFallingEdgeTDC() - temp253[j].GetRisingEdgeTDC();
                if (timeDif > 5){
                    ++cosmicHits;
                }
            }
            if (cosmicHits >10) {
                TDirectory *hitDir =  wfile.mkdir((evNumString.str()).c_str());
                hitDir->cd();
                {
                    std::tuple<TH1F, TH1F, TH2F> tempTH = GetPictures(evNumString, temp253, 253, map);
                    std::get<0>(tempTH).Write();
                    std::get<0>(tempTH).Delete();
                    std::get<1>(tempTH).Write();
                    std::get<1>(tempTH).Delete();
                    std::get<2>(tempTH).Write();
                    std::get<2>(tempTH).Delete();
                }

                {
                    std::vector<TSFGDigit> temp247 = FEBs[247]->back().GetHits();
                    std::tuple<TH1F, TH1F, TH2F> tempTH = GetPictures(evNumString, temp247, 247, map);
                    std::get<0>(tempTH).Write();
                    std::get<0>(tempTH).Delete();
                    std::get<1>(tempTH).Write();
                    std::get<1>(tempTH).Delete();
                    std::get<2>(tempTH).Write();
                    std::get<2>(tempTH).Delete();
                }

                {
                    std::vector<TSFGDigit> temp251 = FEBs[251]->back().GetHits();
                    std::tuple<TH1F, TH1F, TH2F> tempTH = GetPictures(evNumString, temp251,251, map);
                    std::get<0>(tempTH).Write();
                    std::get<0>(tempTH).Delete();
                    std::get<1>(tempTH).Write();
                    std::get<1>(tempTH).Delete();
                    std::get<2>(tempTH).Write();
                    std::get<2>(tempTH).Delete();
                }
            }

        }
//        cout << temp247.size() << " " << temp251.size() << " " << temp253.size()<<endl;
        temp253.clear();
    }
    FileInput->Close();
    wfile.Write();
    wfile.Close();
    return 0;
}