//
// Created by Maria on 11.07.2023.
//

#include "BaseLine.h"
#include <fstream>
#include <iomanip>
#include "TGraph.h"
#include <TF1.h>
#include <TMath.h>
#include <TROOT.h>
#include "SFGD_defines.h"
#include <TSpectrum.h>
#include <mutex>
#define MIN_BASELINE_POSITION -115
#define MAX_BASELINE_POSITION 0
#define MIN_BASELINE_PEAK 30

std::mutex mtx;

BaseLine::BaseLine(unsigned int current_feb, unsigned int files_num) : _num_feb(current_feb){
    for(unsigned int i = 0; i < current_feb; ++i){
        for(unsigned int j = 0; j < SFGD_FEB_NCHANNELS; ++j){
            Connection c = {i, j};
            for(auto n = 0; n < files_num; ++n) {
                if (_baseline.find({2, c}) == _baseline.end())
                    _baseline[{2, c}] = {{0, 0}};
                else{
                    _baseline[{2, c}].push_back({0, 0});
                }
                if (_baseline.find({3, c}) == _baseline.end())
                    _baseline[{3, c}] = {{0, 0}};
                else{
                    _baseline[{3, c}].push_back({0, 0});
                }
            }
        }
    }
}


void BaseLine::SFGD_BaseLine(unsigned int file_num, const unsigned int (&HG_LG_baseline_value)[2], Connection NFEBCh,const unsigned int (&HG_LG)[2]){

    for(unsigned int i = 0; i <= 1; i++ ) {
        /// Add max bin in HG/LG histogram
        std::lock_guard<std::mutex> guard(mtx);
        Elems el = {i + 2, NFEBCh};
        _baseline[el][file_num] = {HG_LG[i], HG_LG_baseline_value[i]};

    }
}

unsigned int BaseLine::FindLocalMax( std::vector<unsigned int>& vec){
    for(unsigned int i = 0; i < vec.size() - 1; ++i){
        if(vec[i] > vec[i +1] && vec[i] > MIN_BASELINE_PEAK && vec[i+1] > MIN_BASELINE_PEAK)
            return i;
    }
    return 0;
}
void BaseLine::doBaselineStudy(const std::string& filename, unsigned int fileNum, CrateSlotAsic_missingChs& crateSlotAsicMissingChs){
    std::cout << "Decoding the input file "<< filename << std::endl;
    unsigned int bin_size = 4;
    std::vector<std::vector<unsigned int>> hFEBCH_HG(_num_feb * (SFGD_FEB_NCHANNELS + 1), std::vector<unsigned int>(4096 / bin_size, 0));
    std::vector<std::vector<unsigned int>> hFEBCH_LG(_num_feb * (SFGD_FEB_NCHANNELS + 1), std::vector<unsigned int>(4096 / bin_size, 0));
    unsigned int HG_LG[2];
    std::string FileOutput = GetLocation(filename, ".bin");
    size_t pos_1 = FileOutput.find("HG");
    size_t pos_2 = FileOutput.find("LG");
    HG_LG[0] = stoi(FileOutput.substr(pos_1 + 2, pos_2 - pos_1 - 5));
    HG_LG[1] = stoi(FileOutput.substr(pos_2 + 2));
    /// Going through data file
    File_Reader file_reader;
    std::cout << filename <<" started reading..."<<std::endl;
    file_reader.ReadFile_for_Baseline(filename,hFEBCH_HG,hFEBCH_LG);
    std::cout << filename <<" finished reading ..."<<std::endl;
    try{
        if(file_reader.GetFEBNumbers().empty()) {
            std::cout << filename << " is empty"<<std::endl;
            throw ;
        }
    }
    catch(...){
        std::cerr << "Something wrong with file "<< filename <<std::endl;
    }
    std::set<unsigned int> NFEB = file_reader.GetFEBNumbers();

    ///get histograms with peaks
    for (const unsigned int& ih: NFEB) {
        unsigned int slot_id = ih & 0x0f;
        unsigned int crate_number = ih >> 4;
        for (unsigned int iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            Mapping map = {crate_number, slot_id, iCh / 32};
            unsigned int hfeb_num;
            if(_num_feb == SFGD_SLOT) hfeb_num = slot_id;
            else{
                hfeb_num = ih;
            }
            if(!crateSlotAsicMissingChs.Is_Missing_Chs(map, iCh) && !crateSlotAsicMissingChs.Is_Missing_FEB(ih) && !crateSlotAsicMissingChs.Is_Missing_ASIC(map)) {
                auto index_HG = FindLocalMax(hFEBCH_HG[hfeb_num * SFGD_FEB_NCHANNELS +  iCh]);
                auto index_LG = FindLocalMax(hFEBCH_LG[hfeb_num * SFGD_FEB_NCHANNELS +  iCh]);
                unsigned int hFull[2] = { bin_size * index_HG, bin_size * index_LG};
                SFGD_BaseLine(fileNum, hFull, {hfeb_num, iCh}, HG_LG);
            }
        }
    }
}

void BaseLine::Print_BaseLine(TFile* &wfile,unsigned int files_number){

    for(const auto &b_l : _baseline){
        std::string s = "FEB_"+std::to_string(b_l.first._connection._boardId);
        if(wfile->GetDirectory(s.c_str()) == nullptr) {
            wfile->mkdir(s.c_str());
            wfile->cd(s.c_str());
        }
        s+="_Channel_"+std::to_string(b_l.first._connection._asicId_channelId);
        auto g = new TGraph();
        if(b_l.first._DAC == 2) s+="_HG";
        if(b_l.first._DAC == 3) s+="_LG";
        g->SetTitle(s.c_str());
        int par_1 = 0;
        for(auto j : b_l.second) {
            if( par_1 != j._par_1)
                g->AddPoint(j._par_1, j._par_2);
            par_1 = j._par_1;
        }
        if(g->GetN() == files_number) {
            g->GetYaxis()->SetTitle("Baseline Peak position, [ADC ch]");
            g->GetXaxis()->SetTitle("BaselineDAC: HG/LG");
            g->Draw("AC*");
            auto f = new TF1("Linear fit", "[0]+x*[1]");
            g->Fit(f, "Q");
            double par[2];
            f->GetParameters(&par[0]);
            if (par[1] != 0) {
                Elems el = {b_l.first._DAC,
                            {b_l.first._connection._boardId, b_l.first._connection._asicId_channelId / 32}};
                Baseline_values<double> min_max_baseline_channel = {
                        (MIN_BASELINE_POSITION - (double) par[0]) / (double) par[1],
                        (MAX_BASELINE_POSITION - (double) par[0]) / (double) par[1],
                        b_l.first._connection._asicId_channelId};
//                if(min_max_baseline_channel._par_1>0)
                _peaks_baseline[el].push_back(min_max_baseline_channel);
                g->Write(s.c_str());
            }
            delete f;
        }else{
            std::cout << s<<" number of points: "<<g->GetN()<<" ";
//            for(auto  l : b_l.second )
//            std::cout<<l._par_1<<" "<<l._par_2<<";\t";
            std::cout <<std::endl;
        }
        delete g;
    }
}

std::map<Elems,std::vector<Baseline_values<unsigned int>>> BaseLine::Find_BaseLine(std::string& filename){

    std::ofstream fout((filename+"/Baseline.txt").c_str());
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
        std::string s = "FEB_" + std::to_string(iCh.first._connection._boardId)+"_ASIC_" + std::to_string(iCh.first._connection._asicId_channelId);
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
            Elems el {0,iCh.first._connection};
            if(iCh.first._DAC == 2)
                _xml_data[el].push_back({(unsigned int)((left_value->_par_1+right_value->_par_2)/2),2});
            if(iCh.first._DAC == 3)
                _xml_data[el].push_back({(unsigned int)((left_value->_par_1+right_value->_par_2)/2),3});
        }
    }
    return _xml_data;
}
