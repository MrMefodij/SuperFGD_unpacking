
//  Created by Maria on 08.10.2022.
//
#include <stdio.h>
#include <stdio.h>
#include "Files_Reader.h"
#include "BaseLine.h"
#include <typeinfo>


string GetLocation(string str, string path){
     
    int i = str.rfind(path.c_str());
    string way = str.substr(0,i);
    return way;
}
void File_Reader::ReadFile(const std::string& sFileName, vector<TH1F*>& hFEBCH, int HG_LG){
    ifstream ifs(sFileName.c_str());
    while (!ifs.eof()) {
        ifs.read((char*)_dataPtr, 4 );
        MDdataWordSFGD dw(_dataPtr);
        switch (dw.GetDataType()) {
            case MDdataWordSFGD::GateHeader:
                _boad_Id_set.insert(dw.GetBoardId());
                _board_Id = dw.GetBoardId();
                break;
            case MDdataWordSFGD::ChargeMeas:
                if (dw.GetAmplitudeId()==HG_LG){
                    hFEBCH[_board_Id * SFGD_FEB_NCHANNELS + dw.GetChannelId()]->Fill(dw.GetAmplitude());
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
        ifs.read((char*)_dataPtr, 4 );
        MDdataWordSFGD dw(_dataPtr);
        switch (dw.GetDataType()) {
            case MDdataWordSFGD::GateHeader:
                _board_Id = dw.GetBoardId();
                _boad_Id_set.insert(_board_Id );
                _slot_Id = _board_Id & 0x0f ;
                break;
            case MDdataWordSFGD::ChargeMeas:
                if (dw.GetAmplitudeId()==2){
                    hFEBCH_HG[_slot_Id][dw.GetChannelId()]->Fill(dw.GetAmplitude());
                }
                if (dw.GetAmplitudeId()==3){
                    hFEBCH_LG[_slot_Id][dw.GetChannelId()]->Fill(dw.GetAmplitude());
                }
                break;
            default:
                break;
        }
    }
    ifs.close();
};


