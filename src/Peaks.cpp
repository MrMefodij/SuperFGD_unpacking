//
//  Created by Maria on 27.09.2022 kolupanova@inr.ru
//

#include "Peaks.h"
// Peaks
Peaks::Peaks(double _position,double _position_error, double _height, double _std_deviation,
             double _chisquare, int _ndf) :
        position(_position), position_error(_position_error), height(_height), std_deviation(_std_deviation),
        chisquare(_chisquare), ndf(_ndf){}

Tree::Tree() {

    tree->Branch("crate", &_crate, "crate/i");
    tree->Branch("feb", &_feb, "feb/i");
    tree->Branch("slot", &_slot, "slot/i");
    tree->Branch("channel", &_channel, "channel/i");
    tree->Branch("gain", &_gain, "gain/D");
    tree->Branch("mean", &_mean, "mean/D");
    tree->Branch("median", &_median, "median/D");
    tree->Branch("x", &_x, "x/i");
    tree->Branch("y", &_y, "y/i");
    tree->Branch("z", &_z, "z/i");
    tree->Branch("position", &_position, "position/i");
    tree->Branch("pcbPosition", &_pcbPosition);
}
