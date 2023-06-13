//  Created by Maria on 27.09.2022.
//

#ifndef Peaks_h
#define Peaks_h
#include <stdio.h>
#include <string>


class Peaks{
public:
    double GetPosition() const {return _position;}
    double GetPositionError() const {return _position_error;}
    int GetHeight() const {return _height;}
    // double GetWidthRhs() const {return _width_rhs;}
    // double GetWidthLhs() const {return _width_lhs;}
    double GetStdDeviation() const {return _std_deviation;}
    Peaks(double position,double position_error,int height, double std_deviation);
    Peaks(){}
private:
    double _position;
    double _position_error;
    int _height; // Number of events
    double _std_deviation;
};



#endif /* Peaks_h */
