//
//  Created by Maria on 27.09.2022.
//
//  Calibration builder


#include "Calibration.h"
#include <math.h>

void Calibration::Gain_Calculation(){
    _gain = 0;
    _gain_error = 0;
    if(_peaks.size() > 2){
        _gain_error+=pow(_peaks[1].GetPositionError(),2);
        for(auto p=2;p<_peaks.size();p++)
        {
            _gain+=(_peaks[p].GetPosition()-_peaks[p-1].GetPosition());
            _gain_error += pow(_peaks[p].GetPositionError(),2);
        }
        _gain  /= (int)(_peaks.size()-2);
        _gain_error= sqrt(_gain_error/(_peaks.size()-1));
    }
}

TH1F* Calibration::SFGD_Calibration(TH1F * &hFEBCH, std::string connection){
    _peaks.clear();
    int npeaks = 30;
    TSpectrum *s = new TSpectrum(2*npeaks);
    int nfound = s->Search(hFEBCH,2,"",0.001);
    double *xpeaks = s->GetPositionX();
    // Double_t par[3*nfound+4];
    if(nfound > 0){
        for (auto p=0;p<std::min(nfound,6);p++) {
            //
            Double_t peakWidth = 10;
            if(p==0 || (p > 0 && (xpeaks[p] - xpeaks[p-1]) > 0)){
                TF1 * fit_1 = new TF1("fit_1","gaus", xpeaks[p] - peakWidth, xpeaks[p] + peakWidth);
                hFEBCH->Fit("fit_1","qr+");
                if((_peaks.empty()  ||  _peaks.back().GetPosition() < fit_1->GetParameter(1)) &&
                    hFEBCH->GetBinContent(fit_1->GetParameter(1)) > 10){
                    Peaks peak = {fit_1->GetParameter(1),fit_1->GetParError(1),hFEBCH->GetBinContent(fit_1->GetParameter(1)),fit_1->GetParameter(2)};
                    _peaks.push_back(peak);
                }

            }
        }
    }
    Gain_Calculation();
    _gain_values.insert({connection,_gain});
    hFEBCH->GetYaxis()->SetTitle("Number of events");
    hFEBCH->GetXaxis()->SetTitle("ADC channels");
    return hFEBCH;
}


TLegend* Calibration::Calibration_Legend(){
    auto* legend = new TLegend(0.5,0.6,0.9,0.9);
    std::string header = "peaks found: " + std::to_string(_peaks.size());
    legend->SetHeader(header.c_str());
    legend -> SetFillColor(0);
    for(auto i = 0; i < _peaks.size();i++){
        std::string name = "mean peak " + std::to_string(i) /*+ " p. e."*/+" = " +
                std::to_string(_peaks[i].GetPosition()) + " +/- " +
                std::to_string(_peaks[i].GetPositionError());
        legend->AddEntry((TObject*)0, name.c_str(), "");
    }
    if(_peaks.size() > 2){
    std::string gain = "gain = " + std::to_string(Calibration_Gain())
        + " +/- " + std::to_string(Calibration_Gain_Error());
    legend->AddEntry((TObject*)0, gain.c_str(), "");
    }
    return legend;
}



