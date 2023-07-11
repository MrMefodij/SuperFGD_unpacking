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
    void ReadFile(const std::string& sFileName, vector<TH1F*>& hFEBCH, int HG_LG = 2);
    void ReadFile(const std::string& sFileName, vector<vector<TH1F*>>& hFEBCH_HG,vector<vector<TH1F*>>& hFEBCH_LG);
    // Create directory
    std::set<unsigned int> GetFEBNumbers(){return _boad_Id_set;}
private:
    // std::string sCh;
    std::set<unsigned int> _boad_Id_set;
    unsigned int* _dataPtr = new unsigned int;
    unsigned int _board_Id ;
    
};
#endif /* Files_Reader_h */
