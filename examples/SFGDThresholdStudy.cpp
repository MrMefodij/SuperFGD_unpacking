//
// Created by Maria on 12.07.2023.
//

#include <iostream>
#include "TGraph.h"
#include <TTree.h>
#include <TROOT.h>
#include <TLine.h>
#include "Files_Reader.h"
#include "MDargumentHandler.h"
#include "Calibration.h"
#include "ThresholdStudy.h"
#include "ThresholdXmlOutput.h"

int main(int argc, char **argv) {

    string stringBuf;
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
    vector<string> vFileNames = argh.GetDataFiles(stringBuf, ".bin");
    std::sort(vFileNames.begin(), vFileNames.end());
    if (vFileNames.empty()) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }
    vector<vector<TH1F*>> hFEBCH (vFileNames.size(), vector<TH1F*>(SFGD_SLOT));
    std::vector<unsigned int> DAC;
    for(int i =0; i < vFileNames.size(); i++) {
        std::string FileOutput = GetLocation(vFileNames[i].c_str(), ".bin");
        size_t pos = FileOutput.find("_DAC");
        DAC.push_back(atoi(vFileNames[i].substr(pos + 8).c_str()));
        for (int j = 0; j < SFGD_SLOT; j++) {
            std::string sCh = "DAC_" + std::to_string(DAC.back()) + "_Slot_" + std::to_string(j);
            hFEBCH[i][j] = new TH1F((sCh).c_str(), sCh.c_str(), 701, 0, 700);
        }
    }
    std::map<Threshold, vector<TH1F*>> threshold_data;

    /// Create root file
    std::string rootFileOutput=GetLocation(stringBuf.c_str(), ".bin");
    rootFileOutput+="/DAC10b.root";
    TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);

    /// Read directory with data and create histograms for each DACs
    for(auto i = 0; i < vFileNames.size(); i++) {
        File_Reader file_reader;
        file_reader.ReadFile_for_Threshold(vFileNames[i], hFEBCH[i]);
        auto NFEBs = file_reader.GetFEBNumbers();
        auto feb_channel = file_reader.GetFEBchannel();
        for(auto board_Id : NFEBs) {
            unsigned int slot_Id = board_Id & 0x0f;
            string s = "FEB_" + to_string(board_Id);
            if(wfile->GetDirectory(s.c_str()) == nullptr) {
                wfile->mkdir(s.c_str());
            }
            wfile->cd(s.c_str());
            if (hFEBCH[i][slot_Id]->GetEntries() > 300) {
                hFEBCH[i][slot_Id]->Write((to_string(DAC[i])).c_str());
                threshold_data[{board_Id, feb_channel[board_Id]}].push_back(hFEBCH[i][slot_Id]);
            }
        }
    }

    /// Receive data for xml file
    ThresholdXmlOutput xmlFile;
    for(auto tr : threshold_data) {
        cout << "FEB_"<<tr.first._DAC<<"_Channel_"<<tr.first._ADC<<endl;
        ThresholdStudy threshold;
        threshold.FindThreshold(tr.second, DAC, 25);
        string s = "FEB_" + to_string(tr.first._DAC);
        wfile->cd(s.c_str());
        TGraph *g = threshold.PrintThreshold(3);
        g->SetTitle(("FEB" + to_string(tr.first._DAC) +"_DAC10b_study_ASIC_" + to_string(tr.first._ADC / 32)).c_str());
        g->GetXaxis()->SetTitle("DAC10b");
        g->GetYaxis()->SetTitle("ADC channels");
        g->Draw("AC* same");
        for(auto j : threshold.GetDAC()) {
            TLine *l1 = new TLine(j, 0, j, 500);
            l1->Draw();
        }
        c1->Update();
        g->Write(("FEBs_DAC10b_study_"+ to_string(tr.first._ADC / 32)).c_str());
        ThresholdData tempData{tr.first._ADC / 32, threshold.GetDAC()};
        std::vector<ThresholdData> tempBoard;
        tempBoard.push_back(tempData);
        BoardData<ThresholdData> tempBoardData;
        tempBoardData.AddAsics(tr.first._DAC, tempBoard);
        xmlFile.AddBoard(tempBoardData);
    }
    string sub = stringBuf.substr(stringBuf.find("ASIC",0), stringBuf.size());
    xmlFile.WriteXml((stringBuf.substr(0,stringBuf.size()-5) + "/Threshold_" + sub +".xml").c_str());
    cout <<"XML file: "<< stringBuf.substr(0,stringBuf.size()-5) + "/Threshold_" << sub << ".xml" << endl;


    for(int j = 0; j < vFileNames.size();j++) {
        for (int i = 0; i < SFGD_SLOT; i++) {
            delete hFEBCH[j][i];
        }
    }

    wfile->Close();
    delete wfile;
    delete c1;
    return 0;
}

