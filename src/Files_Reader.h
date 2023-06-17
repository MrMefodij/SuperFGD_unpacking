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
    ~File_Reader() = default;
    void ReadFile(const std::string& sFileName);
    // Create directory
    std::set<Int_t> GetFEBNumbers(){return NFEB;}
    TH1F* Get_hFEBCH(const int& ih,const int& ich) {return hFEBCH[ih][ich];}
private:
    // std::string sCh;
    std::set<Int_t> NFEB;
    uint32_t* dataPtr = new uint32_t;
    TH1F *hFEBCH[SFGD_FEBS_NUM][SFGD_FEB_NCHANNELS];
    Int_t FEB_number ;
    
};
#endif /* Files_Reader_h */
