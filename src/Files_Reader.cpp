
//  Created by Maria on 08.10.2022.
//
#include <stdio.h>
#include <stdio.h>
#include "Files_Reader.h"
#include <typeinfo>
using namespace std;


string GetLocation(string str, string path){
     
    int i = str.rfind(path.c_str());
    string way = str.substr(0,i);
    return way;
}

void File_Reader::ReadFile(const string& sFileName){
    for(int i = 0; i < SFGD_FEBS_NUM; i++){
        for(int j = 0; j < SFGD_FEB_NCHANNELS; j++){
            std::string sCh = "FEB_"+std::to_string(i)+"_Channel_"+std::to_string(j);
            hFEBCH[i][j] = new TH1F(sCh.c_str(),sCh.c_str(),  701, 0, 700);
        }
    }
    ifstream ifs(sFileName.c_str());
    while (!ifs.eof()) {
        ifs.read((char*)dataPtr, 4 );
        MDdataWordSFGD dw(dataPtr);
        switch (dw.GetDataType()) {
            case MDdataWordSFGD::GateHeader:
                NFEB.insert(dw.GetBoardId());
                FEB_number = dw.GetBoardId();
                // std::cout<<"FEB# " << FEB_number<< std::endl;
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

