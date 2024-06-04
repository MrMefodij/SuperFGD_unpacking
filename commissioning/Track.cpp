//
// Created by amefodev on 27.05.2024.
//

#include "Track.h"

namespace AttLengthCalculation {

    AttenuationPlots::AttenuationPlots(){
        AttenuationLength->GetYaxis()->SetTitle("Observed Light [p.e.]");
        AttenuationLength->GetXaxis()->SetTitle("Length [cm]");
        AttenuationLength->GetZaxis()->SetTitle("N");


    }

    HorizontalPosition::HorizontalPosition(){
        position_.resize(Y_SIZE);
        max_LY_.resize(Y_SIZE);
        norm_LY_.resize(Y_SIZE);
        std::fill(position_.begin(), position_.end(), NAN);
        std::fill(max_LY_.begin(), max_LY_.end(), NAN);
        std::fill(norm_LY_.begin(), norm_LY_.end(), NAN);
    }

    Track::Track(std::vector<Hit>& selectedTrack){
        for (int k = 0; k < selectedTrack.size(); ++k) {
            if (!std::isnan(selectedTrack.at(k)._y)) {
                double y = selectedTrack.at(k)._y;
                if (!std::isnan(selectedTrack.at(k)._z) ) {
                    if(std::isnan(_hSide.max_LY_[(int)y]) || _hSide.max_LY_[(int)y] < selectedTrack.at(k)._LY_PE){
                        _hSide.max_LY_[(int)y] = selectedTrack.at(k)._LY_PE;
                        _hSide.position_[(int)y] = selectedTrack.at(k)._z;
                    }
                } else if (!std::isnan(selectedTrack.at(k)._x)){
                    if(std::isnan(_hUpstream.max_LY_[(int)y]) || _hUpstream.max_LY_[(int)y] < selectedTrack.at(k)._LY_PE){
                        _hUpstream.max_LY_[(int)y] = selectedTrack.at(k)._LY_PE;
                        _hUpstream.position_[(int)y] = selectedTrack.at(k)._x;
                    }
                }
            }
        }
    }

    void Track::CalculateAngle(const HorizontalPosition& side, double& angle){
        bool isFirst{true};
        std::pair<double, double> lowestPosition {NAN,NAN};
        std::pair<double, double> highPosition {NAN,NAN};

        for (int y = 0; y < Y_SIZE; ++y) {
            if (!std::isnan(side.position_.at(y))){
                if (isFirst){
                    lowestPosition = {side.position_.at(y), y};
                    isFirst = false;
                }
                highPosition = {side.position_.at(y), y};
            }
        }
        double track = sqrt((highPosition.second - lowestPosition.second)*(highPosition.second - lowestPosition.second)
                            + (highPosition.first - lowestPosition.first) * (highPosition.first - lowestPosition.first));
        angle = (highPosition.second - lowestPosition.second) / track;
    }

    void Track::CalculateAngles(){
        CalculateAngle(_hSide, _sin_y_z);
        CalculateAngle(_hUpstream, _sin_x_y);
        CalculateNormalization();
    }

    void Track::CalculateNormalization(){
        for (int y = 0; y < Y_SIZE; ++y) {
            _hSide.norm_LY_[y] = _hSide.max_LY_[y] * _sin_x_y * _sin_y_z;
            _hUpstream.norm_LY_[y] = _hSide.max_LY_[y] * _sin_x_y * _sin_y_z;
        }
    }
}