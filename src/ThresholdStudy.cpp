//
// Created by Maria on 18.07.2023.
//

#include "ThresholdStudy.h"
#include "Files_Reader.h"


#define ADC_FOR_THRESHOLD_VALUE 0.25

void ThresholdStudy::FindThreshold(std::vector<TH1F*>& hFEBCH, std::vector<unsigned int> DAC, unsigned int bin_entries){

    for (auto j = 0; j < hFEBCH.size(); j++) {
            auto bin = hFEBCH[j]->FindFirstBinAbove(bin_entries);
            if(bin > 0) {
                _threshold.push_back({DAC[j], (unsigned int) bin});
            }
            else{
                std::cout <<"File "<<DAC[j] <<": Number of entries in bin "<<hFEBCH[j]->GetBinContent(hFEBCH[j]->GetMaximumBin())<<std::endl;
            }

    }
    sort(_threshold.begin(), _threshold.end(), [](Threshold a, Threshold b) {
        return a._DAC < b._DAC;
    });
}

TGraph* ThresholdStudy::PrintThreshold(double tan_angle){
    unsigned int p_e = 0;
    std::map<unsigned int, std::vector<Threshold>> th_pars;
    try {
        for (int i = 0; i < _threshold.size(); i++) {
            _g->AddPoint(_threshold[i]._DAC, _threshold[i]._ADC);
            if (i != 0 && abs(double(_threshold[i]._ADC) - double(_threshold[i - 1]._ADC)) /
                          (_threshold[i]._DAC - _threshold[i - 1]._DAC) > tan_angle
                    ) {
                continue;
            }
            if (i == 0 || _threshold[i - 1] != th_pars[p_e].back()) {
                p_e++;
            }
            if (i != 0 && th_pars[p_e].size() == 0)
                th_pars[p_e].push_back(_threshold[i - 1]);
            th_pars[p_e].push_back(_threshold[i]);

        }
        double half_step = (_threshold[1]._DAC - _threshold[0]._DAC) / 2;
        for (auto i: th_pars) {
            TF1 *f = new TF1("Linear fit", "[0]", i.second[0]._DAC - half_step, i.second.back()._DAC + half_step);
            _g->Fit(f, "qr+");
            double par;
            f->GetParameters(&par);
            _section.push_back({i.second[0]._DAC, i.second.back()._DAC, par});
            _pars.push_back(par);
        }
        double square_fit_boarder = half_step / 2;
        if (_section.size() > 1) {
            for (auto i = 0; i < _section.size() - 1; i++) {
                TF1 *f1 = new TF1("Square fit", "[2]*x*x + [1]*x + [0]", _section[i]._x_next - square_fit_boarder,
                                  _section[i + 1]._x_prev + square_fit_boarder);
                _g->Fit(f1, "qr+");
                double par[3];
                f1->GetParameters(&par[0]);
                double DAC_value = (-par[1] + sqrt(par[1] * par[1] - 4 * par[2] * (par[0] - (_section[i]._y +
                                                                                        ADC_FOR_THRESHOLD_VALUE *
                                                                                             (_pars[i + 1] -
                                                                                              _pars[i]))))
                                   ) / 2 / par[2];
                if(DAC_value > 0) {
                    std::cout << _DAC.size() + THRESHOLD_VALUE << ": " << round(DAC_value) << std::endl;
                    _DAC.push_back(round(DAC_value));
                }

            }
        }
    }
    catch (const std::exception &ex) {
            std::cerr << "Error: "<< ex.what() << '\n';
        }
    return _g;
}

