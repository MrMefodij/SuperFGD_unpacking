
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
    TH1F* SFGD_Calibration(TH1F* &hFEBCH, std::string connection);
    TLegend* Calibration_Legend();
    std::map<std::string,Double_t> GetGain(){return gain_values;};
    std::vector<Peaks> Calibration_Par() const {return peaks;}

private:
    std::vector<Peaks> peaks;
    Double_t gain;
    Double_t gain_error;
    std::map<std::string,Double_t> gain_values;
    void Gain_Calculation();
    Double_t Calibration_Gain() const{return gain;}
    Double_t Calibration_Gain_Error() const{return gain_error;}
};


#endif /* Calibration_h */