
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

class Calibration{
public:
    TH1F* SFGD_Calibration(TH1F* &h, std::string name);
    void Gain_Calculation(const vector<Peaks> & peaks);
    vector<Peaks> Calibration_Par() const {return peaks;}
    Double_t Calibration_Gain() const{return hg;}
    Double_t Calibration_Gain_Error() const{return hg_error;}
    TLegend* Calibration_Legend(); 
    std::map<std::string,Double_t> GetGain(){return gain;};
    TH1F* Gain_Distrubution();
    void Clear_Peaks(){peaks.clear();}
    Calibration(){}
private:
    static Double_t par[500];
    vector<Peaks> peaks;
    Double_t hg = 0;
    Double_t hg_error = 0;
    std::map<std::string,Double_t> gain;
};


#endif /* Calibration_h */