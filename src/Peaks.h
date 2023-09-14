//
//  Created by Maria on 27.09.2022 kolupanova@inr.ru
//

#ifndef Peaks_h
#define Peaks_h
#include <stdio.h>
#include <string>
#include <TTree.h>

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


struct Tree{
public:
    Tree();
    ~Tree() = default;
    TTree* Get_Tree(){ return tree;}
    void Add_Element(){tree->Fill();}
    unsigned int _crate, _feb, _slot, _channel;
    double _gain,_mean,_median;
    unsigned int _x,_y,_z, _position;
    unsigned int _num_histogram,_max_histogram;
    std::string _pcbPosition;
private:
    TTree* tree = new TTree("tree","ttree");
};



#endif /* Peaks_h */
