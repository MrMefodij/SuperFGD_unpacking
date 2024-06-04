//
// Created by Maria on 12.07.2023 kolupanova@inr.ru
//

#include <iostream>
#include <thread>
#include <mutex>
#include <TTree.h>
#include <TLine.h>

#include "Files_Reader.h"
#include "MDargumentHandler.h"
#include "SFGD_defines.h"

std::mutex mtx;

void ReadDirectory(TFile* wfile, std::vector<std::vector<TH1F *>>&hFEBCH, std::vector<unsigned int> &DAC, const std::string& s,
                   unsigned int file_num) {
    {
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << "Reading " << s << "...." << std::endl;
    }
    File_Reader file_reader;
    file_reader.ReadFile_for_Threshold(s, hFEBCH[file_num]);
    auto NFEBs = file_reader.GetFEBNumbers();
    auto feb_channel = file_reader.GetFEBchannel();
    for (auto board_Id: NFEBs) {
        std::lock_guard<std::mutex> guard(mtx);
        std::string feb_num = "FEB_" + std::to_string(board_Id);
        if (wfile->GetDirectory(feb_num.c_str()) == nullptr) {
            wfile->mkdir(feb_num.c_str());
        }
        wfile->cd(feb_num.c_str());
        if (hFEBCH[file_num][board_Id]->GetEntries() > 1)
            hFEBCH[file_num][board_Id]->Write((std::to_string(DAC[file_num])).c_str());
    }
    {
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << "Finished " << s << "...." << std::endl;
    }
}



void Threshold_St(const std::vector<std::string>& vFile_Threshold,const std::string& stringBuf, unsigned int ASIC_num){
    std::vector<std::vector<TH1F *>> hFEBCH(vFile_Threshold.size(), std::vector<TH1F *>(SFGD_FEBS_NUM));
    std::vector<unsigned int> DAC;

    for (int i = 0; i < vFile_Threshold.size(); ++i) {
        std::string FileOutput = GetLocation(vFile_Threshold[i], ".bin");
        size_t pos = FileOutput.find("_DAC");
        DAC.push_back(std::atoi(vFile_Threshold[i].substr(pos + 8).c_str()));
        for (int j = 0; j < SFGD_FEBS_NUM; ++j) {
            std::string sCh = "DAC_" + std::to_string(DAC.back()) + "_FEB_" + std::to_string(j);
            hFEBCH[i][j] = new TH1F((sCh).c_str(), sCh.c_str(), 700, 0, 700);
        }
    }

    /// Create root file
    std::string rootFileOutput = GetLocation(stringBuf, ".bin");
    rootFileOutput += "/DAC10b_ASIC" + std::to_string(ASIC_num) +  ".root";
    TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");

    /// Read directory with data and create histograms for each DACs
    std::thread th[vFile_Threshold.size()];
    for (auto i = 0; i < vFile_Threshold.size(); i++) {
        th[i] = std::thread(ReadDirectory, std::ref(wfile),std::ref(hFEBCH), std::ref(DAC), vFile_Threshold[i],i);
    }
    for(int i = 0; i < vFile_Threshold.size(); ++i){
        th[i].join();
    }

    std::cout << "Finished adding elements"<<std::endl;
    for (int j = 0; j < vFile_Threshold.size(); j++) {
        for (int i = 0; i < SFGD_FEBS_NUM; i++) {
            delete hFEBCH[j][i];
        }
    }

    wfile->Close();
    delete wfile;
}




int main(int argc, char **argv) {

    std::string stringBuf;
    MDargumentHandler argh("Example of sfgd threshold study.");
    argh.Init();

    /// Check the user arguments consistancy
    /// All mandatory arguments should be provided and
    /// There should be no extra arguments

    if (argh.ProcessArguments(argc, argv)) {
        argh.Usage();
        return -1;
    }
    /// Treat arguments, obtain values to be used later
    if (argh.GetValue("help")) {
        argh.Usage();
        return 0;
    }

    if (argh.GetMode() == "f") {
        if (argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else if (argh.GetMode() == "d") {
        if (argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else {
        return -1;
    }

    /// Create histograms for each DACs
    std::vector<std::string> vFileNames = argh.GetDataFiles(stringBuf, "","ASIC");

    if(!vFileNames.empty() &&  vFileNames[0].find(".bin") == std::string::npos) {
        std::sort(vFileNames.begin(), vFileNames.end());
        for (int j = 0; j < vFileNames.size(); ++j) {
            unsigned int ASIC_num = (unsigned int)(vFileNames[j].back()) - (unsigned int)48;
            std::cout<<"ASIC " << ASIC_num << std::endl;
            std::vector<std::string> vFile_Threshold = argh.GetDataFiles(vFileNames[j], ".bin");
            if (vFile_Threshold.empty()) {
                std::cerr << "Can not open directory " << vFileNames[j] << std::endl;
                return 1;
            }
            std::sort(vFile_Threshold.begin(), vFile_Threshold.end());
            std::vector<std::string> vFile_Th;
            for(string& str : vFile_Threshold){
                std::string FileOutput = GetLocation(str, ".bin");
                vFile_Th.push_back(str);
            }
            Threshold_St(vFile_Th, stringBuf, ASIC_num);
        }
    }
    else {
        if(stringBuf.back() == '/')
            stringBuf.pop_back();
        unsigned int ASIC_num = (unsigned int)(stringBuf.back()) - (unsigned int)48;
        std::cout<<"ASIC " << ASIC_num << std::endl;
        vFileNames = argh.GetDataFiles(stringBuf, ".bin");
        if (vFileNames.empty()) {
            std::cerr << "Can not open directory " << stringBuf << std::endl;
            return 1;
        }
        std::sort(vFileNames.begin(), vFileNames.end());
        std::vector<std::string> vFile_Threshold;
        for(string& str : vFileNames){
            std::string FileOutput = GetLocation(str, ".bin");
            vFile_Threshold.push_back(str);
        }

        Threshold_St(vFile_Threshold, stringBuf, ASIC_num);
    }

    return 0;
}
