#include "MDfragmentSFGD.h"
#include "MDdataWordSFGD.h"

using namespace std;

void MDfragmentSFGD::SetDataPtr(void *d, uint32_t aSize, uint32_t gtsTagBeforeSpill) {
    MDdataContainer::SetDataPtr(d);
    _previousGtsTag = gtsTagBeforeSpill;
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
    this->Clean();
    _size = 4;

    unsigned int * ptr = this->Get32bWordPtr(0);
    MDdataWordSFGD dw(ptr);
    if ( dw.IsValid() ) {
        if (dw.GetDataType() != MDdataWordSFGD::GateHeader) {
            throw MDexception("ERROR in MDfragmentBM::Init() : 1st word is not a spill header.");
        } else {
            if (dw.GetGateHeaderID() == 0) {
                _gateNumber = dw.GetGateNumber();
                _boardId = dw.GetBoardId();
                cout << "1: SFGD spill Header \"A\" Board ID " << _boardId << " SpillTag: "
                     << dw.GetGateNumber() << endl;
                _size += 4;
                dw.SetDataPtr(++ptr);
            }
            if (dw.GetGateHeaderID() == 1) {
                _gateTimeFrGts = dw.GetGateTimeFrGts();
                _boardId = dw.GetBoardId();
                cout << "2: SFGD spill Header \"B\" Spill Time from GTRIG (10ns res.): " << _gateTimeFrGts
                     << endl;
                _size += 4;
                dw.SetDataPtr(++ptr);
            }
            if (dw.GetDataType() != MDdataWordSFGD::GateTime) {
                throw MDexception("ERROR in MDfragmentBM::Init() : 2nd word is not a spill spill time.");
            } else {
                _gateTime = dw.GetGateTime();
                _size += 4;
                dw.SetDataPtr(++ptr);
                bool done(false);
                while (!done) {
                    dw.SetDataPtr(ptr);
                    if (dw.GetDataType() == MDdataWordSFGD::GTSHeader) {
                        MDpartEventSFGD *xPe = new MDpartEventSFGD(ptr, _previousGtsTime, _previousGtsTag);
                        xPe->SetTriggerEvents(&_trigEvents);
                        xPe->Init();
                        unsigned int pe_size = xPe->GetSize();
                        _size += pe_size;
                        ptr += pe_size/4;
                        if (xPe->getNumDataWords() > 2){
                            _trigEvents.push_back( xPe );
                            //cout<< dw.GetGateNumber()<< " " <<GetBoardId()<<endl;
                            if(xPe->spillHeaderAExists()) {
                                _gateNumber = xPe->GetSpillHeaderA();
                                if (_boardId == xPe->GetSpillHeaderABoardID()){
                                    _boardId = xPe->GetSpillHeaderABoardID();
                                } else {
                                    throw MDexception("ERROR in MDfragmentSFGD::Init() :  The Spill trailer Board ID is not consistent.");
                                }
                            }
                        } else {
                            delete xPe;
                        }
                        dw.SetDataPtr(ptr);
                        cout << "After GTS Gate: " << dw << endl;
                    } else if (dw.GetDataType() == MDdataWordSFGD::GTSTrailer1){
//                        _previousGtsTag = dw.GetGtsTag();
                        _size += 4;
                        dw.SetDataPtr(++ptr);
                        if (dw.GetDataType() == MDdataWordSFGD::GTSTrailer2){
                            _previousGtsTime = dw.GetGtsTime();
                            _size += 4;
                            dw.SetDataPtr(++ptr);
                        }
                    } else {
                        //cout << dw << endl;
                        _size += 4;
                        dw.SetDataPtr(++ptr);
                        //throw MDexception("ERROR in MDfragmentBM::Init() : Wrong data type.");
                    }
                }
            }
        }
        /*
                case MDdataWordSFGD::GateTime:
                    _gateTime = dw.GetGateTime();
                    _size += 4;
                    dw.SetDataPtr(++ptr);
                    break;
                case MDdataWordSFGD::GateTrailer:
                    if (_gateNumber != dw.GetGateNumber()) {
                        cout << dw.GetGateNumber() << "!=" << _gateNumber << endl;
                        throw MDexception("ERROR in MDfragmentSFGD::Init() :  The Spill trailer is not consistent.");
                    }
                    _size += 4;
                    dw.SetDataPtr(++ptr);
                    done = true;
                    break;
//                case MDdataWordSFGD::HoldTime:
//                    throw MDexception("L1Hold time out of GTS Slot");
//                    break;
                case MDdataWordSFGD::GTSTrailer1:
                    _size += 4;
                    dw.SetDataPtr(++ptr);
                    break;
                case MDdataWordSFGD::GTSTrailer2:
                    _size += 4;
                    dw.SetDataPtr(++ptr);
                    break;
                default:
                    _size += 4;
                    dw.SetDataPtr(++ptr);
//                    throw MDexception("ERROR in MDfragmentSFGD::Init() : Wrong data type.");
                    break;
                case MDdataWordSFGD::GTSHeader:
                    MDpartEventSFGD *xPe = new MDpartEventSFGD(ptr, _previousGtsTime, _previousGtsTag);
                    xPe->SetTriggerEvents(&_trigEvents);
                    xPe->Init();
                    unsigned int pe_size = xPe->GetSize();
                    _size += pe_size;
                    ptr += pe_size/4;
                    if (xPe->getNumDataWords() > 3){
                        _trigEvents.push_back( xPe );
                        //cout<< dw.GetGateNumber()<< " " <<GetBoardId()<<endl;
                        if(xPe->spillHeaderAExists()) {
                            _gateNumber = xPe->GetSpillHeaderA();
                            if (_boardId == xPe->GetSpillHeaderABoardID()){
                                _boardId = xPe->GetSpillHeaderABoardID();
                            } else {
                                throw MDexception("ERROR in MDfragmentSFGD::Init() :  The Spill trailer Board ID is not consistent.");
                            }
                        }
                    } else {
                        delete xPe;
                    }
                    dw.SetDataPtr(ptr);
                    cout << "After GTS Gate: " << dw << endl;
                    break;
            }*/


    /*
    {
                if (dw.GetDataType() == MDdataWordSFGD::GTSHeader) {
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
    cout<<"\n";*/
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
