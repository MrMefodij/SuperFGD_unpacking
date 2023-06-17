//
//  Files_Reader.h
//  SFGD_collibration
//
//  Created by Maria on 08.10.2022.
//

#ifndef Files_Reader_h
#define Files_Reader_h

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <set> 

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>

#include "MDdataWordSFGD.h"
#include "MDpartEventSFGD.h"

#define SFGD_NFEB 290

std::string GetMCRnumber(std::string str);
std::string GetSlotNumber(std::string str);
std::string GetDir(std::string str);
std::string GetLocation(std::string str, std::string path);

class File_Reader{
public:
    // Fill histograms
    void ReadFile(const std::string& sFileName);
    // Create directory
    std::set<Int_t> GetFEBNumbers(){return NFEB;}
    TH1F* Get_hFEBCH(const int& ih,const int& ich);
    ~File_Reader(){}
private:
    std::set<Int_t> NFEB;
    std::string sCh;
    uint32_t* dataPtr = new uint32_t;
    TH1F *hFEBCH[SFGD_NFEB][SFGD_FEB_NCHANNELS];
    
};
#endif /* Files_Reader_h */
