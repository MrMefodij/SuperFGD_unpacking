//
//  Created by Maria on 08.10.2022 kolupanova@inr.ru
//

#ifndef FILES_READER_H
#define FILES_READER_H
#pragma once
#include <fstream>
#include <iomanip>
#include <sstream>
#include <set> 
#include <map>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>

#include "MDdataWordSFGD.h"

std::string GetLocation(std::string str, std::string path);

enum HG_LG_input {
    HG = 2,
    LG = 3
};

class File_Reader {
public:
    /// Fill histograms
    File_Reader() = default;

    ~File_Reader() {
        delete _dataPtr;
    }

    /// Function for calibration study
    void ReadFile_for_Calibration(const std::string &sFileName, std::vector<std::vector<TH1F *>> &hFEBCH,
                                  HG_LG_input hg_lg = HG_LG_input::HG);

    /// Function for baseline study
    void ReadFile_for_Baseline(const std::string &sFileName, std::vector<std::vector<unsigned int>> &hFEBCH_HG,
                               std::vector<std::vector<unsigned int>> &hFEBCH_LG);

    /// Function for threshold study
    void ReadFile_for_Threshold(const std::string &sFileName, std::vector<TH1F *> &hFEBCH);

    /// Create directory
    std::map<unsigned int, unsigned int> GetFEBchannel() const { return _feb_channel; }

    std::set<unsigned int> GetFEBNumbers() const { return _boad_Id_set; }

    unsigned int GetFEBNumber() const { return _board_Id; }

    unsigned int GetChannelNumber() const { return _channel_Id; }

private:
    std::set<unsigned int> _boad_Id_set;
    unsigned int *_dataPtr = new unsigned int;
    unsigned int _board_Id = 0;
    unsigned int _channel_Id = 0;
    std::map<unsigned int, unsigned int> _feb_channel;
    unsigned int OcbEvNum = 0;
};

#endif /* Files_Reader_h */
