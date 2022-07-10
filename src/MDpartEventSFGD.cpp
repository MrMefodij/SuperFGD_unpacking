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

#include "MDpartEventSFGD.h"
//#include <vector>
#include <math.h> 
#include <cmath>

using namespace std;

MDpartEventSFGD::MDpartEventSFGD(void *d, unsigned int time, unsigned int tag): MDdataContainer(d),
                                                                                _gtsTag(-1), _nDataWords(0), _trigEvents(0) { _previousTrTime = time; _previousTrTag = tag;}

void MDpartEventSFGD::SetDataPtr( void *d, uint32_t aSize ) {
  MDdataContainer::SetDataPtr(d);
  this->Init();
}



void MDpartEventSFGD::Init() {
  //   cout << " Calling MDpartEventSFGD::Init() " << endl;
  this->UnValidate();

  for (int ich=0 ; ich < SFGD_FEB_NCHANNELS ; ich++) {
    _nLeadingEdgeHits[ich]=0;
    _nTrailingEdgeHits[ich]=0;
    _lgHit[ich]=false;
    _hgHit[ich]=false;
    _lgHitAmplitude[ich]=0;
    _hgHitAmplitude[ich]=0;
    _leadingEdgeHitTime[ich].clear();
    _trailingEdgeHitTime[ich].clear();
    _leadingEdgeHitId[ich].clear();
    _trailingEdgeHitId[ich].clear();
  }

  _nDataWords = 1;
    _gtsTag = -1;
    _gateHeaderNumber = -1;
  _size = 4;

  unsigned int * ptr = Get32bWordPtr(0);
  
  MDdataWordSFGD dw(ptr);
  dw.SetDataPtr(ptr);
//   cout << dw << endl;
  if ( dw.IsValid() ) {
    // Check the reliability of the header and decode the header information.
    if (dw.GetDataType() != MDdataWordSFGD::GTSHeader ) { // The data doesn't start with a header
        cout << dw<<endl;
      throw MDexception("ERROR in MDpartEventSFGD::Init() : 1st word is not a trigger header");
    } else {

        _gtsTag = dw.GetGtsTag();
        _gtsTagId = dw.GetTriggerTagShort();
      if (dw.GetGtsTag() != _previousTrTag +1 && _previousTrTag!=0)
          cout << "ERROR in MDpartEventSFGD::Init() : Trigger Tag is NOT consistent with previous Trigger Tag: " <<
               _gtsTag << " != " << _previousTrTag << "+ 1" << endl;
      
      bool done(false);
      bool done2(false);
      _spillHeaderA = false;
      
      while (!done) {
        ptr++;
        dw.SetDataPtr(ptr);
        _size += 4;
        int dataType = dw.GetDataType();
        switch (dataType) {
            
          case MDdataWordSFGD::TimeMeas :
            if (dw.GetTagId() == _gtsTagId) {
              this->AddTimeHit(dw);
              ++_nDataWords;
            } else {
              if (_trigEvents) {
                int nTr = _trigEvents->size(), lastPending;
                lastPending = (nTr > 3) ? nTr-4 : 0;
                for (int i = nTr-1; i >= lastPending; --i) {
                  if (_trigEvents->at(i)->GetTriggerTagId() == dw.GetTagId()) {
                    _trigEvents->at(i)->AddTimeHit(dw);
                    break;
                  }
                }
              }
            }
            break;

          case MDdataWordSFGD::ChargeMeas :
            if (dw.GetAmplitudeId() == 2 || dw.GetAmplitudeId() == 3) {
                if (dw.GetTagId() == _gtsTagId) {
                    this->AddAmplitudeHit(dw);
                    ++_nDataWords;
                } else {
                    if (_trigEvents) {
                        int nTr = _trigEvents->size(), lastPending;
                        lastPending = (nTr > 3) ? nTr-4 : 0;
                        for (int i = nTr-1; i >= lastPending; --i) {
                            if (_trigEvents->at(i)->GetTriggerTagId() == dw.GetTagId()) {
                                _trigEvents->at(i)->AddAmplitudeHit(dw);
                                break;
                            }
                        }
                    }
                }
            } 
            break;

          case MDdataWordSFGD::GTSTrailer1 :
            done = true;
            ++_nDataWords;
            break;
            
          case MDdataWordSFGD::GateHeader :
            if (dw.GetGateHeaderID() == 0) {
                _gateHeaderNumber = dw.GetGateNumber();
                _spillHeaderA = true;
                _gateHeaderBoardID = dw.GetBoardId();
                cout << "2: SFGD spill Header \"A\" Board ID " << _gateHeaderBoardID << " SpillTag: " << dw.GetGateNumber() << endl;
            }
            ++_nDataWords;
            break;
            
          case MDdataWordSFGD::GateTrailer :
            done = true;
            done2 = true;
            ++_nDataWords;
            break;

          default :
            stringstream ss;
            ss << "ERROR in MDpartEventSFGD::Init() : Unexpected data word (id: "
               << dw.GetDataType() << ")";
            cout << dw << endl;
            throw MDexception(ss.str());
            return;
            break;
        }
      }
      
      if (!done2){
        if (dw.GetGtsTag() != _gtsTag) {
            stringstream ss;
            ss << "ERROR in MDpartEventSFGD::Init() : The trigger trailer is not consistent \n(Trigger tag: "
               << dw.GetGtsTag() << "!=" << _gtsTag << ")";
            throw MDexception(ss.str());
        }
        dw.SetDataPtr(++ptr);
        dw.GetDataType();
        if (dw.GetDataType() != MDdataWordSFGD::GTSTrailer2){
            stringstream ss;
            ss << "ERROR in MDpartEventSFGD::Init() : Unexpected data word (id: "
               << dw.GetDataType() << ")";
            cout << dw << endl;
            throw MDexception(ss.str());
        } else {
            ++_nDataWords;
            _size +=4;
            if ( dw.GetGtsTime() != _previousTrTime +1 && dw.GetGtsTime() !=0 && dw.GetGtsTime() !=1 && dw.GetGtsTime() !=2)
                cout << "ERROR in MDpartEventSFGD::Init() : Trigger Time is not consistent: "
                << dw.GetGtsTime() << " != " << _previousTrTime <<" +1"<<endl;
            _gtsTime = dw.GetGtsTime();
        //cout <<"1:"<< _gtsTime<<endl;
        }
      }
        else{
          _gtsTime = _previousTrTime + 1;
            //cout<<"2:" << _gtsTime <<endl;
        }
    }
  }

  this->Validate();
}

void MDpartEventSFGD::AddTimeHit(MDdataWordSFGD &dw) {
  unsigned int xChan = dw.GetChannelId();
  if (dw.GetEdgeId()==0) {
    ++this->_nLeadingEdgeHits[xChan];
    this->_leadingEdgeHitTime[xChan].push_back(dw.GetHitTime());
    this->_leadingEdgeHitId[xChan].push_back(dw.GetHitId());
  } else {
    ++this->_nTrailingEdgeHits[xChan];
    this->_trailingEdgeHitTime[xChan].push_back(dw.GetHitTime());
    this->_trailingEdgeHitId[xChan].push_back(dw.GetHitId());
  }
}

void MDpartEventSFGD::AddAmplitudeHit(MDdataWordSFGD &dw) {
  unsigned int xChan = dw.GetChannelId();
  switch (dw.GetAmplitudeId()) {
    case MDdataWordSFGD::Amplitude_LG :
      _lgHitAmplitude[xChan] = dw.GetAmplitude();
      _lgHitAmplitudeId[xChan] = dw.GetHitId();
      _lgHit[xChan] = true;
      break;

    case MDdataWordSFGD::Amplitude_HG :
      _hgHitAmplitude[xChan] = dw.GetAmplitude();
      _hgHitAmplitudeId[xChan] = dw.GetHitId();
      _hgHit[xChan] = true;
      break;

    default :
      stringstream ss;
      ss << "ERROR in MDpartEventSFGD::Init() : Unknown  Amplitide Id ( "
      << dw.GetAmplitudeId() << ")";
//       cout << ss.str() << endl;
      throw MDexception(ss.str());
  }
}

unsigned int  MDpartEventSFGD::GetHitTime(unsigned int ih, unsigned int ich, char t) {
  int rv = 0xFFFFFFFF ;
  if ( ich > SFGD_FEB_NCHANNELS-1 ) {
    stringstream ss;
    ss << "ERROR in MDpartEventSFGD::GetHitTime() : ";
    ss << "Wrong argument: ch = " << ich;
    throw MDexception( ss.str() );
  }
  switch(t){
    case 'l':
    {
      if (ih<_nLeadingEdgeHits[ich]) { rv = _leadingEdgeHitTime[ich][ih]; }
      else {
        stringstream ss;
        ss << "ERROR in MDpartEventSFGD::GetHitTime() case l : ";
        ss << "Wrong argument: ih = " << ih;
        throw MDexception( ss.str() );
      }
      break;
    }
    case 't':
    {
      if (ih<_nTrailingEdgeHits[ich]) { rv = _trailingEdgeHitTime[ich][ih]; }
      else {
        stringstream ss;
        ss << "ERROR in MDpartEventSFGD::GetHitTime() case t : ";
        ss << "Wrong argument: ih = " << ih;
        throw MDexception( ss.str() );
      }
      break;
    }
    default:
    {
      stringstream ss;
      ss << "ERROR in MDpartEventSFGD::GetHitTime() : ";
      ss << "Wrong argument: t = " << t;
      throw MDexception( ss.str() );
    }
  }

  return rv;
}

unsigned int  MDpartEventSFGD::GetHitTimeId(unsigned int ih, unsigned int ich, char t) {
  int rv = 0xFFFFFFFF ;
  if ( ich > SFGD_FEB_NCHANNELS-1 ) {
    stringstream ss;
    ss << "ERROR in MDpartEventSFGD::GetHitId() : ";
    ss << "Wrong argument: ch = " << ich;
    throw MDexception( ss.str() );
  }
  switch(t){
    case 'l':
    {
      if (ih<_nLeadingEdgeHits[ich]) { rv = _leadingEdgeHitId[ich][ih]; }
      else {
        stringstream ss;
        ss << "ERROR in MDpartEventSFGD::GetHitId() case l : ";
        ss << "Wrong argument: ih = " << ih;
        throw MDexception( ss.str() );
      }
      break;
    }
    case 't':
    {
      if (ih<_nTrailingEdgeHits[ich]) {rv = _trailingEdgeHitId[ich][ih];}
      else {
        stringstream ss;
        ss << "ERROR in MDpartEventSFGD::GetHitId() case t : ";
        ss << "Wrong argument: ih = " << ih;
        throw MDexception( ss.str() );
      }
      break;
    }
    default:
    {
      stringstream ss;
      ss << "ERROR in MDpartEventSFGD::GetHitId() : ";
      ss << "Wrong argument: t = " << t;
      throw MDexception( ss.str() );
    }
  }

  return rv;
}


unsigned int  MDpartEventSFGD::GetHitAmplitude(unsigned int ich, char t) {
  int rv = 0xFFFFFFFF ;
  if ( ich > SFGD_FEB_NCHANNELS-1 ) {
    stringstream ss;
    ss << "ERROR in MDpartEventSFGD::GetHitAmplitude(): ";
    ss << "Wrong argument: ch = " << ich;
    throw MDexception( ss.str() );
  }

  switch(t) {
    case 'l':
      rv = _lgHitAmplitude[ich];
      break;

    case 'h':
      rv = _hgHitAmplitude[ich];
      break;

    default:
      stringstream ss;
      ss << "ERROR in MDpartEventSFGD::GetHitAmplitude(): ";
      ss << "Wrong argument: " << t;
      throw MDexception( ss.str() );

  }

  return rv;
}

unsigned int  MDpartEventSFGD::GetHitAmplitudeId(unsigned int ich, char t) {
  int rv = 0xFFFFFFFF ;
  if ( ich > SFGD_FEB_NCHANNELS-1 ) {
    stringstream ss;
    ss << "ERROR in MDpartEventSFGD::GetHitAmplitudeId(): ";
    ss << "Wrong argument: ch = " << ich;
    throw MDexception( ss.str() );
  }

  switch(t) {
    case 'l':
      rv = _lgHitAmplitudeId[ich];
      break;

    case 'h':
      rv = _hgHitAmplitudeId[ich];
      break;

    default:
      stringstream ss;
      ss << "ERROR in MDpartEventSFGD::GetHitAmplitudeId(): ";
      ss << "Wrong argument: " << t;
      throw MDexception( ss.str() );

  }

  return rv;
}

void MDpartEventSFGD::Dump() {
  cout << *this;
}



ostream &operator<<(std::ostream &s, MDpartEventSFGD &pe) {

  s << " ++++++++++ SFGD Part Even ++++++++++ \n";
  s << " Tr. tag : " << pe.GetTriggerTag() << "(" << pe.GetTriggerTagId() << ")\n";
  s << " Tr. time : " << pe.GetTriggerTime() << "\n";
  for (int ich=0 ; ich < SFGD_FEB_NCHANNELS ; ich++) {
    if (pe._lgHit[ich]) {
      s << " Ch: " << ich
        << "  Amplitude LG: " << pe.GetHitAmplitude(ich, 'l') << "  (hit Id: " << pe.GetHitAmplitudeId(ich, 'l') <<  ")\n";
    }

    if (pe._hgHit[ich]) {
      s << " Ch: " << ich
        << "  Amplitude HG: " << pe.GetHitAmplitude(ich, 'h') << "  (hit Id: " << pe.GetHitAmplitudeId(ich, 'h') <<  ")\n";
    }

    if (pe._nLeadingEdgeHits[ich]) {
      for (unsigned int ih=0; ih<pe._nLeadingEdgeHits[ich]; ++ih) {
        s << " Ch: " << ich
          << "  LTime: " << pe.GetHitTime(ih, ich, 'l') << "  (hit Id: " << pe.GetHitTimeId(ih, ich, 'l') << ")\n";
      }
    }

    if (pe._nTrailingEdgeHits[ich]) {
      for (unsigned int ih=0; ih<pe._nTrailingEdgeHits[ich]; ++ih) {
        s << " Ch: " << ich
          << "  TTime: " << pe.GetHitTime(ih, ich, 't') << "  (hit Id: " << pe.GetHitTimeId(ih, ich, 't') << ")\n";
      }
    }
  }

   s << " ++++++++++++++++++++++++++++++++++ \n\n";
  return s;
}


