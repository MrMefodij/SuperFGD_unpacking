//
// Created by Maria on 05.04.2024 kolupanova@inr.ru
//
#include "MDargumentHandler.h"
#include <TH1F.h>
#include <TF1.h>
#include "SFGD_defines.h"
#include "Hit.h"
#include "Connection_Map.h"
#include <TFile.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TSpectrum.h>
struct LY_peLength{
    double _y{NAN};
    double _LY_PE{NAN};
//    bool operator<(const LY_peLength &other) const {
//        if (_LY == other._LY) {
//            return _y < other._y;
//        }
//        return _LY < other._LY;
//    }
//
//    bool const operator==(const LY_peLength &other) const {
//        return _y == other._y && _LY == other._LY;
//    }
};

int main( int argc, char **argv ) {
    std::string stringBuf;
    // The following shows how to use the MDargumentHandler class
    // to deal with the main arguments
    // Define the arguments
    MDargumentHandler argh("Example of unpacking application.");
    argh.Init();
    if (argh.ProcessArguments(argc, argv)) {
        argh.Usage();
        return -1;
    }
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
    vector<string> vFileNames = argh.GetDataFiles(stringBuf, ".root", "StraightTracksTree");
    std::sort(vFileNames.begin(), vFileNames.end());

    if (vFileNames.empty()) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    size_t pos = stringBuf.find("StraightTracksTree");
    std::string rootFileOutput;
    if(pos != string::npos)
        rootFileOutput = stringBuf.substr(0, pos);
    else{
        rootFileOutput = stringBuf;
    }
    rootFileOutput += "LY_LengthVertical.root";
    std::cout << rootFileOutput << std::endl;
    cout << rootFileOutput << endl;
    std::string mapFile = "../connection_map/SFG_Geometry_Map_v18.csv";
    if(getenv("UNPACKING_ROOT")!= nullptr){
        mapFile = (string)getenv("UNPACKING_ROOT") + "/connection_map/SFG_Geometry_Map_v18.csv";
    }

    Connection_Map connectionMap(mapFile);
    try {
        connectionMap.Init();
    } catch (const exception& e) {
        std::cerr << "Unable to open file " << mapFile << std::endl;
        exit(1);
    }

    TH1F* LY_Length[Y_SIZE];
    for(auto i = 0; i < Y_SIZE; ++i){
        LY_Length[i]  = new TH1F(("LY_Length_"+ std::to_string(i)).c_str(), "",100, 0, 200);
    }
    for (auto file = 0; file < vFileNames.size(); ++file) {
        std::string filename = vFileNames.at(file);
        TFile *fileInput = new TFile((filename).c_str());
        if (!fileInput->IsOpen()) {
            cerr << "Can not open file " << filename << endl;
            return 1;
        }
        TTree *eventTree = fileInput->Get<TTree>("EventsHits");
        std::vector<Hit> *eventsHits(nullptr);
        double x_angle{0.0};
        double y_angle{0.0};
        try {
            if (eventTree->GetBranch("Events"))
                eventTree->SetBranchAddress("Events", &eventsHits);
            if(eventTree->GetBranch("x_angle") )
                eventTree->SetBranchAddress("x_angle", &x_angle);
            if(eventTree->GetBranch("y_angle") )
                eventTree->SetBranchAddress("y_angle", &y_angle);

        } catch (...) {
            std::cerr << "Error in file " << vFileNames[file] << std::endl;
        }
        for (unsigned int j = 0; j < eventTree->GetEntries(); ++j) {
            try {
                eventTree->GetEntry(j);
            } catch (...) {
                std::cerr << "Error in file " << file << " entry " << j << std::endl;
            }
            std::vector<LY_peLength> LY_Y(Z_SIZE);
            std::vector<double> LY_X(Z_SIZE, 0.0);
            for (auto i = 0; i < eventsHits->size(); ++i) {
                if (!std::isnan(eventsHits->at(i)._x) && !std::isnan(eventsHits->at(i)._z) &&
                    !std::isnan(eventsHits->at(i)._LY_PE))
                    LY_X[eventsHits->at(i)._z] = std::max(LY_X[eventsHits->at(i)._z], eventsHits->at(i)._LY_PE * std::cos(std::atan(x_angle)));
                if (!std::isnan(eventsHits->at(i)._z) && !std::isnan(eventsHits->at(i)._y) &&
                    !std::isnan(eventsHits->at(i)._LY_PE)) {
                    LY_peLength ly_y = {eventsHits->at(i)._y, eventsHits->at(i)._LY_PE * std::cos(std::atan(y_angle))};
                    LY_Y[eventsHits->at(i)._z] = (ly_y._LY_PE > LY_Y[eventsHits->at(i)._z]._LY_PE) ||
                                                  std::isnan(LY_Y[eventsHits->at(i)._z]._LY_PE)
                                                 ? ly_y : LY_Y[eventsHits->at(i)._z];
                }
            }
            for (auto i = 0; i < Z_SIZE; ++i) {
                if (LY_X[i] != 0 && !std::isnan(LY_Y[i]._y) && !std::isnan(LY_Y[i]._LY_PE)) {
//                    if ((Y_SIZE  - (unsigned int) LY_Y[i]._y == 13) ||
//                        (Y_SIZE  - (unsigned int) LY_Y[i]._y == 19) ||
//                        (Y_SIZE  - (unsigned int) LY_Y[i]._y == 20) ||
//                        (Y_SIZE  - (unsigned int) LY_Y[i]._y >= 22)) {
//                        LY_Length[Y_SIZE - 1 - (unsigned int) LY_Y[i]._y]->Fill(
//                                LY_X[i] - 4.0);
//                    } else {
                        LY_Length[Y_SIZE - 1 - (unsigned int) LY_Y[i]._y]->Fill(
                                LY_X[i]);
//                    }
                }
            }
        }
    }
    TFile* wfile = new TFile(rootFileOutput.c_str(), "recreate");
    wfile->cd();
    auto sumLY_avr_err = new TGraphErrors();
    sumLY_avr_err->SetTitle("AttenuationLength_Y");
    TDirectory *dir = wfile->mkdir("Historgrams");
    dir->cd();
    for(auto y = 0; y < Y_SIZE; ++y){
        TSpectrum* _s = new TSpectrum;
        int nfound = _s->Search(LY_Length[y], 4, "", 0.05);
        double *xpeaks = _s->GetPositionX();
        if(nfound > 0) {
            TF1 *fit = new TF1("fit", "gaus", xpeaks[0] - 10, xpeaks[0] + 10);
            LY_Length[y]->Fit("fit", "MERQ", "");
            double mean_LY = fit->GetParameter(1);
            double mean_LY_err = fit->GetParError(1);
            sumLY_avr_err->AddPoint(y +1, mean_LY);
            sumLY_avr_err->SetPointError(y, 0, mean_LY_err);
        }
        LY_Length[y]->GetXaxis()->SetTitle("Observed Light [p.e.]");
        LY_Length[y]->GetYaxis()->SetTitle("N");
        LY_Length[y]->Write();
        delete LY_Length[y];
        delete _s;
    }


    TF1 *fit_func = new TF1("fit_func", "((([0]*exp(-1.0*x/[2])+[1]*exp(-1.0*x/[3])) + [4]*([0]*exp(-1.0*(390.0-x)/[2]) + [1]*exp(-1.0*(390.0-x)/[3]))))",1,Y_SIZE-1 );
//    fit_func->SetLineColor(1);
    fit_func->SetLineWidth(3);

//    fit_func->SetParameter(0, 11.0); // L_s
    fit_func->SetParLimits(0,5,20);
    fit_func->SetParName(0, "L_{s}");

//    fit_func->SetParameter(1, 37.0); // L_l
    fit_func->SetParLimits(1,12,75);
    fit_func->SetParName(1, "L_{l}");

//    fit_func->SetParameter(2, 12.0); // A_s
    fit_func->SetParLimits(2,5,60);
    fit_func->SetParName(2, "A_{s}");

//    fit_func->SetParameter(3, 400.0); // A_l
    fit_func->SetParLimits(3,250,450);
    fit_func->SetParName(3, "A_{l}");

//    fit_func->SetParameter(4, 0.25); // R
    fit_func->SetParLimits(4,0.10,0.25);
    fit_func->SetParName(4, "R");
    sumLY_avr_err->Fit("fit_func","MERQ+", "");
    sumLY_avr_err->GetXaxis()->SetLimits(1. , Y_SIZE+1 );
    sumLY_avr_err->GetYaxis()->SetTitle("Observed Light [p.e.]");
    sumLY_avr_err->GetXaxis()->SetTitle("Length [cm]");

    wfile->cd();
    sumLY_avr_err->Write("AttenuationLength_Y");
    delete sumLY_avr_err;
    wfile->Close();
    delete wfile;
    return 0;
}