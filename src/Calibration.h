
//  Created by Maria on 27.09.2022.
//

#ifndef Calibration_h
#define Calibration_h

#include <TTree.h>
#include <TFractionFitter.h>
#include <TVirtualFitter.h>
#include <TF1.h>
#include <TMath.h>
#include <TROOT.h>
#include <TSpectrum.h>
#include <TLegend.h>

#include "Files_Reader.h"
#include "Peaks.h"
#include <vector>
#include <map>
// #include <pair>

class Calibration{
public:
    Calibration() = default;
    ~Calibration() = default;
    TH1F* SFGD_Calibration(TH1F* &hFEBCH, string connection);
    TLegend* Calibration_Legend(); 
    std::map<std::string,Double_t> GetGain(){return gain_values;};
    vector<Peaks> Calibration_Par() const {return peaks;}
    Double_t Calibration_Gain() const{return gain;}
    Double_t Calibration_Gain_Error() const{return gain_error;}

private:
    vector<Peaks> peaks;
    Double_t gain;
    Double_t gain_error;
    std::map<std::string,Double_t> gain_values;
    void Gain_Calculation();
};


#endif /* Calibration_h */