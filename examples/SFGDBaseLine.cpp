#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "TGraph.h"
#include <TCanvas.h>
#include <TTree.h>
#include <TF1.h>
#include <TMath.h>
#include <TROOT.h>
#include "Calibration.h"
#include "Files_Reader.h"
#include "MDpartEventSFGD.h"
#include "MDargumentHandler.h"
#include <map>
using namespace std;

struct baseline{
    Int_t FEB;
    Int_t channel;
    Double_t zero_peak_position;
    Int_t HG_LG;
    Int_t ASIC = channel / 32;
};

struct par_fit{
    Double_t min_elem;
    Double_t max_elem;

};
Double_t fpeaks(Double_t *x, Double_t *par) {
   return par[0]*x[0] + par[1];
}

int main(int argc, char **argv){

    string sFileName;
    ifstream fList("febs_files_list.list");
    vector<string> vFileNames;
    map<string,vector<baseline>> BaseLine;
    while (!fList.eof()) {
        fList >> sFileName;
        vFileNames.push_back(sFileName);
    }

    set<Int_t> NFEB;
    for(auto filename : vFileNames){
        string FileOutput =GetLocation(filename.c_str(), ".bin");
        cout << FileOutput <<endl;
        string toErase = "feb_7_11_13_BS_";
        size_t pos = FileOutput.find(toErase);
        if (pos != std::string::npos)
        // If found then erase it from string
            FileOutput.erase(0, pos+toErase.length());
        int HG_LG = atoi(FileOutput.c_str());
        cout << HG_LG<<endl;
        // Going through data file
        Calibration cl;
        File_Reader file_reader;
        file_reader.ReadFile(filename);
        // find numbers of measured FEB
        NFEB = file_reader.GetFEBNumbers();
        //get histograms with peaks
        for(const int ih : NFEB){
            for (Int_t iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
                TH1F* hFEBCH = file_reader.Get_hFEBCH(ih,iCh);
                string feb_channel = "FEB_" + to_string(ih) + "_Channel_" +  to_string(iCh);
                hFEBCH =  cl.SFGD_Calibration(hFEBCH, feb_channel);
                vector<Peaks> peaks = cl.Calibration_Par();
                if(peaks[0].GetHeight() > 70){

                    BaseLine[feb_channel].push_back({ih, iCh, peaks[0].GetPosition(),  HG_LG } );
                }
                else{
                    cout <<feb_channel<<": "<<peaks[0].GetHeight()<<" "<<peaks[0].GetPosition()<<endl;
                }
                delete hFEBCH;
            }  
        }
        // NFEB.clear();
    }

    Double_t par[2];
    map<string,vector<par_fit>> peaks_baseline;
    string rootFileOutput=GetLocation(vFileNames[0].c_str(), ".bin");
    rootFileOutput+="_baseline.root";
    TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);
    string FileOutput=GetLocation(vFileNames[0].c_str(), ".bin");
    FileOutput += ".txt";
    ofstream fout(FileOutput.c_str()); 
    for(auto b_l : BaseLine){
        if(b_l.second.size()==5){
        // cout <<b_l.second.size()<<endl;
            wfile->cd();
            c1->cd();
            TGraph* g = new TGraph();
            c1->cd();
            g->SetTitle(b_l.first.c_str());
            for(auto j : b_l.second)
                g->AddPoint(j.HG_LG,j.zero_peak_position);
            g->GetYaxis()->SetTitle("Baseline Peak position, [ADC ch]");
            g->GetXaxis()->SetTitle("BaselineDAC: HG");
            g->Draw("AC*");
            auto f = new TF1("Linear fit","[0]+x*[1]");
            g->Fit(f);
            f->GetParameters(&par[0]);
            if(par[1]!= 0){
                string st = "FEB_" + to_string(b_l.second[0].FEB) + "_ASIC_" +  to_string(b_l.second[0].ASIC);
                peaks_baseline[st].push_back({(-100 - par[0])/par[1], (0 - par[0])/par[1]});
                // fout << st<<"_CHANNEL_"<<b_l.second[0].channel<<": "<< (-70 - par[0])/par[1]<<" "<<(0 - par[0])/par[1]<<endl;
            }
            c1->Update();
            c1->Write();
            g->Clear();
        }
    }
    for(auto iCh : peaks_baseline){
        auto right_value = std::min_element(begin(iCh.second), end(iCh.second),
            [] (par_fit const& p1, par_fit const& p2)
            {
                return p1.max_elem < p2.max_elem;
            });
        auto left_value = std::max_element(begin(iCh.second), end(iCh.second),
            [] (par_fit const& p1, par_fit const& p2)
            {
                return p1.min_elem < p2.min_elem;
            });
        if(right_value->max_elem > left_value->min_elem)
            fout <<iCh.first<< ": ["<<left_value->min_elem<<","<<right_value->max_elem<<"]"<<endl;
        else{
            fout <<iCh.first<< ": "<< "No possible options"<<endl;}
    }
    c1->Clear();
    wfile->Close();
    return 0;
}

