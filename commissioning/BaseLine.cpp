//
// Created by Maria on 11.07.2023.
//

#include "BaseLine.h"
#include <fstream>
#include <iomanip>
#include "TGraph.h"
#include <TCanvas.h>
#include <TF1.h>
#include <TTree.h>
#include <TFile.h>
#include "Calibration.h"
#include <TMath.h>
#include <TROOT.h>
#define MIN_BASELINE_POSITION -100
#define MAX_BASELINE_POSITION 0


void BaseLine::SFGD_BaseLine(TH1F* (&hFEBCH_full)[2], std::pair<unsigned int,unsigned int> NFEBCh,std::vector<int> HG_LG){
//    std::vector<TH1F*> hFEBCH_full = {hFEBCH_HG,hFEBCH_LG};
    for(unsigned int i = 0; i <= 1; i++ ) {
        std::string connection = "FEB_" + std::to_string(NFEBCh.first) + "_Channel_" + std::to_string(NFEBCh.second);
        Calibration cl;
        cl.SFGD_Calibration(hFEBCH_full[i], connection);
        Elems el = {i + 2, NFEBCh.first, NFEBCh.second};
        Peaks peak = cl.Calibration_Par()[0];
        if (peak.GetPosition() != 0)
            _baseline[el].push_back({HG_LG[i], peak.GetPosition()});

    }


}

void BaseLine::Print_BaseLine(std::string& filename,unsigned int files_number){
    auto *wfile = new TFile((filename+"_baseline.root").c_str(), "RECREATE");
    auto *c1 = new TCanvas("c1","",0,10,700,500);
    TDirectory *FEBdir;
    for(const auto &b_l : _baseline){
        std::string s = "FEB_"+std::to_string(b_l.first._boardId);
        if(wfile->GetDirectory(s.c_str()) == nullptr)
           FEBdir = wfile->mkdir(s.c_str());
        s+="_Channel_"+std::to_string(b_l.first._asicId_channelId);
        if(b_l.second.size()==files_number){
            wfile->cd();
            auto g = new TGraph();
            c1->cd();
            if(b_l.first._DAC == 2) s+="_HG";
            if(b_l.first._DAC == 3) s+="_LG";
            g->SetTitle(s.c_str());
            for(auto j : b_l.second)
                g->AddPoint(j._par_1,j._par_2);
            g->GetYaxis()->SetTitle("Baseline Peak position, [ADC ch]");
            g->GetXaxis()->SetTitle("BaselineDAC: HG/LG");
            g->Draw("AC*");
            auto f = new TF1("Linear fit","[0]+x*[1]");
            g->Fit(f, "Q");
            double par[2];
            f->GetParameters(&par[0]);
            if(par[1]!= 0){
                Elems el = {b_l.first._DAC,b_l.first._boardId,b_l.first._asicId_channelId/32};
                Baseline_values min_max_baseline_channel = {(MIN_BASELINE_POSITION - par[0])/par[1], (MAX_BASELINE_POSITION - par[0])/par[1], b_l.first._asicId_channelId};
                _peaks_baseline[el].push_back(min_max_baseline_channel);
            }
            FEBdir->cd();
            g->Write(s.c_str());
            c1->Clear();
            delete g;
            delete f;
        }
        else{
            std::cout << s<<" number of points: "<<b_l.second.size()<<std::endl;
        }
    }
    delete c1;
    delete FEBdir;
    delete wfile;
}

std::map<Elems,std::vector<Baseline_values<int>>> BaseLine::Find_BaseLine(std::string filename, unsigned int files_number){
    Print_BaseLine(filename, files_number);
    std::ofstream fout((filename+".txt").c_str());
    for(auto iCh : _peaks_baseline)
    {
        auto right_value = std::min_element(begin(iCh.second), end(iCh.second),
        [] (Baseline_values<double> const& p1, Baseline_values<double>const& p2)
                {
                    return p1._par_2 < p2._par_2;
                });
        auto left_value = std::max_element(begin(iCh.second), end(iCh.second),
        [] (Baseline_values<double> const& p1, Baseline_values<double>const& p2)
                {
                    return p1._par_1 < p2._par_1;
                });
        std::string s = "FEB_" + std::to_string(iCh.first._boardId)+"_ASIC_" + std::to_string(iCh.first._asicId_channelId);
        if(iCh.first._DAC == 2) s+="_HG";
        if(iCh.first._DAC == 3) s+="_LG";

        if(right_value->_par_2 < left_value->_par_1) {
            fout << "No possible options: " << s << ": ["<<left_value->_par_1<<","<<right_value->_par_2<<"]"<<std::endl;
            for(const auto& borders: iCh.second){
                if(borders._par_1 > right_value->_par_2 ||  borders._par_2 < left_value->_par_1){
                    std::cout << "Problem in "<<s<<"_CHANNEL_"<< borders._par_3 << ": ["<<borders._par_1<<","<<borders._par_2<<"]"<<std::endl;
                }
            }
        }
        else{
            fout << s << ": [" << left_value->_par_1 << "," << right_value->_par_2 << "]" << std::endl;
            Elems el = {0,iCh.first._boardId, iCh.first._asicId_channelId};
            if(iCh.first._DAC == 2)
                _xml_data[el].push_back({int((left_value->_par_1+right_value->_par_2)/2),2.0});
            if(iCh.first._DAC == 3)
                _xml_data[el].push_back({int((left_value->_par_1+right_value->_par_2)/2),3.0});
        }
    }
    return _xml_data;
}
