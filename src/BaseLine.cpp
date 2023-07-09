//
// Created by Maria on 09.07.2023.
//

#include <fstream>
#include <iomanip>
#include <sstream>
#include "TGraph.h"
#include <TCanvas.h>
#include <TF1.h>
#include <TTree.h>
#include <TFile.h>
#include "BaseLine.h"
#include "Calibration.h"
#include <TMath.h>
#include <TROOT.h>

#define MIN_BASELINE_POSITION -70
#define MAX_BASELINE_POSITION 0
#define BASELINE_HEIGHT 50

void BaseLine::SFGD_BaseLine(TH1F* &hFEBCH_HG,TH1F* &hFEBCH_LG, std::pair<Int_t,Int_t> NFEBCh,Int_t HG_LG){
    std::vector<TH1F*> hFEBCH_full = {hFEBCH_HG,hFEBCH_LG};
    for(int i = 0; i < hFEBCH_full.size(); i++ ) {
        if(NFEBCh.first == 0)
            NFEBCh.first = 247;
        if(NFEBCh.first == 1)
            NFEBCh.first = 251;
        if(NFEBCh.first == 2)
            NFEBCh.first = 253;
        std::string connection = "FEB_" + std::to_string(NFEBCh.first) + "_Channel_" + std::to_string(NFEBCh.second);
        Calibration cl;
        TH1F *th = cl.SFGD_Calibration(hFEBCH_full[i], connection);
        Elems el = {i + 2, NFEBCh.first, NFEBCh.second};
        Peaks peak = cl.Calibration_Par()[0];
        if (peak.GetHeight() > BASELINE_HEIGHT && peak.GetPosition() != 0)
            baseline[el].push_back({HG_LG, peak.GetPosition()});
    }
}

void BaseLine::Print_BaseLine(std::string filename){
    TFile *wfile = new TFile((filename+"_baseline.root").c_str(), "RECREATE");
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);
    for(auto b_l : baseline){
        if(b_l.second.size()==4){
            wfile->cd();
            TGraph* g = new TGraph();
            c1->cd();
            std::string s = "FEB_"+std::to_string(b_l.first.FEB)+"_Channel_"+std::to_string(b_l.first.ASIC_Channel);
            if(b_l.first.Gain == 2) s+="_HG";
            if(b_l.first.Gain == 3) s+="_LG";
            g->SetTitle(s.c_str());
            for(auto j : b_l.second)
                g->AddPoint(j.par_1,j.par_2);
            g->GetYaxis()->SetTitle("Baseline Peak position, [ADC ch]");
            g->GetXaxis()->SetTitle("BaselineDAC: HG/LG");
            g->Draw("AC*");
            auto f = new TF1("Linear fit","[0]+x*[1]");
            g->Fit(f);
            Double_t par[2];
            f->GetParameters(&par[0]);
            if(par[1]!= 0){
                    Elems el = {b_l.first.Gain,b_l.first.FEB,b_l.first.ASIC_Channel/32};
                    Baseline_values min_max_baseline_channel = {(MIN_BASELINE_POSITION - par[0])/par[1], (MAX_BASELINE_POSITION - par[0])/par[1], b_l.first.ASIC_Channel};
                    peaks_baseline[el].push_back(min_max_baseline_channel);
            }
            c1->Update();
            c1->Write();
            g->Clear();
        }
    }
    c1->Clear();
    wfile->Close();
}

std::map<Elems,std::vector<Baseline_values<Int_t>>> BaseLine::Find_BaseLine(std::string filename){
    Print_BaseLine(filename);
    std::ofstream fout((filename+".txt").c_str());
    for(auto iCh : peaks_baseline)
    {
        auto right_value = std::min_element(begin(iCh.second), end(iCh.second),
                                            [] (Baseline_values<Double_t> const& p1, Baseline_values<Double_t>const& p2)
                                            {
                                                return p1.par_2 < p2.par_2;
                                            });
        auto left_value = std::max_element(begin(iCh.second), end(iCh.second),
                                           [] (Baseline_values<Double_t> const& p1, Baseline_values<Double_t>const& p2)
                                           {
                                               return p1.par_1 < p2.par_1;
                                           });
        std::string s = "FEB_"+std::to_string(iCh.first.FEB)+"_ASIC_"+std::to_string(iCh.first.ASIC_Channel);
        if(iCh.first.Gain == 2) s+="_HG";
        if(iCh.first.Gain == 3) s+="_LG";

        if(!(right_value->par_2 > left_value->par_1)) {
            fout << "No possible options: " << s << ": ["<<left_value->par_1<<","<<right_value->par_2<<"]"<<std::endl;
//            for(int ih = 0; ih < iCh.second.size(); ih++){
//                if(iCh.second[ih].par_1 > right_value->par_2 ||  iCh.second[ih].par_2 < left_value->par_1){
//                    std::cout << "Problem in "<<s<<"_CHANNEL_"<< iCh.second[ih].par_3 << ": ["<<iCh.second[ih].par_1<<","<<iCh.second[ih].par_2<<"]"<<std::endl;
//                }
//            }
        }
        else{
            fout << s << ": [" << left_value->par_1 << "," << right_value->par_2 << "]" << std::endl;
            Elems el = {0,iCh.first.FEB, iCh.first.ASIC_Channel};
            if(iCh.first.Gain == 2)
                xml_data[el].push_back({int((left_value->par_1+right_value->par_2)/2),2.0});
            if(iCh.first.Gain == 3)
                xml_data[el].push_back({int((left_value->par_1+right_value->par_2)/2),3.0});
        }
    }
    return xml_data;
}
