//
//  Calibration.h
//  SuperFGD
//
//  Created by Maria on 27.09.2022.
//

#ifndef Calibration_h
#define Calibration_h
#include "Peaks.h"

#include <vector>
class Calibration /*: public Peaks*/ {
public:
    Calibration(){}
    void AddPeaks(Peaks new_peak);
private:
    vector<Peaks> vPeaks;
};

#endif /* Calibration_h */


