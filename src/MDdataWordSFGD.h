/* This file is part of BabyMINDdaq software package. This software
 * package is designed for internal use for the Baby MIND detector
 * collaboration and is tailored for this use primarily.
 *
 * BabyMINDdaq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BabyMINDdaq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BabyMINDdaq.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MDDATWORD_SFGD_H
#define __MDDATWORD_SFGD_H

#include "MDdataWord.h"
// #include "MDTypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

class MDdataWordSFGD : public MDdataWord {

public:
    MDdataWordSFGD(void *d=0) : MDdataWord(d) {}
    virtual ~MDdataWordSFGD() {}

    void Dump();

    uint32_t GetDataType();
    uint32_t GetBoardId();
    uint32_t GetGateHeaderID();
    uint32_t GetGateType();
    uint32_t GetGateNumber();
    uint32_t GetGateTimeFrGts();
    uint32_t GetGateTime();

    uint32_t GetHoldTimeID();
    uint32_t GetHoldTimeStartFrGTS();
    uint32_t GetHoldTimeStopFrGTS();

    uint32_t GetGtsTag();
    uint32_t GetGtsTagShort();

    uint32_t GetChannelId();
    uint32_t GetHitId();
    uint32_t GetTagId();
    uint32_t GetEdgeId();
    uint32_t GetHitTime();
    uint32_t GetAmplitudeId();
    uint32_t GetAmplitude();

    uint32_t GetGtsDataFlag();
    uint32_t GetGtsTime();

    uint32_t GetSpecialWord();

    enum DWSFGDDataType {
        GateHeader      = 0x0,
        GTSHeader       = 0x1,
        TimeMeas        = 0x2,
        ChargeMeas      = 0x3,
        GTSTrailer1     = 0x4,
        GTSTrailer2     = 0x5,
        GateTrailer     = 0x6,
        GateTime        = 0x7,
        HoldTime        = 0xB,
   
        SpecialWord     = 0xF
    };

    enum DWSFGDAmplitudeId {
        Amplitude_HG = 2,
        Amplitude_LG = 3
    };

private:

    enum DWSFGDMask {
        DataTypeMask        = 0xF0000000,
        BoardIdMask         = 0x0FF00000,
        GateHeaderIDMask    = 0x00080000,
        GateTypeMask        = 0x00070000,
        GateNumberMask      = 0x0000FFFF,
        GateTimeFrGtsMask   = 0x000007FF,
        GateTimeMask        = 0x0FFFFFFF,

        HoldTimeIDMask      = 0x00080000,
        HoldTimeGTSMask     = 0x000007FF,

        GtsTagMask          = 0x0FFFFFFF,
        GtsTagShortMask     = 0x00000003,

        ChannelIdMask       = 0x0FF00000,
        HitIdMask           = 0x000E0000,
        TagIdMask           = 0x00018000,
        EdgeIdMask          = 0x00004000,
        HitTimeMask         = 0x00001FFF,

        AmplitudeIdMask     = 0x00007000,
        AmplitudeMask       = 0x00000FFF,

        GtsDataFlagMask     = 0x08000000,
        GtsTimeMask         = 0x000FFFFF,
   
        SpecialIDParamMask  = 0x000FFFFF
    };

    enum DWSFGDShift {
        DataTypeShift       = 28,
        BoardIdShift        = 20,
        GateHeaderIDShift   = 19,
        GateTypeShift       = 16,
        GateNumberShift     = 0,
        GateTimeFrGtsShift  = 0,
        GateTimeShift       = 0,

        HoldTimeIDShift     = 19,
        HoldTimeGTSShift    = 0,

        GtsTagShift         = 0,
        GtsTagShortShift    = 0,

        ChannelIdShift      = 20,
        HitIdShift          = 17,
        TagIdShift          = 15,
        EdgeIdShift         = 14,
        HitTimeShift        = 0,

        AmplitudeIdShift    = 12,
        AmplitudeShift      = 0,

        GtsDataFlagShift    = 27,
        GtsTimeShift        = 0,

        SpecialIDParamShift  = 0,
  };
};

ostream &operator<<(ostream &s, MDdataWordSFGD &dw);

#endif

