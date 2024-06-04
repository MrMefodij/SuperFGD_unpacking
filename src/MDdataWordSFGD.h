/* This file is part of SuperFGD software package. This software
 * package is designed for internal use for the SuperFGD detector
 * collaboration and is tailored for this use primarily.
 *
 * Unpacking is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Unpacking is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SuperFGD Unpacking.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//
// Created by amefodev on 13.06.2023. mrmefodij@gmail.com
//

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
    ~MDdataWordSFGD() override = default;

    void Dump() override;

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

    uint32_t GetOcbGateType();
    uint32_t GetOcbGateTag();
    uint32_t GetOcbEventNumber();
    uint32_t GetOcbGateOpenTimeout();
    uint32_t GetOcbGateCloseError();
    uint32_t GetOcbFebDataErrNum();

    uint32_t GetFebGateFifo0Full();
    uint32_t GetFebGateFifo1Full();
    uint32_t GetFebGateErrNum();

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
        OcbGateHeader   = 0x8,
        OcbGateTrailer  = 0x9,
        HoldTime        = 0xB,
        EventDone       = 0xC,
        FebDataTrailer  = 0xD,

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
   
        SpecialIDParamMask  = 0x000FFFFF,

        OcbGateType         = 0x0E000000,
        OcbGateTag          = 0x01800000,
        OcbEventNumber      = 0x007FFFFF,

        OcbGateOpenTimeout  = 0x00008000,
        OcbGateCloseError   = 0x00004000,
        OcbFebDataErrNum    = 0x00003FFF,

        OcbFebGateFifo0Full = 0x00010000,
        OcbFebGateFifo1Full = 0x00020000,
        OcbFebGateErrNum    = 0x0000FFFF
    };

    enum DWSFGDShift {
        DataTypeShift           = 28,
        BoardIdShift            = 20,
        GateHeaderIDShift       = 19,
        GateTypeShift           = 16,
        GateNumberShift         = 0,
        GateTimeFrGtsShift      = 0,
        GateTimeShift           = 0,

        HoldTimeIDShift         = 19,
        HoldTimeGTSShift        = 0,

        GtsTagShift             = 0,
        GtsTagShortShift        = 0,

        ChannelIdShift          = 20,
        HitIdShift              = 17,
        TagIdShift              = 15,
        EdgeIdShift             = 14,
        HitTimeShift            = 0,

        AmplitudeIdShift        = 12,
        AmplitudeShift          = 0,

        GtsDataFlagShift        = 27,
        GtsTimeShift            = 0,

        SpecialIDParamShift     = 0,

        OcbGateTypeShift        = 25,
        OcbGateTagShift         = 23,
        OcbEventNumberShift     = 0,

        OcbGateOpenTimeoutShift = 15,
        OcbGateCloseErrorShift  = 14,
        OcbFebDataErrNumShift   = 0,

        OcbFebGateFifo0Shift    = 16,
        OcbFebGateFifo1Shift    = 17,
        OcbFebGateErrNumShift   = 0
  };
};

std::ostream &operator<<(std::ostream &s, MDdataWordSFGD &dw);

#endif

