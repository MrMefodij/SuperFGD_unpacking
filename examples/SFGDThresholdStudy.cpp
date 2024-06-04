//
// Created by Maria on 16.04.2024.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <TFile.h>
#include <TLine.h>
#include "ThresholdStudy.h"
#include "ThresholdXmlOutput.h"
#include "MDargumentHandler.h"
#include "SFGD_defines.h"

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

    std::vector<std::string> vFileNames = argh.GetDataFiles(stringBuf, ".root", "_ASIC");
    if ( vFileNames.empty()) {
        std::cerr << "Can not open directory " << stringBuf << std::endl;
        return 1;
    }
    std::sort(vFileNames.begin(), vFileNames.end(), [](const auto &itL, const auto &itR) { return itL > itR; });
    /// Create histograms for each DACs
    size_t pos = stringBuf.find("ASIC");
    std::string rootFileOutput;
    if (pos != std::string::npos)
        rootFileOutput = stringBuf.substr(0, pos) + "_";
    else {
        rootFileOutput = stringBuf;
    }
    rootFileOutput += "DAC10b_Calibration.root";
    TFile wfile(rootFileOutput.c_str(), "recreate");
    for (int j = 0; j < vFileNames.size(); ++j) {
        size_t pos = vFileNames[j].find("ASIC");
        if(pos != std::string::npos) {
            std::string ASIC_num{vFileNames[j].substr(pos + 4, 1)};
            TDirectory *ASIC = wfile.mkdir(("ASIC " + ASIC_num).c_str());
            TFile *fileInput = new TFile(vFileNames[j].c_str());
            auto dirList = fileInput->GetListOfKeys();
            ASIC->cd();
            ThresholdStudy th;
            ThresholdXmlOutput xmlFile;
//            std::map<Threshold, vector<TH1F *>> threshold_data;
            for (const TObject *dir: *dirList) {
                std::string str{dir->GetName()};
                pos = str.find("FEB_");
                if (pos != std::string::npos) {
                    unsigned int boardId = std::stoi(str.substr(pos + 4));
                    TDirectory *FEB = ASIC->mkdir(dir->GetName());
                    auto dirName = fileInput->GetDirectory(dir->GetName());
                    auto objectList = dirName->GetListOfKeys();
                    ThresholdStudy threshold;
                    std::map<unsigned int, TH1F*> DAC10b_hFEBH;
//                    std::vector<unsigned int> DAC10bValues;
//                    vector<TH1F *> hFEBCH_Values;
                    for (const TObject *obj: *objectList) {
                        unsigned int DAC10b = std::stoi(obj->GetName());
                        TH1F *hFEBCH;
                        std::string path(dir->GetName());
                        path += "/";
                        path += obj->GetName();
                        fileInput->GetObject(path.c_str(), hFEBCH);
                        if (hFEBCH->GetEntries() > 10) {
//                            DAC10bValues.push_back(DAC10b);
//                            hFEBCH_Values.push_back(hFEBCH);
                            DAC10b_hFEBH[DAC10b] = hFEBCH;
//                            auto feb_channel = file_reader.GetFEBchannel(); // feb_channel[boardId]
//                            unsigned int feb_channel = 8;
//                            if (threshold_data.find({boardId, feb_channel}) == threshold_data.end())
//                                threshold_data[{boardId, feb_channel}] = {hFEBCH};
//                            else {
//                                threshold_data[{boardId, feb_channel}].push_back(hFEBCH);
//                            }
                        }
//                FEB->cd();
//                hFEB->Write();
//                hFEB->Delete();
                    }
                    threshold.FindThreshold(DAC10b_hFEBH, 8);
                    FEB->cd();
                    TGraph *g = threshold.PrintThreshold(2);
                    g->SetTitle(("FEB" + to_string(boardId) + "_DAC10b_study_ASIC_" + ASIC_num).c_str());
                    g->GetXaxis()->SetTitle("DAC10b");
                    g->GetYaxis()->SetTitle("ADC channels");
//                    g->Draw("AC* same");
                    g->Write();
                    FEB->Close();
                    ThresholdData tempData{(unsigned int)std::stoi(ASIC_num), threshold.GetDAC()};
                    std::vector<ThresholdData> tempBoard;
                    tempBoard.push_back(tempData);
                    BoardData<ThresholdData> tempBoardData;
                    tempBoardData.AddAsics(boardId, tempBoard);
                    xmlFile.AddBoard(tempBoardData);
                }
            }
            ASIC->Close();
            xmlFile.WriteXml((stringBuf + "ASIC_"+ ASIC_num +".xml").c_str());
            std::cout << "XML file: " << (stringBuf + "ASIC_"+ ASIC_num +".xml") << std::endl;
        }
    }
    wfile.Close();
    wfile.Delete();
    return 0;
}
