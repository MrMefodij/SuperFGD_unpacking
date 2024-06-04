//
// Created by amefodev on 16.09.2023.
//

#include <string>
#include <iostream>
#include <TFile.h>
#include <TH2.h>

#include "MDargumentHandler.h"
#include "ToaEventDummy.h"
#include "Connection_Map.h"
#include "SFGD_defines.h"
#include "EventDisplay.h"
#include <time.h>
#include <TProfile.h>
#include <TF1.h>
#include "Files_Reader.h"
#include <TROOT.h>
double fit(double *x, double *par) {
    return par[0] + par[1]*x[0] + par[2]*x[0]*x[0] +par[3]*x[0]*x[0]*x[0] +par[4]*x[0]*x[0]*x[0]*x[0];
}

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
    vector<string> vDirs = argh.GetDataFiles(stringBuf, "", "");
    vector<string> vFileNames;
    for (auto vDir: vDirs) {
        vector<string> vFileNamesInFolder = argh.GetDataFiles(vDir, ".root", "_plots");
        vFileNames.insert(vFileNames.end(), vFileNamesInFolder.begin(), vFileNamesInFolder.end());
    }
//    vector<string> vFileNames = argh.GetDataFiles(stringBuf, ".root", "_plots");
    if (vFileNames.empty()) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    std::string mapFile = "../connection_map/SFG_Geometry_Map_v18.csv";
    if (getenv("UNPACKING_ROOT") != nullptr) {
        mapFile = (string) getenv("UNPACKING_ROOT") + "/connection_map/SFG_Geometry_Map_v18.csv";
    }
    Connection_Map connectionMap(mapFile);
    try {
        connectionMap.Init();
    } catch (const exception &e) {
        std::cerr << "Unable to open file " << mapFile << endl;
        std::exit(1);
    }
    TH2F HGvsLG("HGvsLG", "HGvsLG", 4000, 0, 4000, 4000, 0, 4000);
    clock_t tStart = clock();
    std::vector<ToaEventDummy> *FEBs[SFGD_FEBS_NUM];
    std::fill(FEBs, FEBs + SFGD_FEBS_NUM, nullptr);
    map<unsigned int, set<unsigned int>> available_Febs;
    TH2F *LG_vs_HG[SFGD_FEBS_NUM * SFGD_FEB_NCHANNELS];
    TH2F *HG_vs_ToT[SFGD_FEBS_NUM * SFGD_FEB_NCHANNELS];
    TH2F *LG_vs_ToT[SFGD_FEBS_NUM * SFGD_FEB_NCHANNELS];

    for (auto ih = 0; ih < SFGD_FEBS_NUM; ih++) {
        for (auto ch = 0; ch < SFGD_FEB_NCHANNELS; ch++) {

            std::string s = "Slot " + to_string(ih) + " channel " + to_string(ch);

            HG_vs_ToT[ih * SFGD_FEB_NCHANNELS + ch] = new TH2F((s + " HG_vs_ToT").c_str(),
                                                               (s + " HG_vs_ToT").c_str(), 50, 0, 50,
                                                               400, 0, 4095);
            LG_vs_ToT[ih * SFGD_FEB_NCHANNELS + ch] = new TH2F((s + " LG_vs_ToT").c_str(),
                                                               (s + " LG_vs_ToT").c_str(), 50, 0, 50,
                                                               400, 0, 4095);
            LG_vs_HG[ih * SFGD_FEB_NCHANNELS + ch] = new TH2F((s + " LG_vs_HG").c_str(), (s + " LG_vs_HG").c_str(), 512,
                                                              0, 4095, 512, 0, 4095);

        }
    }
//    set<unsigned int> FEBs_in_Crate;
    std::sort(vFileNames.begin(), vFileNames.end());
    for (unsigned int file = 0; file < vFileNames.size(); ++file) {
        DataFile dataFile;
        dataFile.fileName_ = vFileNames.at(file);
        dataFile.FileInput_ = new TFile((vFileNames.at(file)).c_str());
        if (!dataFile.FileInput_->IsOpen()) {
            std::cerr << "Can not open file " << vFileNames.at(file) << endl;
            return 1;
        }
        vector<unsigned int> available_Febs_in_Crate;
        dataFile.AllEvents_ = (TTree *) dataFile.FileInput_->Get("AllEvents");
        ostringstream sFEBnum;
        string sFEB;
        for (unsigned int ih = 0; ih < SFGD_FEBS_NUM; ++ih) {
            sFEBnum.str("");
            sFEBnum << ih;
            sFEB = "FEB_" + sFEBnum.str();
            if (dataFile.AllEvents_->GetBranch((sFEB).c_str())) {
                dataFile.AllEvents_->SetBranchAddress((sFEB).c_str(), &FEBs[ih]);
                available_Febs_in_Crate.push_back(ih);
                available_Febs[(ih >> 4)].insert(ih);
            }
        }
        if (!available_Febs_in_Crate.empty()) {
            unsigned int nEntries = dataFile.AllEvents_->GetEntries();
            dataFile.availableCrates_ = (CrateGates{(available_Febs_in_Crate.back() >> 4),
                                                    available_Febs_in_Crate,
                                                    nEntries});
            std::cout << dataFile.fileName_ << " entries " << nEntries << std::endl;
//            FEBs_in_Crate = available_Febs[Crate_Num];
//        std::cout<<"Crate "<< Crate_Num <<"  " << dataFile.fileName_ << std::endl;
            auto evNums = dataFile.availableCrates_.nEntries_;
            for (auto evNum = 0; evNum < evNums; evNum++) {
                try {
                    dataFile.AllEvents_->GetEntry(evNum);
                } catch (...) {
                    cerr << "Error in file " << dataFile.fileName_ << " entry " << evNum << endl;
                }
                bool gateIsOk = true;
                for (auto boardId: available_Febs_in_Crate) {
                    if (gateIsOk /*&&std::find(file.availableCrates_.availableFebs_.begin(), file.availableCrates_.availableFebs_.end(), boardId) != file.availableCrates_.availableFebs_.end()*/) {
                        try {
                            auto hits = FEBs[boardId]->at(0).GetHits();
                            if (!hits.empty()) {
                                for (const auto &hit: hits) {
                                    auto ch256 = hit.GetChannelNumber();
                                    auto HG_value = hit.GetHighGainADC();
                                    auto LG_value = hit.GetLowGainADC();
                                    auto ToT_value = hit.GetTimeOverThreshold();
                                    if (HG_value > 1 && LG_value > 1 && ToT_value > 1) {
                                        LG_vs_HG[boardId * SFGD_FEB_NCHANNELS + ch256]->Fill(HG_value, LG_value, 1);
                                        HG_vs_ToT[boardId * SFGD_FEB_NCHANNELS + ch256]->Fill(ToT_value, HG_value, 1);
                                        LG_vs_ToT[boardId * SFGD_FEB_NCHANNELS + ch256]->Fill(ToT_value, LG_value, 1);
                                    }
                                }
                            }
                        }
                        catch (...) {
                            gateIsOk = false;
                            // std::cout << "Error on OCB event Number: \t" << evNum + 1 << "\t FEB#: " << boardId << endl;
                            break;
                        }
                    }
                }
            }
        }
    }

//    for(auto crate_num : available_Febs) {

    for (unsigned int Crate_Num = 0; Crate_Num < SFGD_CRATES_NUM; Crate_Num++){

    std::string rootFileOutput = {vFileNames[0], 0, vFileNames[0].rfind("_plots.root")};
    rootFileOutput += "_HG_LG_all_files_crate_" + to_string(Crate_Num);
    std::cout << rootFileOutput << endl;
    TFile *wfile = new TFile((rootFileOutput + ".root").c_str(), "recreate");

    wfile->cd();
    TF1 *f = new TF1("f", fit, 12, 30, 5);
    TF1 *f1 = new TF1("Linear fit", "[0]+x*[1]");
    TProfile *prof;
    std::cout << "Writing histograms...\n";
    std::ofstream fout((rootFileOutput + ".txt").c_str());
    TDirectory *Cratedir = wfile->mkdir(("Crate " + to_string(Crate_Num)).c_str());
    for (auto ih: available_Febs[Crate_Num]) {
        TDirectory *FEBdir = Cratedir->mkdir(("FEB " + to_string(ih)).c_str());
        FEBdir->cd();
        for (auto ch = 0; ch < SFGD_FEB_NCHANNELS; ch++) {
            std::string s = "FEB " + to_string(ih) + " channel " + to_string(ch);
            prof = HG_vs_ToT[ih * SFGD_FEB_NCHANNELS + ch]->ProfileX();
            prof->GetYaxis()->SetTitle("HG [ADC]");
            prof->GetXaxis()->SetTitle("Time over Threshold [1.25 ns]");
//            prof->Fit(f,"MERQ","",25,45);
            prof->Write();
            HG_vs_ToT[ih * SFGD_FEB_NCHANNELS + ch]->Write();
            prof = LG_vs_ToT[ih * SFGD_FEB_NCHANNELS + ch]->ProfileX();
            prof->GetYaxis()->SetTitle("LG [ADC]");
            prof->GetXaxis()->SetTitle("Time over Threshold [1.25 ns]");
//            prof->Fit(f,"MERQ","",25,45);
            prof->Write();
            LG_vs_ToT[ih * SFGD_FEB_NCHANNELS + ch]->Write();
            prof = LG_vs_HG[ih * SFGD_FEB_NCHANNELS + ch]->ProfileX();
            prof->Fit(f1, "MERQ", "", 400, 2000);
            prof->GetYaxis()->SetTitle("LG [ADC]");
            prof->GetXaxis()->SetTitle("HG [ADC]");
            prof->Write();
            double par[2];
            double par_err[2];
            f1->GetParameters(&par[0]);
            fout << ih << " " << ch << " " << par[1] << " " << f1->GetParError(1) << std::endl;
            LG_vs_HG[ih * SFGD_FEB_NCHANNELS + ch]->Write();
            delete HG_vs_ToT[ih * SFGD_FEB_NCHANNELS + ch];
            delete LG_vs_ToT[ih * SFGD_FEB_NCHANNELS + ch];
            delete LG_vs_HG[ih * SFGD_FEB_NCHANNELS + ch];
        }
    }
//    }
//    std::cout <<"Deleting histograms..."<<std::endl;
    delete prof;
    delete f;
    delete f1;
    std::cout << "Closing root file..." << std::endl;;
    wfile->Close();
}
//    gROOT->GetListOfFiles()->Remove(wfile);
    printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
    return 0;
}