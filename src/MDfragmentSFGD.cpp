#include "MDfragmentSFGD.h"
#include "MDdataWordSFGD.h"

using namespace std;

void MDfragmentSFGD::SetDataPtr(void *d, uint32_t aSize) {
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
                cout <<dw.GetDataType()<< " "<< dw << endl;
                throw MDexception("ERROR in MDfragmentBM::Init() : 3rd word is not a spill spill time.");
            } else {
                _gateTime = dw.GetGateTime();
                _size += 4;
                dw.SetDataPtr(++ptr);
                bool done(false);
                while (!done) {
                    dw.SetDataPtr(ptr);
//                    cout<< dw << endl;
                    if (dw.GetDataType() == MDdataWordSFGD::GTSHeader) {
//                        cout<< dw << endl;
                        MDpartEventSFGD *xPe = new MDpartEventSFGD(ptr, &_previousGtsTime, &_previousGtsTag);
                        xPe->SetTriggerEvents(&_trigEvents);
                        xPe->Init();
                        unsigned int pe_size = xPe->GetSize();
                        _size += pe_size;
                        ptr += pe_size/4;
                        if (xPe->getNumDataWords() > 2){
                            _trigEvents.push_back( xPe );
                        } else {
                            delete xPe;
                        }
                        dw.SetDataPtr(ptr);
//                        cout<< dw << endl;
                    } else if (dw.GetDataType() == MDdataWordSFGD::GTSTrailer1){
                        _previousGtsTag = dw.GetGtsTag();
                        _size += 4;
                        dw.SetDataPtr(++ptr);
//                        cout<< dw << endl;
                        if (dw.GetDataType() == MDdataWordSFGD::GTSTrailer2){
                            _previousGtsTime = dw.GetGtsTime();
                            _size += 4;
                            dw.SetDataPtr(++ptr);
//                            cout<< dw << endl;
                        }
                    } else if (dw.GetDataType() == MDdataWordSFGD::GateTime){
                        dw.SetDataPtr(--ptr);
                        if (dw.GetDataType() != MDdataWordSFGD::GateTrailer){
                            throw MDexception("ERROR in MDfragmentBM::Init() : preLast word is not a spill trailer.");
                        } else {
                            if (dw.GetGateNumber()!= _gateNumber){
                                stringstream ss;
                                ss << "ERROR in MDfragmentSFGD::Init() : The Gate trailer is not consistent \n(Gate #: "
                                   << dw.GetGateNumber() << "!=" << _gateNumber << ")";
                                throw MDexception(ss.str());
                            } else {
                                _previousSpillTag = dw.GetGateNumber();
                                dw.SetDataPtr(++ptr);
                                _gateTrailTime = dw.GetGateTime();
                                done = true;
                                cout<<endl;
                            }
                        }
                    } else {
                        _size += 4;
                        dw.SetDataPtr(++ptr);
//                        cout << dw << endl;
                    }
                }
            }
        }
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
