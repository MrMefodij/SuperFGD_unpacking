//
//  Created by Maria on 27.09.2022.
//
//  Calibration builder
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "Calibration.h"
#include <math.h>
using namespace std;



void Calibration::Gain_Calculation(){
    gain = 0;
    gain_error = 0;
    if(peaks.size() > 2){
        gain_error+=pow(peaks[1].GetPositionError(),2);
        for(auto p=2;p<peaks.size();p++)
        {
            gain+=(peaks[p].GetPosition()-peaks[p-1].GetPosition());
            gain_error += pow(peaks[p].GetPositionError(),2);
        }
        gain  /= (int)(peaks.size()-2);
        gain_error= sqrt(gain_error/(peaks.size()-1));
    }
}

TH1F* Calibration::SFGD_Calibration(TH1F * &hFEBCH, std::string connection){
    peaks.clear();
    Int_t npeaks = 30;
    TSpectrum *s = new TSpectrum(2*npeaks);
    Int_t nfound = s->Search(hFEBCH,2,"",0.001);
    Double_t *xpeaks = s->GetPositionX();
    // Double_t par[3*nfound+4];
    if(nfound > 0){
        for (auto p=0;p<min(nfound,6);p++) {
            //
            Double_t peakWidth = 10;
            if(p==0 || (p > 0 && (xpeaks[p] - xpeaks[p-1]) > 0)){
                TF1 * fit_1 = new TF1("fit_1","gaus", xpeaks[p] - peakWidth, xpeaks[p] + peakWidth);
                hFEBCH->Fit("fit_1","qr+");
              // cout << connection<<" "<<hFEBCH->GetBinContent(fit_1->GetParameter(1))<<" "<<endl;
                if((peaks.empty()  ||  peaks.back().GetPosition() < fit_1->GetParameter(1)) &&  
                    hFEBCH->GetBinContent(fit_1->GetParameter(1)) > 10){
                    Peaks peak = {fit_1->GetParameter(1),fit_1->GetParError(1),hFEBCH->GetBinContent(fit_1->GetParameter(1)),fit_1->GetParameter(2)};
                    peaks.push_back(peak);
                }

            }
        }
    }
    Gain_Calculation();
    gain_values.insert({connection,gain});
    hFEBCH->GetYaxis()->SetTitle("Number of events");
    hFEBCH->GetXaxis()->SetTitle("HG ADC channels");
    return hFEBCH;
}

TLegend* Calibration::Calibration_Legend(){
    auto* legend = new TLegend(0.5,0.6,0.9,0.9);
    std::string header = "peaks found: " + to_string(peaks.size());
    legend->SetHeader(header.c_str());
    legend -> SetFillColor(0);
    for(auto i = 0; i < peaks.size();i++){
        std::string name = "mean peak " + to_string(i) /*+ " p. e."*/+" = " + 
            to_string(peaks[i].GetPosition()) + " +/- " + 
            to_string(peaks[i].GetPositionError());
        legend->AddEntry((TObject*)0, name.c_str(), "");
    }
    if(peaks.size() > 2){
    std::string gain = "gain = " + to_string(Calibration_Gain())
        + " +/- " + to_string(Calibration_Gain_Error());
    legend->AddEntry((TObject*)0, gain.c_str(), "");
    }
    return legend;
}



