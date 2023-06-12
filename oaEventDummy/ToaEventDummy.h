//
// Created by amefodev on 09.06.2023.
//

#ifndef UNPACKING_TOAEVENTDUMMY_H
#define UNPACKING_TOAEVENTDUMMY_H
#include <vector>
#include "TSFGDigit.hxx"

class ToaEventDummy {
public:
    ToaEventDummy();
    ToaEventDummy(unsigned int eventNumber, unsigned int boardId, unsigned int gateNumber,
                  unsigned int gateTime, unsigned int gateTimeFrGts, unsigned int gateTrailTime);
    void SetOaEvent();

private:
    const unsigned int _eventNumber;
    const unsigned int _boardId;
    const unsigned int _gateNumber;
    const unsigned int _gateTime;
    const unsigned int _gateTimeFrGts;
    const unsigned int _gateTrailTime;

    std::vector<TSFGDigit> _eventsVector;
};


#endif //UNPACKING_TOAEVENTDUMMY_H
