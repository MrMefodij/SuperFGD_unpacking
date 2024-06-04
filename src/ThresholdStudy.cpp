//
// Created by Maria on 18.07.2023 kolupanova@inr.ru
//

#include "ThresholdStudy.h"
#include "Files_Reader.h"
#define ADC_FOR_THRESHOLD_VALUE 0.25

void ThresholdStudy::FindThreshold(std::map<unsigned int, TH1F *>& DAC10b_hFEBH, const unsigned int bin_entries){
    for (auto value : DAC10b_hFEBH) {
        auto max_value = value.second->GetBinContent(value.second->GetMaximumBin());
        int nfound = _s->Search(value.second, 4, "", 0.05);
        double *xpeaks = _s->GetPositionX();
        std::sort(xpeaks, xpeaks + nfound);
        int xpeaks_num = 0;
        if (nfound > 0 && max_value > bin_entries && value.first > 185) {
            while (xpeaks_num < nfound - 1 && xpeaks[xpeaks_num + 1] - xpeaks[xpeaks_num] > 50)
                ++xpeaks_num;
            if(xpeaks[xpeaks_num] > 20 && value.second->GetBinContent(xpeaks[xpeaks_num]) > bin_entries ) {
                _threshold.push_back({value.first, (unsigned int) xpeaks[xpeaks_num]});
            }
        } else {
            std::cout << "File " << value.first << ": Number of entries in bin "
                      << value.second->GetBinContent(value.second->GetMaximumBin()) << std::endl;
        }
//        sort(_threshold.begin(), _threshold.end(), [](Threshold a, Threshold b) {
//            return a._DAC < b._DAC;
//        });
    }
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
        if ((!th_pars[p_e].empty() && i > 0 && _threshold[i - 1] != th_pars[p_e].back())) {
            ++p_e;
        }
        if (i != 0 && th_pars[p_e].size() == 0)
            th_pars[p_e].push_back(_threshold[i - 1]);
        th_pars[p_e].push_back(_threshold[i]);
        }
        if (_threshold.size() > 1) {

            double half_step = (_threshold[1]._DAC - _threshold[0]._DAC) / 2;
            size_t i = 1;
            if(th_pars.size() > 0 ) {
                while (i < th_pars.size()) {
                    if(std::next(th_pars.begin(), i - 1)->second.size() > 3) {
                        auto left_side = std::next(th_pars.begin(), i - 1)->second.back()._DAC;
                        auto right_side = std::next(th_pars.begin(), i)->second[0]._DAC;
                        _DAC.push_back(std::round(left_side + 0.25 * double(right_side - left_side)));
                    }
                    ++i;
                }
            }
            for (auto i: th_pars) {
                if (i.second.back()._DAC - i.second[0]._DAC > 2 * half_step) {
                    TF1 *f = new TF1("Linear fit", "[0]", i.second[0]._DAC - half_step,
                                     i.second.back()._DAC + half_step);
                    _g->Fit(f, "qr+");
//                    double par;
//                    f->GetParameters(&par);
//                    if (!_section.empty() && abs(_section[-1]._y - par) > 3)
//                        _section.push_back({i.second[0]._DAC, i.second.back()._DAC, par});
//                    else {
//                        if (_section.empty())
//                            _section.push_back({i.second[0]._DAC, i.second.back()._DAC, par});
//                        else {
//                            _section[-1] = {_section[-1]._x_prev, i.second.back()._DAC, par};
//                        }
//                    }
//                    _pars.push_back(par);
//                    DAC_value
                }
            }
//            double square_fit_boarder = half_step / 2;
//            if (_section.size() > 2) {
//                for (auto i = 0; i < _section.size() - 1; i++) {
//                    if(_pars.size() > i+1) {
//                        TF1 *f1 = new TF1("Square fit", "[2]*x*x + [1]*x + [0]",
//                                          _section[i]._x_next - square_fit_boarder,
//                                          _section[i + 1]._x_prev + square_fit_boarder);
//                        _g->Fit(f1, "qr+");
//                        double par[3];
//                        f1->GetParameters(&par[0]);
//                        double DAC_value = (-par[1] + sqrt(par[1] * par[1] - 4 * par[2] * (par[0] - (_section[i]._y +
//                                                                                                     ADC_FOR_THRESHOLD_VALUE *
//                                                                                                     (_pars[i + 1] -
//                                                                                                      _pars[i]))))
//                                           ) / 2 / par[2];
//                        if (DAC_value > 0) {
//                            std::cout << _DAC.size() + ADC_FOR_THRESHOLD_VALUE + 1 << ": " << round(DAC_value)
//                                      << std::endl;
//                            _DAC.push_back(round(DAC_value));
//                        }
//
//                    }
//                }
//            }
//        }
        }
    }
    catch (const std::exception &ex) {
            std::cerr << "Error: "<< ex.what() << '\n';
        }
    return _g;
}

