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
//                         _matchedADCandTDC(0),
                        _ToT(0),
                        _timeFromGateStart(0){}
TSFGDigit::TSFGDigit(
                     unsigned int gtsTime,
                     unsigned int chan,
                     unsigned int risingEdgeTDC,
                     bool fallingEdgeExist,
                     unsigned int fallingEdgeTDC,
                     unsigned int highGainADC,
                     unsigned int lowGainADC,
//                     bool matchedAdcTdc,
                     unsigned int ToT,
                     unsigned int timeFromGateStart):
                                            _gtsTime(gtsTime),
                                            _chan(chan),
                                            _risingEdgeTDC(risingEdgeTDC),
                                            _fallingEdgeExist(fallingEdgeExist),
                                            _fallingEdgeTDC(fallingEdgeTDC),
                                            _highGainADC(highGainADC),
                                            _lowGainADC(lowGainADC),
//                                            _matchedADCandTDC(matchedAdcTdc)
                                            _ToT(ToT),
                                            _timeFromGateStart(timeFromGateStart)
                                            {}

void TSFGDigit::SetHighGainADC(unsigned int highGainADC){_highGainADC = highGainADC;}

void TSFGDigit::SetLowGainADC(unsigned int lowGainADC)  {_lowGainADC = lowGainADC;}

unsigned int TSFGDigit::GetGTSCounter() const           { return _gtsTime;}

unsigned int TSFGDigit::GetChannelNumber() const        {return _chan;}

unsigned int TSFGDigit::GetRisingEdgeTDC() const        {return _risingEdgeTDC;}

unsigned int TSFGDigit::GetFallingEdgeTDC() const       {return _fallingEdgeTDC;}

unsigned int TSFGDigit::GetHighGainADC() const          {return _highGainADC;}

unsigned int TSFGDigit::GetLowGainADC() const           {return _lowGainADC;}

//bool TSFGDigit::IsMatchedADCandTDC() const  {return _matchedADCandTDC;}

unsigned int TSFGDigit::GetTimeOverThreshold() const    {return _ToT;}

unsigned int TSFGDigit::GetTimeFromGateStart() const {return _timeFromGateStart;};

