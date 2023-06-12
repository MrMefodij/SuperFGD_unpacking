//
// Created by amefodev on 09.06.2023.
//

#include "ToaEventDummy.h"

ToaEventDummy::ToaEventDummy(unsigned int eventNumber, unsigned int boardId, unsigned int gateNumber,
                             unsigned int gateTime, unsigned int gateTimeFrGts, unsigned int _gateTrailTime ) :
                             _eventNumber (eventNumber), _boardId(boardId), _gateNumber(gateNumber),
                             _gateTime(gateTime), _gateTimeFrGts(gateTimeFrGts), _gateTrailTime(_gateTrailTime){}