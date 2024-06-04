//  Created by Maria on 27.09.2022.
//
//
//  Created by Maria on 27.09.2022 kolupanova@inr.ru
//

#ifndef Peaks_h
#define Peaks_h
#include <stdio.h>
#include <string>


class Peaks{
public:
    Peaks(double _position,double _position_error,double _height, double _std_deviation,
          double _chisquare=0, int _ndf=0);
    Peaks(){}
    ~Peaks() = default;
    double GetPosition() const {return position;}
    double GetPositionError() const {return position_error;}
    double GetHeight() const {return height;}
    double GetStdDeviation() const {return std_deviation;}
    double GetChisquare() const {return chisquare;}
    int GetNDF() const {return ndf;}

private:
    double position;
    double position_error;
    double height;
    double std_deviation;
    double chisquare;
    int ndf;
};



#endif /* Peaks_h */
