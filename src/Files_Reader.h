#ifndef FILES_READER_H
#define FILES_READER_H

#include <fstream>
#include <iomanip>
#include <sstream>
#include <set> 
#include <map>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>

#include "MDdataWordSFGD.h"
#include "SFGD_defines.h"


std::string GetLocation(std::string str, std::string path);

class File_Reader{
public:
    /// Fill histograms
    File_Reader() = default;
    ~File_Reader(){
        delete _dataPtr;
    }
    /// Function for calibration study
    void ReadFile_for_Calibration(const std::string& sFileName, std::vector<std::vector<TH1F*>>& hFEBCH, int HG_LG = 2);
    /// Function for baseline study
    void ReadFile_for_Baseline(const std::string& sFileName, std::vector<std::vector<TH1F*>>& hFEBCH_HG,std::vector<std::vector<TH1F*>>& hFEBCH_LG);
    /// Function for threshold study
    void ReadFile_for_Threshold(const std::string& sFileName, std::vector<TH1F*>& hFEBCH);

    /// Create directory
    std::map<unsigned int, unsigned int> GetFEBchannel() {return _feb_channel;}
    std::set<unsigned int> GetFEBNumbers(){return _boad_Id_set;}
    unsigned int GetFEBNumber(){return _board_Id;}
    unsigned int GetChannelNumber(){return _channel_Id;}
private:
    std::set<unsigned int> _boad_Id_set;
    unsigned int* _dataPtr = new unsigned int;
    unsigned int _board_Id ;
    unsigned int _channel_Id = 0 ;
    std::map<unsigned int, unsigned int> _feb_channel;
    
};
#endif /* Files_Reader_h */
