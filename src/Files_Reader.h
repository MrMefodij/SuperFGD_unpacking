#ifndef FILES_READER_H
#define FILES_READER_H
#include <fstream>
#include <iomanip>
#include <sstream>
#include <set> 

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>

#include "MDdataWordSFGD.h"
#include "MDpartEventSFGD.h"


std::string GetLocation(std::string str, std::string path);

class File_Reader{
public:
    // Fill histograms
    File_Reader() = default;
    ~File_Reader(){
        delete _dataPtr;
    }
    void ReadFile(const std::string& sFileName, std::vector<TH1F*>& hFEBCH, int HG_LG);
    void ReadFile(const std::string& sFileName, std::vector<std::vector<TH1F*>>& hFEBCH_HG,std::vector<std::vector<TH1F*>>& hFEBCH_LG);
    void ReadFile(const std::string& sFileName, std::vector<TH1F*>& hFEBCH);
    // Create directory
    std::set<unsigned int> GetFEBNumbers(){return _boad_Id_set;}
    unsigned int GetFEBNumber(){return _board_Id;}
    unsigned int GetChannelNumber(){return _channel_Id;}
private:
    // std::string sCh;
    std::set<unsigned int> _boad_Id_set;
    unsigned int* _dataPtr = new unsigned int;
    unsigned int _board_Id ;
    unsigned int _slot_Id ;
    unsigned int _channel_Id ;
    
};
#endif /* Files_Reader_h */
