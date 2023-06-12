//
// Created by amefodev on 06.06.2023.
//

#include "TSFGDigit.hxx"
TSFGDigit::TSFGDigit(): _chan(0),
                        _risingEdgeTDC(0),
                        _fallingEdgeTDC(0),
                        _highGainADC(0),
                        _lowGainADC(0),
                        _matchedADCandTDC(0){}
TSFGDigit::TSFGDigit(unsigned int chan,
                     unsigned int risingEdgeTDC,
                     unsigned int fallingEdgeTDC,
                     unsigned int highGainADC,
                     unsigned int lowGainADC,
                     bool matchedADCandTDC) : _chan(chan),
                                            _risingEdgeTDC(risingEdgeTDC),
                                            _fallingEdgeTDC(fallingEdgeTDC),
                                            _highGainADC(highGainADC),
                                            _lowGainADC(lowGainADC),
                                            _matchedADCandTDC(matchedADCandTDC){}