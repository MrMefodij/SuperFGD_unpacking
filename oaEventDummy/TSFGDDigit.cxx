//
// Created by amefodev on 06.06.2023.
//

#include "TSFGDigit.hxx"
TSFGDigit::TSFGDigit() :
                        _gtsTime(0),
                         _chan(0),
                         _risingEdgeTDC(0),
                         _fallingEdgeExist(0),
                         _fallingEdgeTDC(0),
                         _highGainADC(0),
                         _lowGainADC(0),
                         _matchedADCandTDC(0){}
TSFGDigit::TSFGDigit(
                     unsigned int gtsTime,
                     unsigned int chan,
                     unsigned int risingEdgeTDC,
                     bool fallingEdgeExist,
                     unsigned int fallingEdgeTDC,
                     unsigned int highGainADC,
                     unsigned int lowGainADC,
                     bool matchedAdcTdc) :
                                            _gtsTime(gtsTime),
                                            _chan(chan),
                                            _risingEdgeTDC(risingEdgeTDC),
                                            _fallingEdgeExist(fallingEdgeExist),
                                            _fallingEdgeTDC(fallingEdgeTDC),
                                            _highGainADC(highGainADC),
                                            _lowGainADC(lowGainADC),
                                            _matchedADCandTDC(matchedAdcTdc){}

int TSFGDigit::GetGTSCounter() const        { return _gtsTime;}

int TSFGDigit::GetChannelNumber() const     {return _chan;}

int TSFGDigit::GetRisingEdgeTDC() const     {return _risingEdgeTDC;}

int TSFGDigit::GetFallingEdgeTDC() const    {return _fallingEdgeTDC;}

int TSFGDigit::GetHighGainADC() const       {return _highGainADC;}

int TSFGDigit::GetLowGainADC() const        {return _lowGainADC;}

bool TSFGDigit::IsMatchedADCandTDC() const  {return _matchedADCandTDC;}