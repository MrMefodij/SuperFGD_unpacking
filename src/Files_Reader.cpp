
//  Created by Maria on 08.10.2022.
//
#include <stdio.h>
#include <stdio.h>
#include "Files_Reader.h"
#include <typeinfo>


string GetLocation(string str, string path){
     
    int i = str.rfind(path.c_str());
    string way = str.substr(0,i);
    return way;
}
void File_Reader::ReadFile(const std::string& sFileName, vector<vector<TH1F*>>& hFEBCH, int HG_LG){
    ifstream ifs(sFileName.c_str());
    while (!ifs.eof()) {
        ifs.read((char*)dataPtr, 4 );
        MDdataWordSFGD dw(dataPtr);
        switch (dw.GetDataType()) {
            case MDdataWordSFGD::GateHeader:
                NFEB.insert(dw.GetBoardId());
                FEB_number = dw.GetBoardId();
                break;
            case MDdataWordSFGD::ChargeMeas:
                if (dw.GetAmplitudeId()==2){
                    hFEBCH[FEB_number][dw.GetChannelId()]->Fill(dw.GetAmplitude());
                }
            break;
        default:
            break;
        }
    }
    ifs.close();
};

void File_Reader::ReadFile(const std::string& sFileName, vector<vector<TH1F*>>& hFEBCH_HG,vector<vector<TH1F*>>& hFEBCH_LG ){
    ifstream ifs(sFileName.c_str());
    while (!ifs.eof()) {
        ifs.read((char*)dataPtr, 4 );
        MDdataWordSFGD dw(dataPtr);
        switch (dw.GetDataType()) {
            case MDdataWordSFGD::GateHeader:
                if(dw.GetBoardId() == 247){
                    NFEB.insert(0);
                    FEB_number = 0;
                }
                if(dw.GetBoardId() == 251){
                    NFEB.insert(1);
                    FEB_number = 1;
                }
                if(dw.GetBoardId() == 253){
                    NFEB.insert(2);
                    FEB_number = 2;
                }
                break;
            case MDdataWordSFGD::ChargeMeas:
                if (dw.GetAmplitudeId()==2){
                    hFEBCH_HG[FEB_number][dw.GetChannelId()]->Fill(dw.GetAmplitude());
                }
                if (dw.GetAmplitudeId()==3){
                    hFEBCH_LG[FEB_number][dw.GetChannelId()]->Fill(dw.GetAmplitude());
                }
                break;
            default:
                break;
        }
    }
    ifs.close();
};


