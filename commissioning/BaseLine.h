//
// Created by Maria on 11.07.2023.
//

#ifndef UNPACKING_BASELINE_H
#define UNPACKING_BASELINE_H

#include <TF1.h>
#include "Peaks.h"
#include "CrateSlotAsic_missingChs.h"
#include "Files_Reader.h"
#include "SFGD_defines.h"
#include <vector>
#include <set>
#include <map>
#include <TH1.h>
#include <TFile.h>
#include <TSpectrum.h>

struct Connection {
    unsigned int _boardId;
    unsigned int _asicId_channelId;

    bool operator<(const Connection &other) const {
        if (_boardId == other._boardId)
            return _asicId_channelId < other._asicId_channelId;
        return _boardId < other._boardId;
    }

    bool const operator==(const Connection &other) const {
        return _boardId == other._boardId && _asicId_channelId == other._asicId_channelId &&
        _asicId_channelId == other._asicId_channelId;
    }
};

/// Structure is use to keep DAC, FEB and ASIC/Channel (depends on the case) values.
struct Elems {
    unsigned int _DAC;
    Connection _connection;

    bool operator<(const Elems &other) const {
        if (_connection == other._connection) {
            return _DAC < other._DAC;
        }
        return _connection < other._connection;
    }

    bool const operator==(const Elems &other) const {
        return _connection == other._connection;
    }
};


/// Structure is used to keep different baseline values, template is used in order not to create lots of structures.
/// Sometimes _par_3 isn't used that's why it equals to 0 by default.
template<typename T>
struct Baseline_values {
    Baseline_values(T par_1, T par_2, unsigned int par_3 = 0) : _par_1(par_1), _par_2(par_2), _par_3(par_3) {}

    T _par_1;
    T _par_2;
    unsigned int _par_3;
};

struct All_values {
    int _param;
    unsigned int _HG_LG;
    unsigned int _board_id;
    unsigned int _asic;

    bool const operator==(const All_values &other) const {
        return _param == other._param && _HG_LG == other._HG_LG && _board_id == other._board_id &&
               _asic == other._asic;
    }

    bool operator<(const All_values &other) const {
        if (_HG_LG == other._HG_LG) {
            if (_param == other._param) {
                if (_board_id == other._board_id) {
                    return _asic < other._asic;
                }
                return _board_id < other._board_id;
            }
            return _param < other._param;
        }
        return _HG_LG < other._HG_LG;
    }
};


class BaseLine {
public:
    BaseLine(unsigned int current_feb, unsigned int files_num);
    ~BaseLine() = default;

    /// Function takes 2 histograms for HG and LG, pair FEB, Channel numbers and vector HG_LG
    /// which consist of 2 values HG DAC and LG DAC  as input.
    /// Function using class Calibration finds 0 peak of histograms and creates map _baseline which has Structure Elems (DAC, FEB, and Channel number) as key
    /// and Baseline_values (DAC, peak position) as value.
    void SFGD_BaseLine(unsigned int file_num, const unsigned int (&HG_LG_baseline_value)[2], Connection NFEBCh, const unsigned int (&HG_LG)[2]);

    /// Returns data for xml as map with key Elems (DAC, FEB, and ASIC number) and value as vector
    /// of Structure Baseline_values which has recommended baseline position and HG/LG (2 - HG, 3 - LG).
    /// After reading all files with different DACs function call Print_BaseLine, uses  _peaks_baseline to find baseline borders for each ASIC
    /// (max value among 32 channel for baseline in -100 position and min value among 32 channel for baseline in 0 position)
    /// and find baseline position as the midpoint of the segment. If left border is bigger than right you'll see message:
    /// "Problem in # FEB_#_ASIC_#_HG/LG_CHANNEL_#" in terminal
    std::map<Elems, std::vector<Baseline_values<unsigned int>>> Find_BaseLine(std::string &filename);

    /// Function creates root file, draws and fits TGraphs for each FEB, Channel and HG/LG
    /// a total of 2*256*(Number of FEB) graphs are obtained (2 - different plots for HG and LG, 256 - number of channels)
    /// And creates _peaks_baseline map with  Elems (DAC, FEB, ASIC number) as key and Baseline_values as value which consists of 2 params with  -100 and 0 baseline position and Channel number as _par_3.
    void Print_BaseLine(TFile *&wfile, unsigned int files_number);

    std::map<Connection, unsigned int> Get_Recommended_Threshold() { return _baseLine_threshold; }

    std::map<Elems, std::vector<Baseline_values<double>>> GetBaselineValues() { return _peaks_baseline; };

    std::map<All_values, std::vector<unsigned int>> GetBadChannels() { return _bad_channels; };

    unsigned int FindLocalMax( std::vector<unsigned int>& vec);

    void doBaselineStudy(const std::string& filename, unsigned int fileNum, CrateSlotAsic_missingChs& crateSlotAsicMissingChs);
private:

    /// Used for keeping FEB and Channel number as key and points for baseline study (DAC, peak position) as value.
    std::map<Elems, std::vector<Baseline_values<unsigned int>>> _baseline;

    /// Used for keeping FEB and ASIC number as key and baseline borders for each ASIC as value.
    std::map<Elems, std::vector<Baseline_values<double>>> _peaks_baseline;

    /// Used for keeping FEB and ASIC number as key, recommended baseline position and HG/LG for each ASIC as value.
    std::map<Elems, std::vector<Baseline_values<unsigned int>>> _xml_data;

    std::map<Connection, unsigned int> _baseLine_threshold;
    std::map<All_values, std::vector<unsigned int>> _bad_channels;

    unsigned int _num_feb = 0;



//    TSpectrum *_s;
};

#endif //UNPACKING_BASELINE_H

