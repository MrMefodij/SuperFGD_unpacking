//
// Created by amefodev on 09.06.2023.
//

#include "ToaEventDummy.h"

ToaEventDummy::ToaEventDummy() :
        _ocbEventNumber (0), _boardId(0), _gateType(0), _gateNumber(0),
        _gateTime(0), _gateTimeFrGts(0), _gateTrailTime(0){}

ToaEventDummy::ToaEventDummy(unsigned int eventNumber, unsigned int boardId, unsigned int gateType, unsigned int gateNumber,
                             unsigned int gateTime, unsigned int gateTimeFrGts, unsigned int gateTrailTime ) :
        _ocbEventNumber (eventNumber), _boardId(boardId), _gateType(gateType), _gateNumber(gateNumber),
        _gateTime(gateTime), _gateTimeFrGts(gateTimeFrGts), _gateTrailTime(gateTrailTime) {}

void ToaEventDummy::SetOaEvent(TSFGDigit event){
    _eventsVector.push_back(std::move(event));
}

unsigned int ToaEventDummy::GetNumberHits(){
    return _eventsVector.size();
}

std::vector<TSFGDigit> ToaEventDummy::GetHits(){
    return _eventsVector;
}