//
// Created by amefodev on 09.06.2023.
//

#ifndef UNPACKING_TDIGITDUMMY_H
#define UNPACKING_TDIGITDUMMY_H


class TDigitDummy {
public:
    TDigitDummy();
    void SetGtsTag(unsigned int gtsTime);
    void SetGtsTime(unsigned int gtsTime);

    unsigned int GetGtsTag();
    unsigned int GetGtsTime();
private:
    unsigned int _gtsTime;
    unsigned int _gtsTag;
};


#endif //UNPACKING_TDIGITDUMMY_H
