//
//  Created by Maria on 08.10.2022 kolupanova@inr.ru
//

#include "Files_Reader.h"
#include "SFGD_defines.h"


std::string GetLocation(std::string str, std::string path){
    int i = str.rfind(path.c_str());
    std::string way = str.substr(0,i);
    return way;
}

void File_Reader::ReadFile_for_Calibration(const std::string &sFileName, std::vector<std::vector<TH1F *>> &hFEBCH, HG_LG_input hg_lg) {
    std::ifstream ifs(sFileName.c_str());
    _boad_Id_set.clear();
//    unsigned int dwCount(0);
    while (!ifs.eof()) {
        try {
            ifs.read((char *) _dataPtr, 4);
            MDdataWordSFGD dw(_dataPtr);

//            ++dwCount;
            switch (dw.GetDataType()) {
                case MDdataWordSFGD::GateHeader:
                    _board_Id = dw.GetBoardId();
                    _boad_Id_set.insert(_board_Id);
                    break;
                case MDdataWordSFGD::ChargeMeas:
                    if (dw.GetAmplitudeId() == hg_lg) {
                        if (hFEBCH.size() == SFGD_SLOT)
                            hFEBCH[_board_Id & 0x0f][dw.GetChannelId()]->Fill(dw.GetAmplitude());
                        else {
                            hFEBCH[_board_Id][dw.GetChannelId()]->Fill(dw.GetAmplitude());
                        }
                    }
                    break;
                case MDdataWordSFGD::OcbGateTrailer:
                    while (!ifs.eof() && dw.GetDataType() != MDdataWordSFGD::OcbGateHeader) {
                        ifs.read((char *) _dataPtr, 4);
                        MDdataWordSFGD dw(_dataPtr);
//                         ++dwCount;
                    }
                    ++OcbEvNum;
                    break;
                case MDdataWordSFGD::OcbGateHeader:
                    ++OcbEvNum;
                    break;

                default:
                    break;

            }
        }
        catch(...){
            std::cerr<<"Error"<<std::endl;
        }
    }
    std::cout <<"OcbEvNum: "<<OcbEvNum<<std::endl;
    ifs.close();
};

void File_Reader::ReadFile_for_Baseline(const std::string &sFileName, std::vector<std::vector<unsigned int>> &hFEBCH_HG,std::vector<std::vector<unsigned int>> &hFEBCH_LG) {
    std::ifstream ifs(sFileName.c_str());
    _boad_Id_set.clear();
    while (!ifs.eof()) {
        ifs.read((char*)_dataPtr, 4 );
        MDdataWordSFGD dw(_dataPtr);
        switch (dw.GetDataType()) {
            case MDdataWordSFGD::GateHeader:
                _board_Id = dw.GetBoardId();
//                if (!(_board_Id == 0 && dw.GetGateType() == 0 && dw.GetGateNumber() == 0))
                    _boad_Id_set.insert(_board_Id);
                break;
            case MDdataWordSFGD::ChargeMeas:
                if (dw.GetAmplitudeId()==2){
                    if(hFEBCH_HG.size() == SFGD_SLOT * (SFGD_FEB_NCHANNELS + 1) ) {
                        hFEBCH_HG[(_board_Id & 0x0f) * SFGD_FEB_NCHANNELS + dw.GetChannelId()][dw.GetAmplitude() /4] += 1;
                    }
                    else{
                        hFEBCH_HG[_board_Id * SFGD_FEB_NCHANNELS + dw.GetChannelId()][dw.GetAmplitude() / 4] += 1;
                    }
                }
                if (dw.GetAmplitudeId()==3){
                    if(hFEBCH_LG.size() == SFGD_SLOT * (SFGD_FEB_NCHANNELS + 1) )
                        hFEBCH_LG[(_board_Id & 0x0f) * SFGD_FEB_NCHANNELS + dw.GetChannelId()][dw.GetAmplitude() / 4] += 1;
                    else{
                        hFEBCH_LG[_board_Id * SFGD_FEB_NCHANNELS + dw.GetChannelId()][dw.GetAmplitude() / 4] += 1;
                    }
                }
                break;
            case MDdataWordSFGD::OcbGateTrailer:
                while (!ifs.eof() && dw.GetDataType() != MDdataWordSFGD::OcbGateHeader) {
                    ifs.read((char *) _dataPtr, 4);
                    MDdataWordSFGD dw(_dataPtr);
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
                    if (!(_board_Id == 0 && dw.GetGateType() == 0 && dw.GetGateNumber() == 0)) {
                        if (hFEBCH.size() == SFGD_SLOT)
                            hFEBCH[_board_Id & 0x0f]->Fill(dw.GetAmplitude());
                        else {
                            hFEBCH[_board_Id]->Fill(dw.GetAmplitude());
                        }
                    }
                }
                break;
            case MDdataWordSFGD::OcbGateTrailer:
                while (!ifs.eof() && dw.GetDataType() != MDdataWordSFGD::OcbGateHeader) {
                    ifs.read((char *) _dataPtr, 4);
                    MDdataWordSFGD dw(_dataPtr);
                }
                break;
            default:
                break;
        }
    }
    ifs.close();
};
