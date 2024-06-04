//
//  Created by Maria on 27.09.2022 kolupanova@inr.ru
//
#include "Calibration.h"
#include <algorithm>
#include <iomanip>
#define MIN_HEIGHT 30


Calibration::Calibration()
{
    fTSpectrum = new TSpectrum(60);
}

Calibration::~Calibration()
{
    delete fTSpectrum;
}


void Calibration::Gain_Calculation(TGraphErrors* gr,std::string connection){
    _gain = 0;
    _gain_error= 0;
    if(_peaks.size() > 2){
        _mean_gain_value = (_peaks.back().GetPosition()-_peaks[0].GetPosition())/(_peaks.size()-1);
        for (int i = 0; i < _peaks.size(); i++) {
            gr->AddPoint(i + 2, _peaks[i].GetPosition());
            gr->SetPointError(i , 0, _peaks[i].GetPositionError());
        }
        gr->SetTitle(connection.c_str());
        gr->GetXaxis()->SetTitle("Peak number");
        gr->GetYaxis()->SetTitle("ADC channels");
        TF1 *f1 = new TF1("Linear fit", "[0]*x+[1]");
        f1->SetLineColor(kRed);
        f1->SetLineStyle(2);
        f1->SetLineWidth(4);
        gr->Fit(f1, "qp");
        _gain = f1->GetParameter(0);
        _gain_error = f1->GetParError(0);
        _legend_for_peaks = new TLegend(0.12,0.7,0.5,0.89);
        _legend_for_peaks->SetBorderSize(0);
        _legend_for_peaks->AddEntry(f1,"Linear fit","fl");
        _legend_for_peaks->AddEntry(gr,"Peak position with error bars","lep");
        os.str("");
        os << std::fixed << std::setprecision(1)
           << "gain =  "<<  _gain
           <<  " +/- " << _gain_error;
        _legend_for_peaks->AddEntry((TObject*)0, os.str().c_str(), "");
        os.str("");
        os << "#chi^{2} / ndf = " << f1->GetChisquare()<<" / "<<f1->GetNDF();
        _legend_for_peaks->AddEntry((TObject*)0, os.str().c_str(), "");
        delete f1;
    }
    _gain_values.insert({connection, {_gain,_gain_error}});
}

void Calibration::SFGD_Calibration(TH1F * &hFEBCH, std::string connection){
    _peaks.clear();
    std::vector<Peaks>().swap(_peaks); // memory release
    int nfound = fTSpectrum->Search(hFEBCH,2,"",0.005);
    if(nfound > 0) {
        double *xpeaks = fTSpectrum->GetPositionX();
        start_peak = 1;
        std::sort(xpeaks, xpeaks + nfound);
        if (xpeaks[0] < 10 && hFEBCH->GetBinContent(xpeaks[0]) > 100) {
            ++start_peak;
        }
        while (start_peak < nfound - 1 && (hFEBCH->GetBinContent(xpeaks[start_peak]) < MIN_HEIGHT ||
                                                        hFEBCH->GetBinContent(xpeaks[start_peak] + 1) < MIN_HEIGHT))
            ++start_peak;

        if (nfound > start_peak + 1/* && hFEBCH->GetMean() < 300 */ ) {
            Double_t peakWidth = 7;
            for (auto p = start_peak; p < nfound; p++) {
                if (hFEBCH->GetBinContent(xpeaks[p]) > MIN_HEIGHT) {
                    TF1 *fit_1 = new TF1("fit_1", "gaus", xpeaks[p] - peakWidth, xpeaks[p] + peakWidth);
                    hFEBCH->Fit("fit_1", "qr+");
                    if (fit_1->GetParError(1) < 3.5
                        && fit_1->GetParameter(2) < 45) {

                        Peaks peak = {fit_1->GetParameter(1), fit_1->GetParError(1),
                                      hFEBCH->GetBinContent(fit_1->GetParameter(1)), fit_1->GetParameter(2),
                                      fit_1->GetChisquare(), fit_1->GetNDF()};

                        _peaks.push_back(peak);
                    }
                    delete fit_1;
                }
            }
            if (_peaks.size() > 1) {

                for (int i = 0; i < _peaks.size() - 1; i++) {
                    if (_peaks[i].GetHeight() < 0.4 * _peaks[i + 1].GetHeight() ||
                        _peaks[i + 1].GetPosition() - _peaks[i].GetPosition() < 10) {
                        _peaks.erase(std::next(_peaks.begin(), i));
                        i--;
                    }
                }
                if (_peaks.size() > 5)
                    _peaks.erase(_peaks.begin() + 5, _peaks.end());
            }
        }
    }
    hFEBCH->GetYaxis()->SetTitle("Number of events");
    hFEBCH->GetXaxis()->SetTitle("ADC channels");
    if(hFEBCH->GetEntries() > 0) {
        double xq[1]={0.5}, yq[1];
        hFEBCH->GetQuantiles(1,yq,xq);
        _median = yq[0];
        _mean = hFEBCH->GetMean();
    }else{
        _mean=_median=0.;
    }
    _max_height = hFEBCH->GetMaximum()*1.05;
}


TLegend* Calibration::Calibration_Legend(){
    auto* legend = new TLegend(0.5,0.6,0.9,0.9);
    std::string header = "peaks found: " + std::to_string(_peaks.size());
    legend->SetHeader(header.c_str());
    legend -> SetFillColor(0);
    if(_peaks.size() > 0) {
        for (auto i = 0; i < _peaks.size(); i++) {
            os.str("");
            os << "mean peak " << i + 2 /*+ " p. e."*/ << " = "
               << std::fixed << std::setprecision(1) << _peaks[i].GetPosition()
               << " +/- " << _peaks[i].GetPositionError()
               << ", #chi^{2} / ndf = " << _peaks[i].GetChisquare() / _peaks[i].GetNDF();
            legend->AddEntry((TObject *) 0, os.str().c_str(), "");
        }
        if (_peaks.size() > 2) {
            os.str("");
            os << "gain = " << _gain
               << " +/- " << _gain_error
               << ", mean distance = " << _mean_gain_value;
            legend->AddEntry((TObject *) 0, os.str().c_str(), "");
        }
        os.str("");
        os << "mean = " << _mean << "   median = " << _median;
        legend->AddEntry((TObject *) 0, os.str().c_str(), "");
    }
    return legend;
}

TLine* Calibration::Calibration_Line_Mean(){
    auto l_mean = new TLine(_mean,0,_mean,_max_height);
    l_mean->SetLineColor(kBlue);
    l_mean->SetLineStyle(2);
    l_mean->SetLineWidth(2);
    return l_mean;
}

TLine* Calibration::Calibration_Line_Median(){
    auto l_median = new TLine(_median,0,_median,_max_height);
    l_median->SetLineColor(kGreen+2);
    l_median->SetLineStyle(2);
    l_median->SetLineWidth(2);
    return l_median;
}
