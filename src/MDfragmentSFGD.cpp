#include "MDfragmentSFGD.h"
#include "MDdataWordSFGD.h"


void MDfragmentSFGD::SetDataPtr(void *d, uint32_t aSize) {
    MDdataContainer::SetDataPtr(d);
    this->Init();
}

//void MDfragmentSFGD::SetPreviousSpill(bool prSpillEx, unsigned int prSpill) {
//    _previousSpillTagExist = prSpillEx;
//    _previousSpillTag = prSpill;
//}

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
            std::stringstream ss;
            ss<<"ERROR in MDfragmentBM::Init() : 1st word is not a spill header.\n";
            ss << dw.GetDataType()<< " "<< dw << "\n";
            throw MDexception(ss.str());
        } else {
            if (dw.GetGateHeaderID() == 0) {
                _gateNumber = dw.GetGateNumber();
                _boardId = dw.GetBoardId();
                _gateType = dw.GetGateType();
//                std::cout << "1: SFGD spill Header \"A\" Board ID " << _boardId << " SpillTag: "
//                     << dw.GetGateNumber() << std::endl;
                _size += 4;
                dw.SetDataPtr(++ptr);
            }
            if (dw.GetGateHeaderID() == 1) {
                _gateTimeFrGtsEx = true;
                _gateTimeFrGts = dw.GetGateTimeFrGts();
                _boardId = dw.GetBoardId();
//                std::cout << "2: SFGD spill Header \"B\" Spill Time from GTRIG (10ns res.): " << _gateTimeFrGts
//                     << std::endl;
                _size += 4;
                dw.SetDataPtr(++ptr);
            }
            if (dw.GetDataType() != MDdataWordSFGD::GateTime) {
                std::stringstream ss;
                ss<< "ERROR in MDfragmentBM::Init() : 3rd word is not a spill spill time.\n";
                ss << dw.GetDataType()<< " "<< dw << "\n";
                throw MDexception(ss.str());
            } else {
                _gateTime = dw.GetGateTime();
                _size += 4;
                dw.SetDataPtr(++ptr);
                bool done(false);
                while (!done) {
                    dw.SetDataPtr(ptr);
//                    cout<< dw << endl;
                    if (dw.GetDataType() == MDdataWordSFGD::GTSHeader || dw.GetDataType() == MDdataWordSFGD::TimeMeas) {
//                        cout<< dw << endl;
                        MDpartEventSFGD *xPe = new MDpartEventSFGD(ptr, &_previousGtsTime, &_previousGtsTag);
                        xPe->SetTriggerEvents(&_trigEvents);
                        xPe->Init();
                        unsigned int pe_size = xPe->GetSize();
                        _size += pe_size;
                        ptr += pe_size/4;
                        if (xPe->GateTrailerExist()){
                            gateTrailer gateTrail = xPe->GetGateTrailer();
                            if (gateTrail.gateNumber_ != _gateNumber) {
                                std::stringstream ss;
                                ss << "ERROR in MDfragmentSFGD::Init() : The Gate trailer is not consistent \n(Gate #: "
                                   << gateTrail.gateNumber_ << "!=" << _gateNumber << ")";
                                throw MDexception(ss.str());
                            } else if (gateTrail.boardId_ != _boardId){
                                std::stringstream ss;
                                ss << "ERROR in MDfragmentSFGD::Init() : The Gate trailer board Id (" << gateTrail.boardId_
                                << ") != Gate header board Id (" << _boardId << ")";
                                throw MDexception(ss.str());
                            } else {
                                _gateTrailTime = gateTrail.gateTime_;
                            };
                        }
                        if (xPe->getNumDataWords() > 2){
                            _trigEvents.push_back( std::move(xPe) );
                        } else {
                            delete xPe;
                        }
                        dw.SetDataPtr(ptr);
//                        std::cout<< dw << std::endl;
                    } else if (dw.GetDataType() == MDdataWordSFGD::GTSTrailer1){
                        _previousGtsTag = dw.GetGtsTag();
                        _size += 4;
                        if (_isFirstGtsTag){
                            _isFirstGtsTag = false;
                            _firstGtsTag = dw.GetGtsTag();
                        }
                        dw.SetDataPtr(++ptr);
//                        cout<< dw << endl;
                        if (dw.GetDataType() == MDdataWordSFGD::GTSTrailer2){
                            _previousGtsTime = dw.GetGtsTime();
                            _size += 4;
                            dw.SetDataPtr(++ptr);
//                            cout<< dw << endl;
                        }
                    } else if (dw.GetDataType() == MDdataWordSFGD::EventDone){
                        _size += 4;
                        dw.SetDataPtr(++ptr);
                    }
                    else if(dw.GetDataType() == MDdataWordSFGD::GateTrailer){
                        if (dw.GetGateNumber() != _gateNumber) {
                            std::stringstream ss;
                            ss << "ERROR in MDfragmentSFGD::Init() : The Gate trailer is not consistent \n(Gate #: "
                               << dw.GetGateNumber() << "!=" << _gateNumber << ")";
                            throw MDexception(ss.str());
                        }
                        if (dw.GetBoardId() != _boardId){
                            std::stringstream ss;
                            ss << "ERROR in MDfragmentSFGD::Init() : The Gate trailer board Id (" << dw.GetBoardId()
                               << ") != Gate header board Id (" << _boardId << ")";
                            throw MDexception(ss.str());
                        }
                        _size += 4;
                        dw.SetDataPtr(++ptr);
                        if (dw.GetDataType() != MDdataWordSFGD::GateTime){
                            std::stringstream ss;
                            ss << "ERROR in MDpartEventSFGD::Init() : Unexpected data word after Gate Trailer 1 (id: "
                               << dw.GetDataType() << ")\n" << dw  << '\n';
                            throw MDexception(ss.str());
                        } else {
                            _gateTrailTime = dw.GetGateTime();
                        }
                        _size += 4;
                        dw.SetDataPtr(++ptr);
                    } else if (dw.GetDataType() == MDdataWordSFGD::FebDataTrailer){
                        _size += 4;
                        dw.SetDataPtr(++ptr);
                        done = true;
                    } else {
                        _size += 4;
                        dw.SetDataPtr(ptr);
                        std::stringstream ss;
                        ss << "ERROR in MDfragmentSFGD::Init() : Unexpected data Word \n" << dw ;
                        throw MDexception(ss.str());
                    }
                }
            }
        }
  } 
}

MDpartEventSFGD* MDfragmentSFGD::GetTriggerEventPtr(unsigned int evId) {
  if ( evId >= _trigEvents.size() ) {
    std::stringstream ss;
    ss << "ERROR in MDfragmentSFGD::GetTriggerEventPtr() : ";
    ss << "Wrong Event Id: " << evId << ". Exceeds the total number of triggers." ;
    throw MDexception( ss.str() );
  }

  return _trigEvents[evId];
}

void MDfragmentSFGD::Dump() {

}
