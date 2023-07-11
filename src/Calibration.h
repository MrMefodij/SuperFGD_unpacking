
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
    std::map<std::string,double> GetGain(){return _gain_values;};
    std::vector<Peaks> Calibration_Par() const {return _peaks;}

private:
    std::vector<Peaks> _peaks;
    double _gain;
    double _gain_error;
    std::map<std::string,double> _gain_values;
    void Gain_Calculation();
    double Calibration_Gain() const{return _gain;}
    double Calibration_Gain_Error() const{return _gain_error;}
};


#endif /* Calibration_h */