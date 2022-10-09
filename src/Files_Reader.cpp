//
//  Files_Reader.cpp
//  SFGD_collibration
//
//  Created by Maria on 08.10.2022.
//
#include <stdio.h>
#include <stdio.h>
#include "Files_Reader.h"
#include <typeinfo>
using namespace std;
string GetMCRnumber(string str) {
    int i = str.find("_MCR_");
    string mcr = str.substr(i+5,1);
    return mcr;
}

string GetSlotNumber(string str){
    int i = str.find("_Slot_");
    string slot = str.substr(i+6,1);
    return slot;
}

string GetDir(string str){
    int i = str.rfind("/");
    string way = str.substr(0,i+1);
    return way;
}

string GetLocation(string str){
     
    int i = str.rfind("_Slot_");
    string way = str.substr(0,i);
    return way;
}

string File_Reader::Read_MDdataWordSFGD(const std::string& sFileName){
        MCRnum = GetMCRnumber(sFileName.c_str());
        Slotnum = GetSlotNumber(sFileName.c_str());
        gainFileOutput=GetDir(sFileName.c_str());
        gainFileOutput+="MCR_"+MCRnum +"_Slot_"+Slotnum +"_gain.txt";
        return gainFileOutput;
    };

void File_Reader::ReadFile(const string& sFileName){
    for (Int_t ih=0; ih < channels_num;ih++){
        sChnum.str("");
        sChnum << ih;
        sCh = "Channel_"+sChnum.str();
        hFEBCH[ih] = new TH1F(sCh.c_str(),sCh.c_str(),  701, 0, 700);
    }
    ifstream ifs(sFileName.c_str());
    // заполняем данными
    while (!ifs.eof()) {
        ifs.read((char*)dataPtr, 4 );
        MDdataWordSFGD dw(dataPtr);
        switch (dw.GetDataType()) {
            case MDdataWordSFGD::ChargeMeas:
                if (dw.GetAmplitudeId()==2){
                    hFEBCH[dw.GetChannelId()]->Fill(dw.GetAmplitude());
                }
            break;
        default:
            break;
        }
     ++dwCount;
    }
    ifs.close();
};


TDirectory* File_Reader::Create_directory(TFile& wfile){
    return wfile.mkdir(("MCR_"+MCRnum+"_Slot_"+Slotnum).c_str());
};

Int_t File_Reader::Fill_FEB(){return 8*(stoi( MCRnum )) + stoi( Slotnum );}

TH1F* File_Reader::Print_hFEBCH(const int& ich){
    return hFEBCH[ich];
};
