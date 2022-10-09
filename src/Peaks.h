//
//  Peaks.h
//  SuperFGD
//
//  Created by Maria on 27.09.2022.
//

#ifndef Peaks_h
#define Peaks_h
#include <stdio.h>



class Peaks{
public:
    double GetPosition() const {return position;}
    double GetPositionError() const {return position_error;}
    int GetHeight() const {return height;}
    double GetWidthRhs() const {return width_rhs;}
    double GetWidthLhs() const {return width_lhs;}
    double GetStdDeviation() const {return std_deviation;}
    Peaks(double new_position,double new_position_error,int new_height,double new_width_rhs,double new_width_lhs, double new_std_deviation);
    
private:
    double position;
    double position_error;
    int height; // Кол-во событий
    double width_rhs; // В пределе которой производилось фитирование справа
    double width_lhs; // слева
    double std_deviation;
};



#endif /* Peaks_h */
