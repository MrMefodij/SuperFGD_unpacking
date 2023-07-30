//
//  Created by Maria on 27.09.2022.
//
//  Calibration builder


#include "Calibration.h"
#include <algorithm>
void Calibration::Gain_Calculation(){
    _gain = 0;
    _gain_error = 0;
    unsigned int start_peak = 2;

    if(_peaks.size() > start_peak + 1){
        if(_peaks[1].GetPosition() - _peaks[0].GetPosition() < 1.2 * (_peaks[2].GetPosition() - _peaks[1].GetPosition()) && _peaks[1].GetPosition() - _peaks[0].GetPosition() < 1.2 * (_peaks[3].GetPosition() - _peaks[2].GetPosition()))
            start_peak = 1;
        _gain_error+=pow(_peaks[start_peak].GetPositionError(),start_peak);
        for(int p=start_peak + 1; p < _peaks.size();p++)
        {
//            if(Connection == "FEB_251_Channel_210")
//            std::cout << _peaks[p].GetPosition()<<" "<<_peaks[p].GetPosition() - _peaks[p-1].GetPosition()<<" "<<start_peak<<std::endl;
            _gain+=(_peaks[p].GetPosition()-_peaks[p-1].GetPosition());
            _gain_error += pow(_peaks[p].GetPositionError(),2);
        }
        _gain /= (int) (_peaks.size() - (start_peak + 1));
        _gain_error = sqrt(_gain_error / (_peaks.size() - start_peak));
    }
}

TH1F* Calibration::SFGD_Calibration(TH1F * &hFEBCH, std::string connection){
    _peaks.clear();
    int npeaks = 30;
    TSpectrum *s = new TSpectrum(2*npeaks);
    int nfound = s->Search(hFEBCH,2,"",0.001);
    double *xpeaks = s->GetPositionX();
    if(nfound > 0 ){
        for (auto p=0;p<std::min(nfound,15) ;p++) {
            if(hFEBCH->GetBinContent(xpeaks[p]) > 50) {
                Double_t peakWidth = 10;
                TF1 *fit_1 = new TF1("fit_1", "gaus", xpeaks[p] - peakWidth, xpeaks[p] + peakWidth);
                hFEBCH->Fit("fit_1", "qr+");
                if (fit_1->GetParError(1) < 2 && fit_1->GetParameter(2) < 25) {
                    Peaks peak = {fit_1->GetParameter(1), fit_1->GetParError(1),
                                  hFEBCH->GetBinContent(fit_1->GetParameter(1)), fit_1->GetParameter(2)};
                    _peaks.push_back(peak);
                }
            }
        }
        if(_peaks.size()> 1) {
            sort(_peaks.begin(), _peaks.end(), [](const Peaks &p_0, const Peaks &p_1) {
                return p_0.GetPosition() < p_1.GetPosition();
            });

            for (int i = 0; i < _peaks.size() - 1; i++) {
                if (_peaks[i].GetHeight() < 0.6 * _peaks[i + 1].GetHeight() || _peaks[i+1].GetPosition() - _peaks[i].GetPosition() < 20
                ) {
                    _peaks.erase(std::next(_peaks.begin(), i));
                    i--;
                }
            }
            if(_peaks.size() > 5)
            _peaks.erase(_peaks.begin()+5, _peaks.end());
//            _peaks.resize(5);
//            while(_peaks.size() > 5){
//                _peaks.pop_back();
//            }
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
    std::string gain = "gain = " + std::to_string(_gain)
        + " +/- " + std::to_string(_gain_error);
    legend->AddEntry((TObject*)0, gain.c_str(), "");
    }
    return legend;
}



