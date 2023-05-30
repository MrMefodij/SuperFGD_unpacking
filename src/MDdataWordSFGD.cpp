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

#include "MDdataWordSFGD.h"

using namespace std;

uint32_t MDdataWordSFGD::GetDataType() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & DataTypeMask ) >> DataTypeShift );
  return 0;
}

uint32_t MDdataWordSFGD::GetBoardId() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & BoardIdMask ) >> BoardIdShift );
  return 0;
}

uint32_t MDdataWordSFGD::GetGateHeaderID(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & GateHeaderIDMask ) >> GateHeaderIDShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetGateType() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & GateTypeMask ) >> GateTypeShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetGateNumber() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & GateNumberMask ) >> GateNumberShift );
  return 0;
}

uint32_t MDdataWordSFGD::GetGateTimeFrGts() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & GateTimeFrGtsMask ) >> GateTimeFrGtsShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetGateTime() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & GateTimeMask ) >> GateTimeShift );
  return 0;
}

uint32_t MDdataWordSFGD::GetHoldTimeID(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & HoldTimeIDMask ) >> HoldTimeIDShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetHoldTimeStartFrGTS(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & HoldTimeGTSMask ) >> HoldTimeGTSShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetHoldTimeStopFrGTS(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & HoldTimeGTSMask ) >> HoldTimeGTSShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetGtsTag(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & GtsTagMask ) >> GtsTagShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetGtsTagShort(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & GtsTagShortMask ) >> GtsTagShortShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetChannelId() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & ChannelIdMask ) >> ChannelIdShift );
  return 0;
}

uint32_t MDdataWordSFGD::GetHitId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & HitIdMask ) >> HitIdShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetTagId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & TagIdMask ) >> TagIdShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetEdgeId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & EdgeIdMask ) >> EdgeIdShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetHitTime() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & HitTimeMask ) >> HitTimeShift );
  return 0;
}

uint32_t MDdataWordSFGD::GetAmplitudeId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & AmplitudeIdMask ) >> AmplitudeIdShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetAmplitude() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & AmplitudeMask ) >> AmplitudeShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetGtsDataFlag() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & GtsDataFlagMask ) >> GtsDataFlagShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetGtsTime() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & GtsTimeMask ) >> GtsTimeShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetOcbGateType(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbGateType ) >> OcbGateTypeShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetOcbGateTag(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbGateTag ) >> OcbGateTagShift );
    return 0;
}
uint32_t MDdataWordSFGD::GetOcbEventNumber(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbEventNumber ) >> OcbEventNumberShift );
    return 0;
}
uint32_t MDdataWordSFGD::GetOcbGateOpenTimeout(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbGateOpenTimeout ) >> OcbGateOpenTimeoutShift );
    return 0;
}
uint32_t MDdataWordSFGD::GetOcbGateCloseError(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbGateCloseError ) >> OcbGateCloseErrorShift );
    return 0;
}
uint32_t MDdataWordSFGD::GetOcbFebDataErrNum(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbFebDataErrNum ) >> OcbFebDataErrNumShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetFebGateFifo0Full(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbFebGateFifo0Full ) >> OcbFebGateFifo0Shift );
    return 0;
}

uint32_t MDdataWordSFGD::GetFebGateFifo1Full(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbFebGateFifo1Full ) >> OcbFebGateFifo1Shift );
    return 0;
}

uint32_t MDdataWordSFGD::GetFebGateErrNum(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & OcbFebGateErrNum ) >> OcbFebGateErrNumShift );
    return 0;
}

uint32_t MDdataWordSFGD::GetSpecialWord() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & SpecialIDParamMask ) >> SpecialIDParamShift );
    return 0;
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
