//  Created by Maria on 27.09.2022.
//

#ifndef Peaks_h
#define Peaks_h
#include <stdio.h>
#include <string>


class Peaks{
public:
    Peaks(double _position,double _position_error,double _height, double _std_deviation);
    Peaks(){}
    ~Peaks() = default;
    double GetPosition() const {return position;}
    double GetPositionError() const {return position_error;}
    double GetHeight() const {return height;}
    double GetStdDeviation() const {return std_deviation;}

private:
    double position;
    double position_error;
    double height;
    double std_deviation;
};



#endif /* Peaks_h */
