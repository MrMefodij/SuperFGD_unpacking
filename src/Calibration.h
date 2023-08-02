
//  Created by Maria on 27.09.2022.
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




class Calibration{
public:

    Calibration() = default;
    ~Calibration() = default;

    /// Returns histogram with calibration. Function takes  histogram
    /// and string FEB_#_Channel_# (#-number) as input and use library TSpectrum to find peaks position.
    /// If any peaks were found, each of them are fitted using the Gaussian distribution curve.
    /// If position of current peak is more then previous one, peak is added into peaks vector for this histogram.
    void SFGD_Calibration(TH1F* &hFEBCH, std::string connection);

    /// Returns Legend with peaks position, errors and gain for histogram which was used in function SFGD_Calibration
    TLegend* Calibration_Legend();

    /// Returns all gain values as map with key value:  FEB_#_Channel_#. #-number
    std::map<std::string,double> GetGain(){return _gain_values;};

    /// Returns peaks which were found during calibration
    std::vector<Peaks> Calibration_Par() const {return _peaks;}

private:
    std::vector<Peaks> _peaks;
    double _gain;
    double _gain_error;
    std::map<std::string,double> _gain_values;

    /// Calculates the gain as the average value between the positions of the peaks without
    /// taking into account zero peak.
    /// If number of peaks less than 3  the gain is considered equal to 0.
    void Gain_Calculation();

};


#endif /* Calibration_h */