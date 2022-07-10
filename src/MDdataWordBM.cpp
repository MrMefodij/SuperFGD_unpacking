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

#include "MDdataWordBM.h"

using namespace std;

uint32_t MDdataWordBM::GetDataType() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & DataTypeMask ) >> DataTypeShift );
  return 0;
}

uint32_t MDdataWordBM::GetBoardId() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & BoardIdMask ) >> BoardIdShift );
  return 0;
}

uint32_t MDdataWordBM::GetGateHeaderID(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & GateHeaderIDMask ) >> GateHeaderIDShift );
}

uint32_t MDdataWordBM::GetGateType() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & GateTypeMask ) >> GateTypeShift );
    return 0;
}

uint32_t MDdataWordBM::GetGateNumber() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & GateNumberMask ) >> GateNumberShift );
  return 0;
}

uint32_t MDdataWordBM::GetGateTimeFrGts() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & GateTimeFrGtsMask ) >> GateTimeFrGtsShift );
    return 0;
}

uint32_t MDdataWordBM::GetGateTime() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & GateTimeMask ) >> GateTimeShift );
  return 0;
}

uint32_t MDdataWordBM::GetHoldTimeID(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & HoldTimeIDMask ) >> HoldTimeIDShift );
    return 0;
}

uint32_t MDdataWordBM::GetHoldTimeStartFrGTS(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & HoldTimeGTSMask ) >> HoldTimeGTSShift );
    return 0;
}

uint32_t MDdataWordBM::GetHoldTimeStopFrGTS(){
    if (IsValid())  return ( (*(uint32_t*)(_data) & HoldTimeGTSMask ) >> HoldTimeGTSShift );
    return 0;
}

uint32_t MDdataWordBM::GetChannelId() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & ChannelIdMask ) >> ChannelIdShift );
  return 0;
}

uint32_t MDdataWordBM::GetHitId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & HitIdMask ) >> HitIdShift );
    return 0;
}

uint32_t MDdataWordBM::GetTagId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & TagIdMask ) >> TagIdShift );
    return 0;
}

uint32_t MDdataWordBM::GetEdgeId() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & EdgeIdMask ) >> EdgeIdShift );
    return 0;
}

uint32_t MDdataWordBM::GetHitTime() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & HitTimeMask ) >> HitTimeShift );
  return 0;
}

//here

uint32_t MDdataWordBM::GetHitCount() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & HitCountMask ) >> HitCountShift );
  return 0;
}

uint32_t MDdataWordBM::GetTriggerTime() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & TriggerTimeMask ) >> TriggerTimeShift );
  return 0;
}

uint32_t MDdataWordBM::GetTriggerTag() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & TriggerTagMask ) >> TriggerTagShift );
  return 0;
}

uint32_t MDdataWordBM::GetTriggerTagShort() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & TrTagShortMask ) >> TriggerTagShift );
  return 0;
}

uint32_t MDdataWordBM::GetAmplitudeId() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & AmplitudeIdMask ) >> AmplitudeIdShift );
  return 0;
}

uint32_t MDdataWordBM::GetAmplitude() {
  if (IsValid())  return ( (*(uint32_t*)(_data) & AmplitudeMask ) >> AmplitudeShift );
  return 0;
}

uint32_t MDdataWordBM::GetSpecialWord() {
    if (IsValid())  return ( (*(uint32_t*)(_data) & SpecialIDParamMask ) >> SpecialIDParamShift );
    return 0;
}


void MDdataWordBM::Dump() {
  cout << *this;
}

ostream & operator<<(ostream &s, MDdataWordBM &dw) {
  uint32_t dt= dw.GetDataType();
  s << " BM FEB ";
  switch (dt) {
  case MDdataWordBM::SpillHeader:
    s << "Spill Header  BoardId: " << dw.GetBoardId();
    if (dw.GetSpillHeadId()==0){
      s << "  Sid: " << dw.GetSid()
      << "  Spill Tag: " << dw.GetSpillTag();}
    else{
      s << "  Sid: " << dw.GetSid() <<
      "  DAQ Type: "<< dw.GetSpillDAQType() <<
      "  Spill time from GTrig: " << dw.GetSpillTimeGTRIG();
    }
    break;

  case MDdataWordBM::TrigHeader:
    s << "Trigger Header  Gl. Trigger Tag: " << dw.GetTriggerTag()
      << " (" << dw.GetTriggerTagShort() << ")";
    break;

  case MDdataWordBM::TimeMeas:
    s << "Channel: " << dw.GetChannelId()
      << "  HitId: " << dw.GetHitId()
      << "  TagId: " << dw.GetTagId();
    if ( dw.GetEdgeId()==0 ) s << "  Time (0, RE): ";
    else s << "  Time (1, FE): ";

    s << dw.GetHitTime();
    break;

  case MDdataWordBM::ChargeMeas:
    s << "Channel: " << dw.GetChannelId()
      << "  HitId: " << dw.GetHitId()
      << "  TagId: " << dw.GetTagId()
      << "  Amplitude Id: " << dw.GetAmplitudeId()
      << "  Charge: " << dw.GetAmplitude();
    break;

  case MDdataWordBM::TrigTrailer1:
    s << "Trigger Trailer (1)  Gl. Trigger Tag: " << dw.GetTriggerTag();
    break;

  case MDdataWordBM::TrigTrailer2:
    s << "Trigger Trailer (2)  Hit count: " << dw.GetHitCount()
      << "  Trigger Time: " << dw.GetTriggerTime();
    break;

  case MDdataWordBM::SpillTrailer1:
    s << "Spill Trailer (1)  BoardId: " << dw.GetBoardId()
      << "  Sid: " << dw.GetSid();
    if (dw.GetHeadTrailId()==0)
      s << "  Spill Tag: " << dw.GetSpillTag();
    else
      s << "  Temperature: " << dw.GetTemperature() << "  Humidity: " << dw.GetHumidity();

    break;

  case MDdataWordBM::SpillTrailer2:
    s << "Spill TimeID Spill time: " << dw.GetSpillTime();
    break;
    
  case MDdataWordBM::SpecialWord:
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
