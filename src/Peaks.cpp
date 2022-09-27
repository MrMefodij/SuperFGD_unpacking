//
//  Peaks.cpp
//  SuperFGD
//
//  Created by Maria on 27.09.2022.
//

#include "Peaks.h"
// Конструктор Peaks
Peaks::Peaks(double new_position,double new_position_error,int new_height, double new_width_rhs, double new_width_lhs, double new_std_deviation){
        position = new_position;
        position_error = new_position_error;
        height = new_height;
        width_rhs = new_width_rhs;
        width_lhs = new_width_lhs;
        std_deviation = new_std_deviation;
    }

