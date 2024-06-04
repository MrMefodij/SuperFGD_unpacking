//
// Created by Maria on 22.03.2024 kolupanova@inr.ru
//

#ifndef UNPACKING_HIT_H
#define UNPACKING_HIT_H

#include <iostream>
#include <cmath>

struct Hit {
    bool operator<(const Hit &other) const {
        if (_timeFromGateStart == other._timeFromGateStart)
            return _global_ch_id < other._global_ch_id;
        return _timeFromGateStart < other._timeFromGateStart;
    }
    unsigned int _board_id;
    unsigned int _channel_id;
    unsigned int _global_ch_id;
    double _x{NAN};
    double _y{NAN};
    double _z{NAN};
    unsigned int _timeFromGateStart;
    unsigned int _highGainADC;
    unsigned int _lowGainADC;
    unsigned int _ToT;
    double _LY_PE{NAN};
};

#endif //UNPACKING_HIT_H
