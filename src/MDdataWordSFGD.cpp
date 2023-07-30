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

#include "MDdataWordSFGD.h"

using namespace std;

uint32_t MDdataWordSFGD::GetDataType() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & DataTypeMask ) >> DataTypeShift );
    throw MDexception("Error to get MDdataWordSFGD::GetDataType()");
}

uint32_t MDdataWordSFGD::GetBoardId() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & BoardIdMask ) >> BoardIdShift );
    throw MDexception("Error to get MDdataWordSFGD::GetBoardId()");
}

uint32_t MDdataWordSFGD::GetGateHeaderID(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & GateHeaderIDMask ) >> GateHeaderIDShift );
    throw MDexception("Error to get MDdataWordSFGD::GetGateHeaderID()");
}

uint32_t MDdataWordSFGD::GetGateType() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & GateTypeMask ) >> GateTypeShift );
    throw MDexception("Error to get MDdataWordSFGD::GetGateType()");
}

uint32_t MDdataWordSFGD::GetGateNumber() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & GateNumberMask ) >> GateNumberShift );
    throw MDexception("Error to get MDdataWordSFGD::GetGateNumber()");
}

uint32_t MDdataWordSFGD::GetGateTimeFrGts() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & GateTimeFrGtsMask ) >> GateTimeFrGtsShift );
    throw MDexception("Error to get MDdataWordSFGD::GetGateTimeFrGts()");
}

uint32_t MDdataWordSFGD::GetGateTime() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & GateTimeMask ) >> GateTimeShift );
  throw MDexception("Error to get MDdataWordSFGD::GetGateTime()");
}

uint32_t MDdataWordSFGD::GetHoldTimeID(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & HoldTimeIDMask ) >> HoldTimeIDShift );
    throw MDexception("Error to get MDdataWordSFGD::GetHoldTimeID()");
}

uint32_t MDdataWordSFGD::GetHoldTimeStartFrGTS(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & HoldTimeGTSMask ) >> HoldTimeGTSShift );
    throw MDexception("Error to get MDdataWordSFGD::GetHoldTimeStartFrGTS()");
}

uint32_t MDdataWordSFGD::GetHoldTimeStopFrGTS(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & HoldTimeGTSMask ) >> HoldTimeGTSShift );
    throw MDexception("Error to get MDdataWordSFGD::GetHoldTimeStopFrGTS()");
}

uint32_t MDdataWordSFGD::GetGtsTag(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & GtsTagMask ) >> GtsTagShift );
    throw MDexception("Error to get MDdataWordSFGD::GetGtsTag()");
}

uint32_t MDdataWordSFGD::GetGtsTagShort(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & GtsTagShortMask ) >> GtsTagShortShift );
    throw MDexception("Error to get MDdataWordSFGD::GetGtsTagShort()");
}

uint32_t MDdataWordSFGD::GetChannelId() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & ChannelIdMask ) >> ChannelIdShift );
  throw MDexception("Error to get MDdataWordSFGD::GetChannelId()");
}

uint32_t MDdataWordSFGD::GetHitId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & HitIdMask ) >> HitIdShift );
    throw MDexception("Error to get MDdataWordSFGD::GetHitId()");
}

uint32_t MDdataWordSFGD::GetTagId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & TagIdMask ) >> TagIdShift );
    throw MDexception("Error to get MDdataWordSFGD::GetTagId()");
}

uint32_t MDdataWordSFGD::GetEdgeId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & EdgeIdMask ) >> EdgeIdShift );
    throw MDexception("Error to get MDdataWordSFGD::GetEdgeId()");
}

uint32_t MDdataWordSFGD::GetHitTime() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & HitTimeMask ) >> HitTimeShift );
  throw MDexception("Error to get MDdataWordSFGD::GetHitTime()");
}

uint32_t MDdataWordSFGD::GetAmplitudeId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & AmplitudeIdMask ) >> AmplitudeIdShift );
    throw MDexception("Error to get MDdataWordSFGD::GetAmplitudeId()");
}

uint32_t MDdataWordSFGD::GetAmplitude() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & AmplitudeMask ) >> AmplitudeShift );
    throw MDexception("Error to get MDdataWordSFGD::GetAmplitude()");
}

uint32_t MDdataWordSFGD::GetGtsDataFlag() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & GtsDataFlagMask ) >> GtsDataFlagShift );
    throw MDexception("Error to get MDdataWordSFGD::GetGtsDataFlag()");
}

uint32_t MDdataWordSFGD::GetGtsTime() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & GtsTimeMask ) >> GtsTimeShift );
    throw MDexception("Error to get MDdataWordSFGD::GetGtsTime()");
}

uint32_t MDdataWordSFGD::GetOcbGateType(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbGateType ) >> OcbGateTypeShift );
    throw MDexception("Error to get MDdataWordSFGD::GetOcbGateType()");
}

uint32_t MDdataWordSFGD::GetOcbGateTag(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbGateTag ) >> OcbGateTagShift );
    throw MDexception("Error to get MDdataWordSFGD::GetOcbGateTag()");
}
uint32_t MDdataWordSFGD::GetOcbEventNumber(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbEventNumber ) >> OcbEventNumberShift );
    throw MDexception("Error to get MDdataWordSFGD::GetOcbEventNumber()");
}
uint32_t MDdataWordSFGD::GetOcbGateOpenTimeout(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbGateOpenTimeout ) >> OcbGateOpenTimeoutShift );
    throw MDexception("Error to get MDdataWordSFGD::GetOcbGateOpenTimeout()");
}
uint32_t MDdataWordSFGD::GetOcbGateCloseError(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbGateCloseError ) >> OcbGateCloseErrorShift );
    throw MDexception("Error to get MDdataWordSFGD::GetOcbGateCloseError()");
}
uint32_t MDdataWordSFGD::GetOcbFebDataErrNum(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbFebDataErrNum ) >> OcbFebDataErrNumShift );
    throw MDexception("Error to get MDdataWordSFGD::GetOcbFebDataErrNum()");
}

uint32_t MDdataWordSFGD::GetFebGateFifo0Full(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbFebGateFifo0Full ) >> OcbFebGateFifo0Shift );
    throw MDexception("Error to get MDdataWordSFGD::GetFebGateFifo0Full()");
}

uint32_t MDdataWordSFGD::GetFebGateFifo1Full(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbFebGateFifo1Full ) >> OcbFebGateFifo1Shift );
    throw MDexception("Error to get MDdataWordSFGD::GetDataType()");
}

uint32_t MDdataWordSFGD::GetFebGateErrNum(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbFebGateErrNum ) >> OcbFebGateErrNumShift );
    throw MDexception("Error to get MDdataWordSFGD::GetFebGateFifo1Full()");
}

uint32_t MDdataWordSFGD::GetSpecialWord() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & SpecialIDParamMask ) >> SpecialIDParamShift );
    throw MDexception("Error to get MDdataWordSFGD::GetSpecialWord()");
}

void MDdataWordSFGD::Dump() {
  cout << *this;
}

ostream & operator<<(ostream &s, MDdataWordSFGD &dw) {
    uint32_t dt= dw.GetDataType();
    s << " SFGD FEB ";
    switch (dt) {
        case MDdataWordSFGD:: GateHeader:
            s << "Spill Header  BoardId: " << dw.GetBoardId();
            if (dw.GetGateHeaderID()==0){
            s << " Gate type: " << dw.GetGateType() <<
            " Gate Number: " << dw.GetGateNumber();
            } else {
                s << " Gate time from GTS: " << dw.GetGateTimeFrGts();
            }
            break;

        case MDdataWordSFGD::GTSHeader:
            s << "GTS Tag: " << dw.GetGtsTag()
            << " (" << dw.GetGtsTagShort() << ")";
            break;

        case MDdataWordSFGD::TimeMeas:
            s << "Channel: " << dw.GetChannelId()
            << "  HitId: " << dw.GetHitId()
            << "  TagId: " << dw.GetTagId();
            if ( dw.GetEdgeId()==0 ) {
                s << "  Time (0, RE): ";
            } else {
                s << "  Time (1, FE): ";
            }
            s << dw.GetHitTime();
            break;

        case MDdataWordSFGD::ChargeMeas:
            s << "Channel: " << dw.GetChannelId()
            << "  HitId: " << dw.GetHitId()
            << "  TagId: " << dw.GetTagId()
            << "  Amplitude Id: " << dw.GetAmplitudeId()
            << "  Charge: " << dw.GetAmplitude();
            break;

        case MDdataWordSFGD::GTSTrailer1:
            s << "GTS Trailer (1)  Gl. GTS Tag: " << dw.GetGtsTag();
            break;

        case MDdataWordSFGD::GTSTrailer2:
            s << "GTS Trailer (2)  Data flag: " << dw.GetGtsDataFlag()
            << "  Trigger Time: " << dw.GetGtsTime();
            break;

        case MDdataWordSFGD::GateTrailer:
            s << "Gate Trailer (1)  BoardId: " << dw.GetBoardId()
            << " Gate type: " << dw.GetGateType() <<
              " Gate Number: " << dw.GetGateNumber();
            break;

        case MDdataWordSFGD::GateTime:
            s << "Spill TimeID Spill time: " << dw.GetGateTime();
            break;

        case MDdataWordSFGD::HoldTime:
            s << "Hold Time  BoardId: " << dw.GetBoardId();
            if (dw.GetHoldTimeID() == 0){
                s << " Hold time Start: "  << dw.GetHoldTimeStartFrGTS();
            } else {
                s << " Hold time Stop: "  << dw.GetHoldTimeStopFrGTS();
            }
            break;

        case MDdataWordSFGD::OcbGateHeader:
            s << "Ocb Header Gate type: " << dw.GetOcbGateType();
            s << " Gate Tag: " << dw.GetOcbGateTag() << " Event Number: " << dw.GetOcbEventNumber();
            break;

        case MDdataWordSFGD::OcbGateTrailer:
            s << "Ocb Trailer Gate time open timeout: " << dw.GetOcbGateOpenTimeout();
            s << " Gate Close error: " << dw.GetOcbGateCloseError();
            s << " FEB data packet error: " << dw.GetOcbFebDataErrNum();
            break;

        case MDdataWordSFGD::FebDataTrailer:
            s << "Feb Data Trailer Fifo 0 Full: " << dw.GetFebGateFifo0Full();
            s << " Feb Data Trailer Fifo 1 Full: " << dw.GetFebGateFifo1Full();
            s << "\nNumber of recorded errors: " << dw.GetFebGateErrNum();
            break;

        case MDdataWordSFGD::SpecialWord:
            s << "SpecialWord: ";
            if (dw.GetSpecialWord()==65536)
                s << "BoardId: " << dw.GetBoardId() << " Readout End";
            if (dw.GetSpecialWord()==1)
                s << "BoardId: " << dw.GetBoardId() << " GTrig Reset";
            if (dw.GetSpecialWord()==2)
                s << "BoardId: " << dw.GetBoardId() << " Spill Reset";
            if (dw.GetSpecialWord()==3)
                s << "BoardId: " << dw.GetBoardId() << " GTrig & Spill Reset";
            if (dw.GetSpecialWord()==16)
                s <<"BoardId: " << dw.GetBoardId() << " FIFO Full";
            if (dw.GetSpecialWord()==983295)
                    s << "BoardId: " << dw.GetBoardId() << " Spill Reset";
            if (dw.GetSpecialWord()==1048575)
            {s<< "strange "<<dw.GetSpecialWord();}
    //s<<endl;
    break;
    
  default:
    s << "Unknown data word (" << hex << *(dw.GetDataPtr()) << dec << ")\n";
    break;
  }

  return s;
}
