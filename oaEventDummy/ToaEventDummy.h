//
// Created by amefodev on 09.06.2023.
//

#ifndef UNPACKING_TOAEVENTDUMMY_H
#define UNPACKING_TOAEVENTDUMMY_H

//#define AS_KV(x) #x<<" = "<< x

#include <vector>
#include <iostream>
#include "TSFGDigit.hxx"

class ToaEventDummy {
public:
    ToaEventDummy();
    ~ToaEventDummy() = default;
    ToaEventDummy(unsigned int eventNumber, unsigned int boardId, unsigned int gateType, unsigned int gateNumber,
                  unsigned long long int gateTime, bool gateTimeFrGtsEx, unsigned int gateTimeFrGts, unsigned int gateTrailTime);
    void SetOaEvent(TSFGDigit event);
    unsigned int GetNumberHits();
    std::vector<TSFGDigit> GetHits() const;

    void ReserveEventVectorSize(unsigned int size) {_eventsVector.reserve(size);}
    unsigned int GetEventNumber() const {return _ocbEventNumber;}
    unsigned int GetBoardId() const {return _boardId;}
    unsigned int GetGateNumber() const {return _gateNumber;}
    unsigned long long int GetGateTime() const {return _gateTime;}
    bool GetGateTimeFrGtsEx() const {return _gateTimeFrGtsEx;};
    unsigned int GetGateTimeFrGts() const {return _gateTimeFrGts;};
    unsigned int GetGateTrailTime() const {return _gateTrailTime;};
    unsigned int GetGateType() const {return _gateType;};
    TSFGDigit * GetPointerToLastEvent()   {return &_eventsVector.back();};
    void PrintToaEventDummy();
private:
    const unsigned int _ocbEventNumber;
    const unsigned int _boardId;
    const unsigned int _gateType;
    const unsigned int _gateNumber;
    const unsigned long long int _gateTime;
    bool _gateTimeFrGtsEx;
    unsigned int _gateTimeFrGts;
    const unsigned int _gateTrailTime;
    std::vector<TSFGDigit> _eventsVector;
};
//std::ostream& operator << (std::ostream& os, const ToaEventDummy* event) {
//    os << "EventNumber: " <<event->GetEventNumber() << "  BoardId: "<< event->GetBoardId() << " Gate Number: " <<
//          event->GetGateNumber() << " GateTime: " << event->GetGateTime() << " GateTimeFromGTS: " <<
//          event->GetGateTimeFrGts() << " GateTrailTime: " << event->GetGateTrailTime();
//    return os ;
//}

#endif //UNPACKING_TOAEVENTDUMMY_H
