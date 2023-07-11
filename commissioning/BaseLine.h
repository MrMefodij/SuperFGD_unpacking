//
// Created by Maria on 11.07.2023.
//

#ifndef UNPACKING_BASELINE_H
#define UNPACKING_BASELINE_H

#include <TF1.h>
#include "Peaks.h"
#include <vector>
#include <map>
#include <TH1.h>


struct Elems{
    unsigned int _DAC;
    unsigned int _boardId;
    unsigned int _asicId_channelId;
    bool operator < (const Elems& other) const
    {
        if (_boardId == other._boardId) {
            if (_asicId_channelId == other._asicId_channelId)
                return _DAC < other._DAC;
            return _asicId_channelId < other._asicId_channelId;
        }
        return _boardId < other._boardId;
    }
    bool const operator == (const Elems& other) const
    {
        return _boardId == other._boardId && _asicId_channelId == other._asicId_channelId && _asicId_channelId == other._asicId_channelId;
    }
};

template <typename T>

struct Baseline_values{
    Baseline_values(T par_1, double par_2, unsigned int par_3 = 0) : _par_1(par_1), _par_2(par_2), _par_3(par_3) {}
    T _par_1;
    double _par_2;
    unsigned int _par_3;
};



class BaseLine {
public:
    BaseLine() = default;
    ~BaseLine() = default;
    void SFGD_BaseLine(TH1F* &hFEBCH_HG,TH1F* &hFEBCH_LG, std::pair<unsigned int,unsigned int> NFEBCh, std::vector<int> HG_LG);
    std::map<Elems,std::vector<Baseline_values<int>>> Find_BaseLine(std::string filename);
private:
    void Print_BaseLine(std::string filename);
    std::map<Elems,std::vector<Baseline_values<int>>> baseline;
    std::map<Elems,std::vector<Baseline_values<double>>> peaks_baseline;
    std::map<Elems,std::vector<Baseline_values<int>>> xml_data;
};


#endif //UNPACKING_BASELINE_H

