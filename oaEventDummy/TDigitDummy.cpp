//
// Created by amefodev on 09.06.2023.
//

#include "TDigitDummy.h"
TDigitDummy::TDigitDummy(){}

void TDigitDummy::SetGtsTag(unsigned int gtsTag){
    _gtsTag = gtsTag;
}

void TDigitDummy::SetGtsTime(unsigned int gtsTag){
    _gtsTag = gtsTag;
}

unsigned int TDigitDummy::GetGtsTag(){
    return _gtsTag;
}
unsigned int TDigitDummy::GetGtsTime(){
    return  _gtsTime;
}