//
// Created by Maria on 25.09.2023.
//
#include <iostream>
//#include <dirent.h>
#include "MDargumentHandler.h"
//#include "Gain_Reader.h"
#include <TFile.h>
#include "Reader.h"
#include <TLine.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH2F.h>
#include "Connection_Map.h"
int main(int argc, char **argv){
//    vector<std::string> vFileNames;
//    vFileNames.clear();
//    DIR *dir;
//    struct dirent *ent;
//    if ((dir = opendir ("../calibration_results/")) != NULL) {
//        while ((ent = readdir (dir)) != NULL) {
//            vFileNames.push_back(ent->d_name);
//        }
//        closedir (dir);
//    } else {
//        perror ("");
//        return EXIT_FAILURE;
//    }
    string stringBuf;
    MDargumentHandler argh("Example of sfgd gain reader study.");
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

    vector<string> vFileNames = argh.GetDataFiles(stringBuf,  ".txt");
    std::sort(vFileNames.begin(), vFileNames.end());
    if (vFileNames.empty()) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    /// Connection map (read csv)
    string mapFile = "../connection_map/SFG_Geometry_Map_v18.csv";
    if(getenv("UNPACKING_ROOT")!=NULL){
        mapFile = (string)getenv("UNPACKING_ROOT") + "/connection_map/SFG_Geometry_Map_v18.csv";
    }
    Connection_Map connectionMap(mapFile);
    try {
        connectionMap.Init();
    } catch (const exception& e) {
        cerr << "Unable to open file " << mapFile << endl;
        exit(1);
    }

    /// Example
    GainReader calib_res(vFileNames);
    calib_res.Init();
    auto global_channels = calib_res.GetAllChannels();
//    PedMapReader pedmap_res(vFileNames);
//    pedmap_res.Init();
//    auto global_channels = pedmap_res.GetAllChannels();
//    auto pedmap = pedmap_res.GetPedestal2d(*global_channels.begin());

    std::string rootFileOutput = stringBuf + "/gain_res.root";
    /// Canvas
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);
    TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");
    TH1F* hGain = new TH1F("Gain_distribution", "Gain_distribution",  400, 0, 100);
    TH2F *GainMap_XZ = new TH2F("Gain_results_XZ","Gain results_XZ",  192,(int)0,(int)192, 182,0,182);
    TH2F *GainMap_XY = new TH2F("Gain_results_XY","Gain results_XY",  192,(int)0,(int)192, 56,0,56);
    TH2F *GainMap_ZY = new TH2F("Gain_results_ZY","Gain results_ZY",   182,0,182,56,0,56);
    TH2F *LED_ZY = new TH2F("LED_ZY","LED_ZY",   182,0,182,56,0,56);
    TH2F *LED_XZ = new TH2F("LED_XZ","LED_XZ",  192,(int)0,(int)192, 182,0,182);
    TH2F *LED_XY = new TH2F("LED_XY","LED_XY",  192,(int)0,(int)192, 56,0,56);
    TH2F *GainMap = new TH2F("Gain_results_for_crate","Gain results for crate",  256,(int)0,(int)256, 252,0,252);
    for(auto global_channel : global_channels){
        if(calib_res.GetValueAndError(global_channel).first!=0)
            hGain->Fill(calib_res.GetValueAndError(global_channel).first);
        auto geomPosition = connectionMap.GetGlobalGeomPositionPtr(global_channel);
        GainMap_XZ->Fill(geomPosition->x_,geomPosition->z_,calib_res.GetValue(global_channel));
        GainMap_XY->Fill(geomPosition->x_,geomPosition->y_,calib_res.GetValue(global_channel));
        GainMap_ZY->Fill(geomPosition->z_,geomPosition->y_,calib_res.GetValue(global_channel));
//        LED_XZ->Fill(geomPosition->x_,geomPosition->z_,calib_res.GetDAC(global_channel));
//        LED_XY->Fill(geomPosition->x_,geomPosition->y_,calib_res.GetDAC(global_channel));
//        LED_ZY->Fill(geomPosition->z_,geomPosition->y_,calib_res.GetDAC(global_channel));
//        GainMap->Fill(feb,ch,calib_res.GetDAC(global_channel));
    }
    TF1 * fit = new TF1("fit","gaus");
    hGain->Fit("fit","","");
    double mean_gain = fit->GetParameter(1);
    double std_gain = fit->GetParameter(2);
    std::cout <<"Mean = "<<mean_gain<<", std = "<<std_gain<<std::endl;
    wfile->cd();
    auto* l_1 = new TLine(hGain->GetMean() - 3*fit->GetParameter(2),0,hGain->GetMean() - 3*fit->GetParameter(2) ,10000);
    auto* l_2 = new TLine(hGain->GetMean() + 3*fit->GetParameter(2),0,hGain->GetMean() + 3*fit->GetParameter(2) ,10000);
    l_1->SetLineStyle(2);
    l_2->SetLineStyle(2);
    c1->cd();
    hGain->Draw();
    l_1->Draw();
    l_2->Draw();
    c1->Write();
    GainMap_XZ->GetYaxis()->SetTitle("Z");
    GainMap_XZ->GetXaxis()->SetTitle("X");
    GainMap_XZ->SetStats(0);
    GainMap_XZ->Write();

    GainMap_ZY->GetYaxis()->SetTitle("Y");
    GainMap_ZY->GetXaxis()->SetTitle("Z");
    GainMap_ZY->SetStats(0);
    GainMap_ZY->Write();

    GainMap_XY->GetYaxis()->SetTitle("Y");
    GainMap_XY->GetXaxis()->SetTitle("X");
    GainMap_XY->SetStats(0);
    GainMap_XY->Write();

    LED_XZ->GetYaxis()->SetTitle("Z");
    LED_XZ->GetXaxis()->SetTitle("X");
    LED_XZ->SetStats(0);
    LED_XZ->Write();

    LED_ZY->GetYaxis()->SetTitle("Y");
    LED_ZY->GetXaxis()->SetTitle("Z");
    LED_ZY->SetStats(0);
    LED_ZY->Write();

    LED_XY->GetYaxis()->SetTitle("Y");
    LED_XY->GetXaxis()->SetTitle("X");
    LED_XY->SetStats(0);
    LED_XY->Write();
    cout <<"Calibrated "<<hGain->GetEntries()<<" channels from "<<global_channels.size()<<"\n";
    delete l_1;
    delete l_2;
    delete hGain;
    delete c1;
    delete GainMap_XZ;
    delete GainMap_XY;
    delete GainMap_ZY;
    delete LED_ZY;
    delete LED_XZ;
    delete LED_XY;
    delete fit;
    wfile->Close();
    return 0;
}