//
// Created by amefodev on 09.06.2023.
//

#include "ToaEventDummy.h"

ToaEventDummy::ToaEventDummy() :
        _eventNumber (0), _boardId(0), _gateNumber(0),
        _gateTime(0), _gateTimeFrGts(0), _gateTrailTime(0){}
ToaEventDummy::ToaEventDummy(unsigned int eventNumber, unsigned int boardId, unsigned int gateNumber,
                             unsigned int gateTime, unsigned int gateTimeFrGts, unsigned int gateTrailTime ) :
                             _eventNumber (eventNumber), _boardId(boardId), _gateNumber(gateNumber),
                             _gateTime(gateTime), _gateTimeFrGts(gateTimeFrGts), _gateTrailTime(gateTrailTime){}