//
// Created by Maria on 09.07.2023.
//

#ifndef UNPACKING_BASELINE_H
#define UNPACKING_BASELINE_H

#include <TF1.h>
#include "Peaks.h"
#include <vector>
#include <map>
#include <TH1.h>


struct Elems{
    Int_t Gain;
    Int_t FEB;
    Int_t ASIC_Channel;
    bool operator < (const Elems& other) const
    {
        if (FEB == other.FEB) {
            if (ASIC_Channel == other.ASIC_Channel)
                return Gain < other.Gain;
            return ASIC_Channel < other.ASIC_Channel;
        }
        return FEB < other.FEB;
    }
    bool const operator == (const Elems& other) const
    {
        return FEB == other.FEB && ASIC_Channel == other.ASIC_Channel && ASIC_Channel == other.ASIC_Channel;
    }
};

template <typename T>

struct Baseline_values{
    Baseline_values(T par_1_, Double_t par_2_, Int_t par_3_ = 0) : par_1(par_1_), par_2(par_2_), par_3(par_3_) {}
    T par_1;
    Double_t par_2;
    Int_t par_3;
};



class BaseLine {
public:
    BaseLine() = default;
    ~BaseLine() = default;
    void SFGD_BaseLine(TH1F* &hFEBCH_HG,TH1F* &hFEBCH_LG, std::pair<Int_t,Int_t> NFEBCh, Int_t HG_LG);
    std::map<Elems,std::vector<Baseline_values<Int_t>>> Find_BaseLine(std::string filename);
    private:
        void Print_BaseLine(std::string filename);
        std::map<Elems,std::vector<Baseline_values<Int_t>>> baseline;
        std::map<Elems,std::vector<Baseline_values<Double_t>>> peaks_baseline;
        std::map<Elems,std::vector<Baseline_values<Int_t>>> xml_data;
};


#endif //UNPACKING_BASELINE_H
