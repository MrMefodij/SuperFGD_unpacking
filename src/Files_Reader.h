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
#include <vector>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>

#include "MDdataWordSFGD.h"
#include "MDpartEventSFGD.h"

std::string GetMCRnumber(std::string str);
std::string GetSlotNumber(std::string str);
std::string GetDir(std::string str);
std::string GetLocation(std::string str, std::string path);

class File_Reader{
public:
    // Считываем слова с помощью MDdataWordSFGD
    std::string Read_MDdataWordSFGD(const std::string& sFileName);
    // Проходимся по данным и заполняем гистограммы
    void ReadFile(const std::string& sFileName);
    // Create directory
    TDirectory * Create_directory(TFile& wfile);
    
    TH1F* Print_hFEBCH(const int& ich);
    
    Int_t Fill_FEB();
    void Fit_hFEBCH();
private:
    Int_t channels_num = SFGD_FEB_NCHANNELS;
    
    std::string MCRnum;
    std::string Slotnum;
    std::string gainFileOutput;
    std::ostringstream sChnum;
    std::string sCh;
    uint32_t* dataPtr = new uint32_t;
    int dwCount = 0;
    TH1F *hFEBCH[SFGD_FEB_NCHANNELS];// = new TH1F[channels_num];
    
};
#endif /* Files_Reader_h */
