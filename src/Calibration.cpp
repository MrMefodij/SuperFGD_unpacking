//
//  Calibration.cpp
//  SuperFGD
//
//  Created by Maria on 27.09.2022.
//
// Конструктор Calibration
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "Calibration.h"
#include <math.h>
using namespace std;


void Calibration::Gain_calculstion(const vector<Peaks> & peaks){
    Double_t distance = 0;
    Double_t distance_error = 0;
    for(auto p=2;p<peaks.size();p++)
    {
        distance+=(peaks[p].GetPosition()-peaks[p-1].GetPosition());
        distance_error += pow(peaks[p].GetPositionError()/peaks[p].GetPosition(),2);
    }
    hg = distance / (int)(peaks.size()-2);
    hg_error= sqrt(distance_error);
}

TH1F* Calibration::SFGD_Calibration(TH1F * &h){
    Int_t npeaks = 30;
    TH1F* hFEBCH = h;
    TSpectrum *s = new TSpectrum(2*npeaks);
    Int_t nfound = s->Search(hFEBCH,2,"",0.001);
    Double_t par[3*nfound+4];
    if(nfound > 0){
        TF1 *fline = new TF1("fline","pol1",0,1000);
        hFEBCH->Fit("fline","qn");
        par[0] = fline->GetParameter(0);
        par[1] = fline->GetParameter(1);
        Double_t *xpeaks = s->GetPositionX();
        for (auto p=0;p<nfound;p++) {
            Double_t xp = xpeaks[p];
            Int_t bin = hFEBCH->GetXaxis()->FindBin(xp);
            Double_t yp = hFEBCH->GetBinContent(bin);
            if (yp-TMath::Sqrt(yp) < fline->Eval(xp)) continue;
                par[3*p+2] = yp; // "height"
                par[3*p+3] = xp; // "mean"
                par[3*p+4] = 2; // "sigma"
            #if defined(__PEAKS_C_FIT_AREAS__)
                par[3*p+2] *= par[3*p+4] * (TMath::Sqrt(TMath::TwoPi())); // "area"
            #endif /* defined(__PEAKS_C_FIT_AREAS__) */
        }

        // We may have more parameters
        TVirtualFitter::Fitter(hFEBCH,10+3*nfound);
        for (auto p=0;p<min(nfound,6);p++) {
            //the fitting takes place at the interval of 3 sigma
            if(p==0 || (p > 0 && (par[3*p+3] - par[3*(p-1)+3]) > 0)){
                TF1 * fit_1 = new TF1("fit_1","gaus",par[3*p+3]-4*par[3*p+4],par[3*p+3]+4*par[3*p+4]);
                hFEBCH->Fit("fit_1","qr+");
                Peaks peak = {fit_1->GetParameter(1),fit_1->GetParError(1),(int)(par[3*p+2]),fit_1->GetParameter(0)};
                peaks.push_back(peak);
            }
        }
    }
    Gain_calculstion(peaks);
    return hFEBCH;
}

