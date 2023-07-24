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
    vector<TDirectory*> FEBdir (SFGD_SLOT);
    unsigned int channel_Id;

    /// Read directory with data and create histograms for each DACs
    for(auto i = 0; i < vFileNames.size(); i++) {
        File_Reader file_reader;
        file_reader.ReadFile(vFileNames[i], hFEBCH[i]);
        auto NFEBs = file_reader.GetFEBNumbers();
        auto feb_channel = file_reader.GetFEBchannel();
        for(auto board_Id : NFEBs) {
            unsigned int slot_Id = board_Id & 0x0f;
            if(i==0)
                FEBdir[slot_Id] = wfile->mkdir(("FEB_" + to_string(board_Id )).c_str());

            if (hFEBCH[i][slot_Id]->GetEntries() > 1) {
                FEBdir[slot_Id]->cd();
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
        threshold.FindThreshold(tr.second, DAC, 8
        );
        FEBdir[tr.first._DAC & 0x0f]->cd();
        TGraph *g = threshold.PrintThreshold(2);
        g->SetTitle(("FEBs_DAC10b_study_ASIC_" + to_string(tr.first._ADC / 32)).c_str());
        g->GetXaxis()->SetTitle("DAC10b");
        g->GetYaxis()->SetTitle("ADC channels");
        g->Draw("AC* same");
        for(auto j : threshold.GetDAC()) {
            TLine *l1 = new TLine(j, 0, j, 500);
            l1->Draw();
        }
        c1->Update();
        c1->Write(("FEBs_DAC10b_study_"+ to_string(tr.first._ADC / 32)).c_str());
        ThresholdData tempData{tr.first._ADC / 32, threshold.GetDAC()};
        std::vector<ThresholdData> tempBoard;
        tempBoard.push_back(tempData);
        BoardData<ThresholdData> tempBoardData;
        tempBoardData.AddAsics(tr.first._DAC, tempBoard);
        xmlFile.AddBoard(tempBoardData);
    }
    xmlFile.WriteXml((stringBuf + "/Threshold.xml").c_str());
    cout <<"XML file: "<<stringBuf + "/Threshold.xml"<<endl;


    for(int j = 0; j < vFileNames.size();j++) {
        for (int i = 0; i < SFGD_SLOT; i++) {
            delete hFEBCH[j][i];
        }
    }

    wfile->Close();
    delete c1;
    return 0;
}

