//
//  Created by Maria on 27.09.2022 kolupanova@inr.ru
//

#ifndef Calibration_h
#define Calibration_h

#include <TSpectrum.h>
#include "Peaks.h"
#include <vector>
#include <map>
#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>
#include <TLegend.h>
#include <TLine.h>
#include <TGraphErrors.h>
#include <iostream>
#include <sstream>

class Calibration{
public:

    Calibration() = default;
    ~Calibration() = default;

    /// Returns histogram with calibration. Function takes  histogram
    /// and string FEB_#_Channel_# (#-number) as input and use library TSpectrum to find peaks position.
    /// If any peaks were found, each of them are fitted using the Gaussian distribution curve.
    /// If position of current peak is more then previous one, peak is added into peaks vector for this histogram.
    void SFGD_Calibration(TH1F* &hFEBCH, std::string connection, double mean_value, double sigma_mean_value,double max_value,double sigma_max_value) ;

    /// Calculates the gain as the average value between the positions of the peaks
    /// without taking into account zero peak.
    /// If number of peaks less than 3  the gain is considered equal to 0.
    void Gain_Calculation(TGraphErrors* gr,std::string connection);

    /// Returns Legend with peaks position, errors and gain for histogram which was used in function SFGD_Calibration
    TLegend* Calibration_Legend();

    /// Returns Legend with gain value
    TLegend* Get_Legend_for_TGraphErrors(){return _legend_for_peaks;}

    /// Returns line for mean/median position.
    TLine* Calibration_Line_Mean();
    TLine* Calibration_Line_Median();

    /// Returns all gain values as map with key value:  FEB_#_Channel_#. #-number
    std::map<std::string,std::pair<double,double>> GetGains(){return _gain_values;}

    /// Return current gain value
    double GetGain(){return _gain;}
    /// Returns peaks which were found during calibration
    std::vector<Peaks> Calibration_Par() const {return _peaks;}

    /// Returns mean/median
    double GetMean(){ return _mean;}
    double GetMedian(){ return _median;}

private:
    std::vector<Peaks> _peaks;
    std::map<std::string,std::pair<double,double>> _gain_values;
    unsigned int start_peak;
    double _gain,_mean_gain_value,_gain_error,_mean, _median, _max_height;
    TLegend* _legend_for_peaks;
    std::ostringstream os;
};


#endif /* Calibration_h */