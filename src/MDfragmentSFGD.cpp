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

#include "MDfragmentSFGD.h"
#include "MDdataWordSFGD.h"

using namespace std;

void MDfragmentSFGD::SetDataPtr(void *d, uint32_t aSize ) {
  MDdataContainer::SetDataPtr(d);
  this->Init();
}

void MDfragmentSFGD::SetPreviousSpill(bool prSpillEx, unsigned int prSpill) {
    _previousSpillTagExist = prSpillEx;
    _previousSpillTag = prSpill;
}

void MDfragmentSFGD::Clean() {
  this->UnValidate();

  int nTr = _trigEvents.size();
  for (int i=0; i<nTr; ++i) {
    delete _trigEvents[i];
  }
  _trigEvents.resize(0);
}

void MDfragmentSFGD::Init() {
   //  cout << " Calling MDfragmentSFGD::Init() " << endl;
  this->Clean();
  _size = 4;

  unsigned int * ptr = this->Get32bWordPtr(0);
  MDdataWordSFGD dw(ptr);
  if ( dw.IsValid() ) {
       
    if (dw.GetDataType() != MDdataWordSFGD::GateHeader && dw.GetGateHeaderID() != 1) {
        throw MDexception("ERROR in MDfragmentSFGD::Init() : 1st word is not a spill header.");
    } else {
        _gateTimeFrGts = dw.GetGateTimeFrGts();
        _boardId = dw.GetBoardId();
        cout << "SFGD spill Header \"B\" Spill Time from GTRIG (10ns res.): " << _gateTimeFrGts << endl;
        _size += 4;
        ++ptr;
        dw.SetDataPtr(ptr);
      
        if (dw.GetDataType() != MDdataWordSFGD::GateTime) {
             throw MDexception("ERROR in MDfragmentSFGD::Init() : 2nd word is not a spill spill time.");
        } else {
            _gateTime = dw.GetGateTime();
            ++ptr;
            dw.SetDataPtr(ptr);
            _size += 4;
            bool done(false);
            _previousGtsTime =0;
            while (!done) {
                
                //cout   <<_size/4 <<" "<<dw<<endl;
                dw.SetDataPtr(ptr);
                //cout   <<_size/4<<endl;
                if (dw.GetDataType() == MDdataWordSFGD::GTSHeader) {
                    //cout   <<_size/4 <<" " << dw.GetDataType()<<endl;
                    MDpartEventSFGD *xPe = new MDpartEventSFGD(ptr, _previousGtsTime, _previousGtsTag);
                    xPe->SetTriggerEvents(&_trigEvents);
                    xPe->Init();
                    unsigned int pe_size = xPe->GetSize();
                    _size += pe_size;
                    ptr += pe_size/4;
                    if (xPe->getNumDataWords() > 2) {
                        _trigEvents.push_back( xPe );
                        //cout<< dw.GetGateNumber()<< " " <<GetBoardId()<<endl;
                        if(xPe->spillHeaderAExists()) {
                            _gateNumber = xPe->GetSpillHeaderA();
                            if (_boardId == xPe->GetSpillHeaderABoardID())
                                _boardId=xPe->GetSpillHeaderABoardID();
                            else 
                                throw MDexception("ERROR in MDfragmentSFGD::Init() :  The Spill trailer Board ID is not consistent.");
                        }
                    } else {
                        delete xPe;
                        }
                    _previousGtsTime = xPe->GetTriggerTime();
                    _previousGtsTag  = xPe->GetTriggerTag();
                        //cout <<"1: "<< _previousGtsTag<<endl;
                } else if (dw.GetDataType() == MDdataWordSFGD::GateHeader && dw.GetGateHeaderID() == 0){
                    _gateNumber = dw.GetGateNumber();
                     _boardId = dw.GetBoardId();
                     cout   <<"1: SFGD spill Header \"A\" Board ID "<< _boardId <<" SpillTag: "<< dw.GetGateNumber()<<endl;
                      ++ptr;
                     dw.SetDataPtr(ptr);
                     _size += 4;
                } else if (dw.GetDataType() == MDdataWordSFGD::GateTrailer ) {
                    
                    if (!_previousSpillTagExist){
                        _previousSpillTag = _gateNumber ;
                    } else{
                        if (_gateNumber != _previousSpillTag + 1){
                            cout << "ERROR in MDfragmentSFGD::Init() : No events for Spill Tag : " <<_previousSpillTag + 1<<endl;
                            if (_gateNumber > _previousSpillTag){
                                _previousSpillTag = _gateNumber;
                            } else {
                                throw MDexception("ERROR in MDfragmentSFGD::Init() :  The Spill Tag  less than previous Spill Tag.");
                            }
                        } else {
                            _previousSpillTag++;
                        }
                    }
                    
                    dw.SetDataPtr(--ptr);
                    if (dw.GetDataType() == MDdataWordSFGD::GateTrailer) {
                        if (_gateNumber == dw.GetGateNumber()){
                            //cout<< "a: "<<dw<<endl;
                            done = true;
                        } else {
                            cout << dw.GetGateNumber() << "!=" << _gateNumber << endl;
                            throw MDexception("ERROR in MDfragmentSFGD::Init() :  The Spill trailer is not consistent.");
                        }
                    }
                    else {
                        throw MDexception("ERROR in MDfragmentSFGD::Init() : Wrong data type.");
                    }
                    
                    dw.SetDataPtr(++ptr);
                    if (dw.GetDataType() == MDdataWordSFGD::GateTime ){
                        _gateTrailTime = dw.GetGateTime();
                        _size += 4;
                    }
                    else {
                        throw MDexception("ERROR in MDfragmentSFGD::Init() : Wrong data type.");
                    }
                    
                } else {
                    //cout << dw << endl;
                    ++ptr;
                    _size += 4;
                    dw.SetDataPtr(ptr);
                    //throw MDexception("ERROR in MDfragmentSFGD::Init() : Wrong data type.");
                }
            }
            
        }
    }
    cout<<"\n";
  }
}

MDpartEventSFGD* MDfragmentSFGD::GetTriggerEventPtr(unsigned int evId) {
  if ( evId >= _trigEvents.size() ) {
    stringstream ss;
    ss << "ERROR in MDfragmentSFGD::GetTriggerEventPtr() : ";
    ss << "Wrong Event Id: " << evId << ". Exceeds the total number of triggers." ;
    throw MDexception( ss.str() );
  }

  return _trigEvents[evId];
}

void MDfragmentSFGD::Dump() {

}
