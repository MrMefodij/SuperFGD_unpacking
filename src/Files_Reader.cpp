
//  Created by Maria on 08.10.2022.
//

#include "Files_Reader.h"
#include "BaseLine.h"


std::string GetLocation(std::string str, std::string path){
     
    int i = str.rfind(path.c_str());
    std::string way = str.substr(0,i);
    return way;
}
void File_Reader::ReadFile_for_Calibration(const std::string &sFileName, std::vector<std::vector<TH1F *>> &hFEBCH, int HG_LG) {
    std::ifstream ifs(sFileName.c_str());
    _boad_Id_set.clear();
    int dwCount(0);
    while (!ifs.eof()) {
        try {
            ifs.read((char *) _dataPtr, 4);
            MDdataWordSFGD dw(_dataPtr);
            ++dwCount;
            switch (dw.GetDataType()) {
                case MDdataWordSFGD::GateHeader:
                    _board_Id = dw.GetBoardId();
                    if(_board_Id>=16 && _board_Id<=29 )
                        _boad_Id_set.insert(_board_Id);

                    break;
                case MDdataWordSFGD::ChargeMeas:
                    if (dw.GetAmplitudeId() == HG_LG) {
                        if(_board_Id>=16 && _board_Id<=29)
                            hFEBCH[_board_Id & 0x0f][dw.GetChannelId()]->Fill(dw.GetAmplitude());
                    }
                    break;
                default:
                    break;
            }
        }
        catch(...){
            std::cerr<<"Error"<<std::endl;
        }

    }
    ifs.close();
};

void File_Reader::ReadFile_for_Baseline(const std::string &sFileName, std::vector<std::vector<TH1F *>> &hFEBCH_HG,std::vector<std::vector<TH1F *>> &hFEBCH_LG) {
    std::ifstream ifs(sFileName.c_str());
    _boad_Id_set.clear();
    while (!ifs.eof()) {
        ifs.read((char*)_dataPtr, 4 );
        MDdataWordSFGD dw(_dataPtr);
        switch (dw.GetDataType()) {
            case MDdataWordSFGD::GateHeader:
                _board_Id = dw.GetBoardId();
                if(_board_Id>=16 && _board_Id<=29)
                _boad_Id_set.insert(_board_Id );
                break;
            case MDdataWordSFGD::ChargeMeas:
                if (dw.GetAmplitudeId()==2){
                    if(_board_Id>=16 && _board_Id<=29)
                    hFEBCH_HG[_board_Id & 0x0f][dw.GetChannelId()]->Fill(dw.GetAmplitude());
                }
                if (dw.GetAmplitudeId()==3){
                    if(_board_Id>=16 && _board_Id<=29)
                    hFEBCH_LG[_board_Id & 0x0f][dw.GetChannelId()]->Fill(dw.GetAmplitude());
                }
                break;
            default:
                break;
        }
    }
    ifs.close();
};

void File_Reader::ReadFile_for_Threshold(const std::string &sFileName, std::vector<TH1F *> &hFEBCH) {
    std::ifstream ifs(sFileName.c_str());
    _boad_Id_set.clear();
    while (!ifs.eof()) {
        ifs.read((char *) _dataPtr, 4);
        MDdataWordSFGD dw(_dataPtr);
        switch (dw.GetDataType()) {
            case MDdataWordSFGD::GateHeader:
                _board_Id = dw.GetBoardId();
                _boad_Id_set.insert(_board_Id);
                break;
            case MDdataWordSFGD::ChargeMeas:
                if (dw.GetAmplitudeId() == 2) {
                    _feb_channel[_board_Id] = _channel_Id;
                    _channel_Id = dw.GetChannelId();
                    hFEBCH[_board_Id & 0x0f]->Fill(dw.GetAmplitude());
                }
                break;
            default:
                break;
        }
    }
    ifs.close();
};