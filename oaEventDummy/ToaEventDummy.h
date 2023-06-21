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
    ToaEventDummy(unsigned int eventNumber, unsigned int boardId, unsigned int gateType, unsigned int gateNumber,
                  unsigned int gateTime, unsigned int gateTimeFrGts, unsigned int gateTrailTime);
    void SetOaEvent(TSFGDigit event);
    unsigned int GetNumberHits();
    std::vector<TSFGDigit> GetHits();

    unsigned int GetEventNumber() const {return _ocbEventNumber;}
    unsigned int GetBoardId() const {return _boardId;}
    unsigned int GetGateNumber() const {return _gateNumber;}
    unsigned int GetGateTime() const {return _gateTime;}
    unsigned int GetGateTimeFrGts() const {return _gateTimeFrGts;};
    unsigned int GetGateTrailTime() const {return _gateTrailTime;};
private:
    const unsigned int _ocbEventNumber;
    const unsigned int _boardId;
    const unsigned int _gateType;
    const unsigned int _gateNumber;
    const unsigned int _gateTime;
    const unsigned int _gateTimeFrGts;
    const unsigned int _gateTrailTime;

    std::vector<TSFGDigit> _eventsVector;
};

//std::ostream& operator << (std::ostream& os, const ToaEventDummy& event) {
//    os << "EventNumber: " <<event.GetEventNumber() << "  BoardId: "<< event.GetBoardId() << " Gate Number: " <<
//          event.GetGateNumber() << " GateTime: " << event.GetGateTime() << " GateTimeFromGTS: " <<
//          event.GetGateTimeFrGts() << " GAteTrailTime: " << event.GetGateTrailTime();
//    return os ;
//}

#endif //UNPACKING_TOAEVENTDUMMY_H
