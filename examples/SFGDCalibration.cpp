//
//  Created by Maria on 27.09.2022 kolupanova@inr.ru
//

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <TCanvas.h>
#include <TTree.h>
#include <TF1.h>
#include <TLine.h>
#include <TROOT.h>
#include "Calibration.h"
#include "Files_Reader.h"
#include "SFGD_defines.h"
#include "MDargumentHandler.h"
#include "Connection_Map.h"
#include <numeric>
#include <map>
#include <TH2F.h>

#define MEAN_VALUE 350
#define ENTRIES_NUM 60

int main(int argc, char **argv){

    string stringBuf;
    MDargumentHandler argh("Example of sfgd calibration.");
    argh.Init();

    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}

    // Treat arguments, obtain values to be used later
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}

    if ( argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
    string filename = stringBuf;

    std::ifstream ifs((filename).c_str());
    if ( ifs.fail() ) {
        cerr << "Can not open file " << filename << endl;
        return 1;
    }

    /// Create root file
    string rootFileOutput=GetLocation(filename.c_str(), ".bin");
    std::ofstream fout((rootFileOutput+".txt").c_str());
    rootFileOutput+="_channels_signal.root";
    cout << rootFileOutput<<endl;

    TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");
    /// Prepare TTree


    /// Canvas
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);

    /// Create TH1F for each SFGD_FEB_NCHANNELS in SFGD_SLOT
    vector<vector<TH1F*>> hFEBCH (SFGD_SLOT, vector<TH1F*>(SFGD_FEB_NCHANNELS));
    for(int i = 0; i < SFGD_SLOT; i++){
        for(int j = 0; j < SFGD_FEB_NCHANNELS; j++){
            std::string sCh = "Slot_"+std::to_string(i)+"_Channel_"+std::to_string(j);
            hFEBCH[i][j] = new TH1F(sCh.c_str(),sCh.c_str(),  700, 0, 700);
        }
    }

    /// Connection map (read csv)
    string mapFile = "../connection_map/SFG_Geometry_Map_v13.csv";
    if(getenv("UNPACKING_ROOT")!=NULL){
        mapFile = (string)getenv("UNPACKING_ROOT") + "/connection_map/SFG_Geometry_Map_v13.csv";
    }
    Connection_Map connectionMap(mapFile);
    try {
        connectionMap.Init();
    } catch (const exception& e) {
        cerr << "Unable to open file " << mapFile << endl;
        exit(1);
    }

    /// Going through data file
    Calibration cl;
    File_Reader file_reader;

    /// parameter HG_LG: for HG - 2, for LG - 3, default - 2
    file_reader.ReadFile_for_Calibration(filename, hFEBCH,2);

    /// Find numbers of measured FEB
    set<unsigned int> NFEB = file_reader.GetFEBNumbers();

    TH1F* h_mean = new TH1F("Mean distribution","Mean distribution",  700, 0, 700);
    TH1F* h_max = new TH1F("Max distribution","Max distribution",  700, 0, 700);
    for(unsigned int ih : NFEB){
        unsigned int slot_id = ih & 0x0f;
        for (unsigned int iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            h_mean -> Fill(hFEBCH[slot_id][iCh]->GetMean());
            h_max -> Fill(hFEBCH[slot_id][iCh]->GetMaximum());
        }
    }
    TF1 * fit_mean = new TF1("fit_mean","gaus");
    TF1 * fit_max = new TF1("fit_max","gaus");
    h_mean->Fit("fit_mean","","");
    h_max->Fit("fit_max","","");
    h_max->Write();
    h_mean->Write();
    double mean_value = fit_mean->GetParameter(1);
    double sigma_mean_value = fit_mean->GetParameter(2);
    double max_value = fit_max->GetParameter(1);
    double sigma_max_value = fit_max->GetParameter(2);
    Tree tree;

    TH2F *GainMap = new TH2F("Gain results for crate","Gain results for crate",  256,(int)0,(int)256, 14,(int)*NFEB.begin(),(int)*NFEB.begin()+14);
    /// Get histograms with peaks
    unsigned int crate_number;
    for(unsigned int ih : NFEB){
        TDirectory *FEBdir = wfile->mkdir(("FEB_"+to_string(ih)).c_str());
        FEBdir->cd();
        map<unsigned int, vector<double>> gain_values;
        for (unsigned int iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            unsigned int slot_id = ih & 0x0f;
            if(hFEBCH[slot_id][iCh]->GetEntries()>0) {
                string feb_channel = "FEB_" + to_string(ih) + "_Channel_" + to_string(iCh);
//            hFEBCH[slot_id][iCh]->Write(feb_channel.c_str());
                cl.SFGD_Calibration(hFEBCH[slot_id][iCh], feb_channel, mean_value, sigma_mean_value, max_value,
                                    sigma_max_value);
                TGraphErrors *gr = new TGraphErrors;
                cl.Gain_Calculation(gr, feb_channel);

                /// for tree
                GlGeomPosition glgeom = connectionMap.GetGlobalGeomPosition(ih, iCh);
                GlChannelPosition glch = connectionMap.GetGlobalChannelPosition(ih, iCh);
                tree._max_histogram = hFEBCH[slot_id][iCh]->GetMaximum();
                tree._num_histogram = hFEBCH[slot_id][iCh]->GetEntries();
                tree._crate = ih >> 4;
                crate_number = ih >> 4;
                tree._feb = ih;
                tree._slot = slot_id;
                tree._channel = iCh;
                tree._mean = cl.GetMean();
                tree._median = cl.GetMedian();
                tree._gain = cl.GetGain();
                tree._x = glgeom.x_;
                tree._y = glgeom.y_;
                tree._z = glgeom.z_;
                tree._pcbPosition = glgeom.pcbPosition_;
                tree._position = glch.position_;
                tree.Add_Element();
                if (cl.GetGain() < 40) {
                    GainMap->Fill(iCh, ih, cl.GetGain());
                }
                auto *l_median = cl.Calibration_Line_Median();
                auto *l_mean = cl.Calibration_Line_Mean();
                l_median->Draw();
                l_mean->Draw();
                auto *legend = cl.Calibration_Legend();
                legend->Draw();
                c1->Update();
                c1->Write(feb_channel.c_str());
                gr->Draw("APE");
//                  auto legend = cl.Get_Legend_for_TGraphErrors();
//                  legend->Draw("LSame");
                c1->Update();
                c1->Write((feb_channel + "_graph").c_str());
                delete gr;
                delete hFEBCH[slot_id][iCh];
            }
        }
    }

    /// Find channels with std more than 3 sigma
    map<string,pair<double,double>> gain_values = cl.GetGains();
    string connection;
    TH1F* hGain = new TH1F("Gain_distribution", "Gain_distribution",  400, 0, 100);
    TH1F* hGain_dist = new TH1F("Gain_deviation", "Gain_deviation",  1000, -10, 10);
    TH1F* hGain_relative_error = new TH1F("Gain_relative_error_distribution", "Gain_relative_error_distribution",  1000, 0, 1);
    for (auto g : gain_values) {
        if (g.second.first != 0) {
            hGain->Fill(g.second.first);
            hGain_relative_error->Fill(g.second.second / g.second.first);
        }
    }
    TF1 * fit = new TF1("fit","gaus");

    /// Adjust the fitting boundaries
    hGain->Fit("fit","","");
    double mean_gain = fit->GetParameter(1);
    double std_gain = fit->GetParameter(2);
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
    hGain_relative_error->Write();
    cout << "Mean_gain: "<<mean_gain<<", std_gain: "<<std_gain<<endl;
    int channels_hg = 0;
    int no_fit = 0;
    for(auto ih : NFEB){
        for (auto iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            connection = "FEB_" + to_string(ih) + "_Channel_" +  to_string(iCh);
            if(gain_values.find( connection ) != gain_values.end()) {
                double gain_value = gain_values.at(connection).first;
                if (gain_value == 0) {
                    cout << "Problem in " << connection << " number of fitted peaks less than 4" << endl;
                    ++no_fit;
                } else {
                    hGain_dist->Fill((gain_value - mean_gain) / mean_gain);
                    if ((gain_value < mean_gain - 3 * std_gain) || (gain_value > mean_gain + 3 * std_gain)) {
                        channels_hg += 1;
                        cout << "Problem in " << connection << ": " << gain_value << " out of 3 sigma" << endl;
                    }
                }
            }
        }
    }
    std::cout <<"gain: "<< mean_gain<<", sigma: "<<std_gain<<std::endl;
    std::cout << "Out of 3 sigma: "<<channels_hg<<", form "<<hGain->GetEntries()<<std::endl;
    std::cout <<"Number of gain = 0: "<<no_fit<<std::endl;
    for(auto connection_gain : gain_values){
        size_t pos_1 = connection_gain.first.find("FEB_");
        size_t pos_2 = connection_gain.first.find("_Channel_");
        unsigned int boardID = atoi(connection_gain.first.substr(pos_1 + 4, pos_2-pos_1 ).c_str());
        unsigned int ch256 = atoi(connection_gain.first.substr(pos_2+ 9,connection_gain.first.size() -14 - pos_2 ).c_str());
        unsigned int global_channel = (boardID << 8) | ch256;
        fout << global_channel << " "<<connection_gain.second.first<<" "<<connection_gain.second.second<<std::endl;

    }
    hGain_dist->Write();
    TTree* tr = tree.Get_Tree();
    std::string title = "Crate_"+ std::to_string(crate_number);

    GainMap->GetYaxis()->SetTitle("FEB");
    GainMap->GetXaxis()->SetTitle("Channel");
    GainMap->SetTitle(title.c_str());
    GainMap->Write();
    tr->Write();
    delete hGain;
    delete hGain_relative_error;
    delete hGain_dist;
    delete GainMap;
    wfile->Close();
    return 0;
}
