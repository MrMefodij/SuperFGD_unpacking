//
// Created by Maria on 18.07.2023 kolupanova@inr.ru
//

#ifndef UNPACKING_THRESHOLDSTUDY_H
#define UNPACKING_THRESHOLDSTUDY_H

#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TSpectrum.h>
#include <vector>

struct Threshold{
    unsigned int _DAC;
    unsigned int _ADC;
    bool operator < (const Threshold& other) const
    {
            if (_DAC == other._DAC)
                return _ADC < other._ADC;
            return _DAC < other._DAC;
    }
    bool operator == (const Threshold& other) const
    {
        return  (_ADC == other._ADC && _DAC == other._DAC);
    }
    bool operator != (const Threshold& other) const
    {
        return  (_ADC != other._ADC || _DAC != other._DAC);
    }
};

struct Coord{
    unsigned int _x_prev;
    unsigned int _x_next;
    double _y;
};

class ThresholdStudy {
public:
    ThresholdStudy() = default;
    ThresholdStudy(std::vector<Threshold> threshold) : _threshold(threshold){};
    ~ThresholdStudy() {
        delete _g;
    };


    /// Function takes histograms for all DACs, DAC values and bin_entries (number of entries in bin below which the values aren't taken into account)
    /// For each DAC algorithm finds out the smallest ADC value (with entries > bin_entries in bin) and writes them down to vector _threshold
    /// If there is something wrong with data, you will see message: "Error: Something wrong with DAC #" in terminal
    void FindThreshold(std::map<unsigned int, TH1F *>& DAC10b_hFEBH, const unsigned int bin_entries = 20);

    /// Returns vector of DACs values which are correspond to p.e. peaks after threshold study
    std::vector<unsigned int> GetDAC() {return _DAC;}

    /// Returns vector of ADCs values which obtained for p.e. peaks
    std::vector<double> GetPars(){return _pars;}

    /// Returns TGraph which has linear and square fit.
    /// Functions takes parameter tan_angle - max angle at which points are considered to lie on the straight line (2 - is recommended).
    /// Algorithm find out points for fitting straight lines which are correspond to the p.e. peaks, then the segments of DAC which don't fall into
    /// the previous ares are fitted by square function. The values equal to the 1.25, 2.25 ... p.e. are searched for
    /// and using square fit parameters DACs values are obtained.
    TGraph* PrintThreshold( double tan_angle = 2.0);

private:
    /// Used for keeping DAC and ADC values received from histograms
    std::vector<Threshold>  _threshold;

    /// Used for keeping DACs values which are correspond to p.e. peaks after threshold study
    std::vector<unsigned int>  _DAC;

    ///  Used for keeping ADCs values which values which obtained for p.e. peaks
    std::vector<double> _pars;

    /// Used for keeping section for square fit
    std::vector<Coord> _section;

    TGraph* _g = new TGraph();

    TSpectrum* _s = new TSpectrum(50);
};



#endif //UNPACKING_THRESHOLDSTUDY_H
