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

/// Structure is use to keep DAC, FEB and ASIC/Channel (depends on the case) values.
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

/// Structure is used to keep different baseline values, template is used in order not to create lots of structures.
/// Sometimes _par_3 isn't used that's why it equals to 0 by default.
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

    /// Function takes 2 histograms for HG and LG, pair FEB, Channel numbers and vector HG_LG
    /// which consist of 2 values HG DAC and LG DAC  as input.
    /// Function using class Calibration finds 0 peak of histograms and creates map _baseline which has Structure Elems (DAC, FEB, and Channel number) as key
    /// and Baseline_values (DAC, peak position) as value.
    void SFGD_BaseLine(TH1F* (&hFEBCH_full)[2], std::pair<unsigned int,unsigned int> NFEBCh, std::vector<int> HG_LG);

    /// Returns data for xml as map with key Elems (DAC, FEB, and ASIC number) and value as vector
    /// of Structure Baseline_values which has recommended baseline position and HG/LG (2 - HG, 3 - LG).
    /// After reading all files with different DACs function call Print_BaseLine, uses  _peaks_baseline to find baseline borders for each ASIC
    /// (max value among 32 channel for baseline in -100 position and min value among 32 channel for baseline in 0 position)
    /// and find baseline position as the midpoint of the segment. If left border is bigger than right you'll see message:
    /// "Problem in # FEB_#_ASIC_#_HG/LG_CHANNEL_#" in terminal
    std::map<Elems,std::vector<Baseline_values<int>>> Find_BaseLine(std::string filename,unsigned int files_number);
private:

    /// Function creates root file, draws and fits TGraphs for each FEB, Channel and HG/LG
    /// a total of 2*256*(Number of FEB) graphs are obtained (2 - different plots for HG and LG, 256 - number of channels)
    /// And creates _peaks_baseline map with  Elems (DAC, FEB, ASIC number) as key and Baseline_values as value which consists of 2 params with  -100 and 0 baseline position and Channel number as _par_3.
    void Print_BaseLine(std::string& filename,unsigned int files_number);

    /// Used for keeping FEB and Channel number as key and points for baseline study (DAC, peak position) as value.
    std::map<Elems,std::vector<Baseline_values<int>>> _baseline;

    /// Used for keeping FEB and ASIC number as key and baseline borders for each ASIC as value.
    std::map<Elems,std::vector<Baseline_values<double>>> _peaks_baseline;

    /// Used for keeping FEB and ASIC number as key, recommended baseline position and HG/LG for each ASIC as value.
    std::map<Elems,std::vector<Baseline_values<int>>> _xml_data;
};


#endif //UNPACKING_BASELINE_H

