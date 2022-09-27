//
//  Calibration.cpp
//  SuperFGD
//
//  Created by Maria on 27.09.2022.
//
// Конструктор Calibration
#include <stdio.h>

#include "Calibration.h"

void Calibration::AddPeaks(Peaks new_peak){
    vPeaks.push_back(new_peak);
}

