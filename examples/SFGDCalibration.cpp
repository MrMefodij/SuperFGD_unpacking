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

#define MEAN_VALUE 350
#define ENTRIES_NUM 60

int main(int argc, char **argv){

    string stringBuf;
    MDargumentHandler argh("Example of sfgd baseline.");
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
            hFEBCH[i][j] = new TH1F(sCh.c_str(),sCh.c_str(),  701, 0, 700);
        }
    }

    /// Connection map (read csv)
    string mapFile = "../connection_map/SFG_Geometry_Map_v11.csv";
    if(getenv("UNPACKING_ROOT")!=NULL){
        mapFile = (string)getenv("UNPACKING_ROOT") + "/connection_map/SFG_Geometry_Map_v11.csv";
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


    Tree tree;
    /// Get histograms with peaks
    for(unsigned int ih : NFEB){
        TDirectory *FEBdir = wfile->mkdir(("FEB_"+to_string(ih)).c_str());
        FEBdir->cd();
        map<unsigned int, vector<double>> gain_values;
        for (unsigned int iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            unsigned int slot_id = ih & 0x0f;
            string feb_channel = "FEB_" + to_string(ih) + "_Channel_" + to_string(iCh);
            cl.SFGD_Calibration(hFEBCH[slot_id][iCh], feb_channel);
            TGraphErrors* gr = new TGraphErrors;
            cl.Gain_Calculation(gr, feb_channel);
            /// for tree
            GlGeomPosition glgeom = connectionMap.GetGlobalGeomPosition(ih,iCh);
            GlChannelPosition glch = connectionMap.GetGlobalChannelPosition(ih,iCh);
            tree._crate = ih >> 4;
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
            if(hFEBCH[slot_id][iCh]->GetMean() < MEAN_VALUE)
                gain_values[iCh / 64].push_back(cl.GetGain());

            if(hFEBCH[slot_id][iCh]->GetEntries() > 0) {
                auto *l_median = cl.Calibration_Line_Median();
                auto *l_mean = cl.Calibration_Line_Mean();
                l_median->Draw();
                l_mean->Draw();
                auto *legend = cl.Calibration_Legend();
                legend->Draw();
                c1->Update();
                c1->Write(feb_channel.c_str());
            }
            gr->Draw("APE");
            auto legend = cl.Get_Legend_for_TGraphErrors();
            legend->Draw("LSame");
            c1->Update();
            c1->Write((feb_channel+"_graph").c_str());
            delete gr;
            delete hFEBCH[slot_id][iCh];
        }
        for(int i = 0; i < 4; i++){
            fout << "FEB_" << ih << "_PCB_" <<i;
            if( gain_values[i].size() > ENTRIES_NUM){
                gain_values[i].erase(std::remove_if(gain_values[i].begin(), gain_values[i].end(), [&](const double &x)
                {
                    return x == 0;
                }),gain_values[i].end());
                if(!gain_values[i].empty()) {
                    double mean = std::accumulate(gain_values[i].begin(), gain_values[i].end(), 0.0) / gain_values[i].size();
                    fout << " mean gain: " << mean << std::endl;
                }
                else{
                    fout  << " Insufficient number of peaks" << std::endl;
                }
            }
            else{
                fout <<" problem with LED"<<std::endl;
            }
        }
    }

    /// Find channels with std more than 3 sigma
    map<string,double> gain = cl.GetGains();
    string connection;
    TH1F* hGain = new TH1F("Gain_distribution", "Gain_distribution",  400, 0, 100);
    for (auto g : gain) {
        if (g.second != 0) hGain->Fill(g.second);
    }
    TF1 * fit = new TF1("fit","gaus");

    /// Adjust the fitting boundaries
    hGain->Fit("fit","","");
    double mean_gain = fit->GetParameter(1);
    double std_gain = fit->GetParameter(2);
    wfile->cd();
    hGain->Write();
    cout << "Mean_gain: "<<mean_gain<<", std_gain: "<<std_gain<<endl;

    for(auto ih : NFEB){
        for (auto iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            connection = "FEB_" + to_string(ih) + "_Channel_" +  to_string(iCh);
            double gain_value = gain.at(connection);
            if((gain_value < mean_gain - 3*std_gain) || (gain_value > mean_gain + 3*std_gain)){
                if(gain_value == 0) cout<< "Problem in " << connection <<" number of fitted peaks less than 4"<<endl;
                else{ cout << "Problem in "<<connection <<": "<< gain_value <<endl;}
            }
        }
    }
    TTree* tr = tree.Get_Tree();
    tr->Write();
    wfile->Close();
    return 0;
}
