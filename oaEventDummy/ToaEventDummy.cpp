//
// Created by amefodev on 09.06.2023.
//

#include "ToaEventDummy.h"
#include "SFGD_defines.h"

ToaEventDummy::ToaEventDummy() :
        _ocbEventNumber (0), _boardId(0), _gateType(0), _gateNumber(0),
        _gateTime(0), _gateTimeFrGtsEx(false), _gateTimeFrGts(0), _gateTrailTime(0){}

ToaEventDummy::ToaEventDummy(unsigned int eventNumber, unsigned int boardId, unsigned int gateType, unsigned int gateNumber,
                             unsigned long long int gateTime, bool gateTimeFrGtsEx, unsigned int gateTimeFrGts, unsigned int gateTrailTime ) :
        _ocbEventNumber (eventNumber), _boardId(boardId), _gateType(gateType), _gateNumber(gateNumber),
        _gateTime(gateTime), _gateTimeFrGtsEx(gateTimeFrGtsEx), _gateTimeFrGts(gateTimeFrGts), _gateTrailTime(gateTrailTime) {}

void ToaEventDummy::SetOaEvent(TSFGDigit event){
    _eventsVector.push_back(std::move(event));
}

unsigned int ToaEventDummy::GetNumberHits(){
    return _eventsVector.size();
}

std::vector<TSFGDigit> ToaEventDummy::GetHits() const{
    return _eventsVector;
}

void ToaEventDummy::PrintToaEventDummy(){
    std::cout << "EventNumber: " << this->GetEventNumber() << "  BoardId: "<< this->GetBoardId() << " Gate Number: " <<
        this->GetGateNumber() << " GateTime: " << this->GetGateTime() << " GateTimeFromGTSEx: " << this->GetGateTimeFrGtsEx()
        << " GateTimeFromGTS: " << this->GetGateTimeFrGts() << " GateTrailTime: " << this->GetGateTrailTime() << std::endl;
    for ( const TSFGDigit & a : _eventsVector) {
        std::cout << AS_KV(a.GetGTSCounter()) << " " << AS_KV(a.GetChannelNumber()) << " " << AS_KV(a.GetRisingEdgeTDC()) << " "
        << AS_KV(a.GetFallingEdgeTDC()) << " " << AS_KV(a.GetHighGainADC()) << " " << AS_KV(a.GetLowGainADC()) << " " << AS_KV(a.GetTimeOverThreshold()) << std::endl;
    }
}