//
// Created by amefodev on 27.05.2024.
//

#ifndef UNPACKING_TRACK_H
#define UNPACKING_TRACK_H

#include <TH2.h>
#include "SFGD_defines.h"
#include "Hit.h"

namespace AttLengthCalculation {
    struct AllEvents {
        TH2F* event_XY = new TH2F("Event_XY", "",Y_SIZE, 0, Y_SIZE, X_SIZE, 0, X_SIZE);
        TH2F* event_YZ = new TH2F("Event_YZ", "",Z_SIZE, 0, Z_SIZE,Y_SIZE, 0, Y_SIZE);
        TH2F* event_XZ = new TH2F("Event_XZ", "",Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);
        TH2F* event_XY_digit = new TH2F("Event_XY_digit", "",Y_SIZE, 0, Y_SIZE, X_SIZE, 0, X_SIZE);
        TH2F* event_YZ_digit = new TH2F("Event_YZ_digit", "",Z_SIZE, 0, Z_SIZE,Y_SIZE, 0, Y_SIZE);
        TH2F* event_XZ_digit = new TH2F("Event_XZ_digit", "",Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);
    };

    struct AttenuationPlots{
        AttenuationPlots();
        TH2D* AttenuationLength = new TH2D("AttenuationLength","AttenuationLength",  200,0,200, 500,0,100);
        TH2D* AttenuationLengthUpstream = new TH2D("AttenuationLengthUpstream","AttenuationLengthUpstream",  200,0,200, 500,0,100);
        TH2D* AttenuationLengthSide = new TH2D("AttenuationLengthSide","AttenuationLengthSide",  200,0,200, 500,0,100);
        TH2D* AttSideEachFiber[Y_SIZE*Z_SIZE];
    };

    struct HorizontalPosition{
        HorizontalPosition();
        std::vector<double> position_;
        std::vector<double> max_LY_;
        std::vector<double> norm_LY_;
    };

    struct VerticalPosition{
        std::vector<double> positionX_;
        std::vector<double> positionZ_;
        std::vector<double> positionY_;
        std::vector<double> max_LY_;
    };

    struct Track{
        Track(std::vector<Hit>& selectedTrack);
        void CalculateAngle(const HorizontalPosition& side, double& angle);
        void CalculateAngles();
        void CalculateNormalization();

        HorizontalPosition _hSide;
        HorizontalPosition _hUpstream;

        double _sin_x_y{NAN};
        double _sin_x_z{NAN};
        double _sin_y_z{NAN};
    };
}



#endif //UNPACKING_TRACK_H
