#include "MDpartEventSFGD.h"


MDpartEventSFGD::MDpartEventSFGD(void *d, unsigned int* time, unsigned int* tag): MDdataContainer(d),
                                                                                  _gtsTag(-1), _nDataWords(0), _trigEvents(0) {
    _previousTrTime = time; _previousTrTag = tag;
}

void MDpartEventSFGD::SetDataPtr( void *d, uint32_t aSize ) {
    MDdataContainer::SetDataPtr(d);
    this->Init();
}



void MDpartEventSFGD::Init() {
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
    _gtsTag = 0;
    _gtsTagId = 0;
    _gtsTime = 0;
    _size = 4;

    unsigned int * ptr = Get32bWordPtr(0);

    MDdataWordSFGD dw(ptr);
    dw.SetDataPtr(ptr);
    if ( dw.IsValid() ) {
        // Check the reliability of the header and decode the header information.
        if (dw.GetDataType() == MDdataWordSFGD::GTSHeader ) {
            _gtsTag = dw.GetGtsTag();
            _gtsTagId = dw.GetGtsTagShort();
            if (dw.GetGtsTag() != *_previousTrTag +1 && *_previousTrTag!=0) {
                std::stringstream ss;
                ss << "ERROR in MDpartEventSFGD::Init() : Trigger Tag is NOT consistent with previous Trigger Tag: "
                        << _gtsTag << " != " << *_previousTrTag << "+ 1\n";
                throw MDexception(ss.str());
            }
        } else if (dw.GetDataType() == MDdataWordSFGD::TimeMeas){
            auto pointer = ptr;
            while (dw.GetDataType() != MDdataWordSFGD::GTSTrailer1){
                dw.SetDataPtr(++ptr);
            }
            _gtsTag = dw.GetGtsTag();
            _gtsTagId = dw.GetGtsTagShort();
            if (dw.GetGtsTag() != *_previousTrTag +1 && *_previousTrTag!=0) {
                std::stringstream ss;
                ss << "ERROR in MDpartEventSFGD::Init() : Trigger Tag is NOT consistent with previous Trigger Tag: "
                   << _gtsTag << " != " << *_previousTrTag << "+ 1\n";
                throw MDexception(ss.str());
            }
            ptr = pointer;
            dw.SetDataPtr(ptr);
        } else {
            std::stringstream ss;
            ss << "ERROR in MDpartEventSFGD::Init() : Unexpected data word (id: "
               << dw.GetDataType() << ")\n" << dw << '\n';
            if (dw.GetDataType() == MDdataWordSFGD::SpecialWord){
                throw MDexception(ss.str(), MDexception::MD_SEVERITY::FIFO_FULL);
            } else
                throw MDexception(ss.str());
        }

        bool reachGtsTrailer(false);
        bool reachFebTrailer(false);

        while (!reachGtsTrailer) {
            dw.SetDataPtr(++ptr);
            _size += 4;
            int dataType = dw.GetDataType();
//            std::cout <<dw.GetDataType()<< " " << dw <<std::endl;
            switch (dataType) {
                case MDdataWordSFGD::TimeMeas :
                    if (dw.GetTagId() == _gtsTagId ) {
                        this->AddTimeHit(dw);
                        this->AddTagIdShift(dw,0);
                        ++_nDataWords;
                    } else {
                        if (_trigEvents) {
                            int nTr = _trigEvents->size(), lastPending;
                            lastPending = (nTr > 3) ? nTr-4 : 0;
                            unsigned int shift = 0;
                            for (int i = nTr-1; i >= lastPending; --i) {
                                ++shift;
                                if (_trigEvents->at(i)->GetTriggerTagId() == dw.GetTagId()) {
                                    _trigEvents->at(i)->AddTimeHit(dw);
                                    _trigEvents->at(i)->AddTagIdShift(dw,shift);
                                    ++_nDataWords;
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
                                        ++_nDataWords;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    break;

                case MDdataWordSFGD::GTSTrailer1 :
                    reachGtsTrailer = true;
                    * _previousTrTag = dw.GetGtsTag();
                    ++_nDataWords;
                    break;

                case MDdataWordSFGD::EventDone :
                    ++_nDataWords;
                    break;

                case MDdataWordSFGD::GateTrailer :
                    _gateTrailerExist = true;
                    _gateTrailer.boardId_ = dw.GetBoardId();
                    _gateTrailer.gateType_ = dw.GetGateType();
                    _gateTrailer.gateNumber_ = dw.GetGateNumber();
                    ++_nDataWords;

                    dw.SetDataPtr(++ptr);
                    _size += 4;
                    if (dw.GetDataType() != MDdataWordSFGD::GateTime){
                        std::stringstream ss;
                        ss << "ERROR in MDpartEventSFGD::Init() : Unexpected data word after Gate Trailer 1 (id: "
                           << dw.GetDataType() << ")\n" << dw  << '\n';
                        throw MDexception(ss.str());
                    } else {
                        _gateTrailer.gateTime_ = dw.GetGateTime();
                    }
                    ++_nDataWords;
                    break;

                case MDdataWordSFGD::FebDataTrailer :
                    reachFebTrailer = true;
                    reachGtsTrailer = true;
                    _size -= 4;
                    break;

                default :
                    ++_nDataWords;
                    std::stringstream ss;
                    ss << "ERROR in MDpartEventSFGD::Init() : Unexpected data word (id: "
                       << dw.GetDataType() << ")\n" << dw  << '\n';
                    if (dw.GetDataType() == MDdataWordSFGD::SpecialWord){
                        throw MDexception(ss.str(), MDexception::MD_SEVERITY::FIFO_FULL);
                    } else
                        throw MDexception(ss.str());
            }
        }
        if (!reachFebTrailer) {
            if (dw.GetGtsTag() != _gtsTag) {
                std::stringstream ss;
                ss << "ERROR in MDpartEventSFGD::Init() : The trigger trailer is not consistent \n(Trigger tag: "
                   << dw.GetGtsTag() << "!=" << _gtsTag << ")";
                throw MDexception(ss.str());
            }
            dw.SetDataPtr(++ptr);
            dw.GetDataType();
            if (dw.GetDataType() != MDdataWordSFGD::GTSTrailer2) {
                std::stringstream ss;
                ss << "ERROR in MDpartEventSFGD::Init() : Unexpected data word (id: "
                   << dw.GetDataType() << ")";
                std::cout << dw << std::endl;
                if (dw.GetDataType() == MDdataWordSFGD::SpecialWord){
                    throw MDexception(ss.str(), MDexception::MD_SEVERITY::FIFO_FULL);
                } else
                    throw MDexception(ss.str());
            } else {
                ++_nDataWords;
                _size += 4;
                if (dw.GetGtsTime() != *_previousTrTime + 1 && dw.GetGtsTime() != 0 && dw.GetGtsTime() != 1)
                    std::cout << "ERROR in MDpartEventSFGD::Init() : Trigger Time is not consistent: "
                              << dw.GetGtsTime() << " != " << *_previousTrTime << " +1" << std::endl;
                _gtsTime = dw.GetGtsTime();
                *_previousTrTime = dw.GetGtsTime();
                //cout <<"1:"<< _gtsTime<<endl;
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

void MDpartEventSFGD::AddTagIdShift(MDdataWordSFGD &dw, unsigned int shift){
    unsigned int xChan = dw.GetChannelId();
    this->_tadIdShift[xChan].push_back(shift);
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
            std::stringstream ss;
            ss << "ERROR in MDpartEventSFGD::Init() : Unknown  Amplitide Id ( "
               << dw.GetAmplitudeId() << ")";
//       cout << ss.str() << endl;
            throw MDexception(ss.str());
    }
}

unsigned int  MDpartEventSFGD::GetHitTime(unsigned int ih, unsigned int ich, char t) {
    int rv = 0xFFFFFFFF ;
    if ( ich > SFGD_FEB_NCHANNELS-1 ) {
        std::stringstream ss;
        ss << "ERROR in MDpartEventSFGD::GetHitTime() : ";
        ss << "Wrong argument: ch = " << ich;
        throw MDexception( ss.str() );
    }
    switch(t){
        case 'l':
        {
            if (ih<_nLeadingEdgeHits[ich]) { rv = _leadingEdgeHitTime[ich][ih]; }
            else {
                std::stringstream ss;
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
                std::stringstream ss;
                ss << "ERROR in MDpartEventSFGD::GetHitTime() case t : ";
                ss << "Wrong argument: ih = " << ih;
                throw MDexception( ss.str() );
            }
            break;
        }
        default:
        {
            std::stringstream ss;
            ss << "ERROR in MDpartEventSFGD::GetHitTime() : ";
            ss << "Wrong argument: t = " << t;
            throw MDexception( ss.str() );
        }
    }

    return rv;
}

unsigned int MDpartEventSFGD::GetHitShift(unsigned int ih, unsigned int ich){
    int rv = 0xFFFFFFFF ;
    if ( ich > SFGD_FEB_NCHANNELS-1 ) {
        std::stringstream ss;
        ss << "ERROR in MDpartEventSFGD::GetHitId() : ";
        ss << "Wrong argument: ch = " << ich;
        throw MDexception( ss.str() );
    }
    if (ih<_nLeadingEdgeHits[ich]) { rv = _tadIdShift[ich][ih]; }
    return rv;
}

unsigned int  MDpartEventSFGD::GetHitTimeId(unsigned int ih, unsigned int ich, char t) {
    int rv = 0xFFFFFFFF ;
    if ( ich > SFGD_FEB_NCHANNELS-1 ) {
        std::stringstream ss;
        ss << "ERROR in MDpartEventSFGD::GetHitId() : ";
        ss << "Wrong argument: ch = " << ich;
        throw MDexception( ss.str() );
    }
    switch(t){
        case 'l':
        {
            if (ih<_nLeadingEdgeHits[ich]) { rv = _leadingEdgeHitId[ich][ih]; }
            else {
                std::stringstream ss;
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
                std::stringstream ss;
                ss << "ERROR in MDpartEventSFGD::GetHitId() case t : ";
                ss << "Wrong argument: ih = " << ih;
                throw MDexception( ss.str() );
            }
            break;
        }
        default:
        {
            std::stringstream ss;
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
        std::stringstream ss;
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
            std::stringstream ss;
            ss << "ERROR in MDpartEventSFGD::GetHitAmplitude(): ";
            ss << "Wrong argument: " << t;
            throw MDexception( ss.str() );

    }

    return rv;
}

unsigned int  MDpartEventSFGD::GetHitAmplitudeId(unsigned int ich, char t) {
    int rv = 0xFFFFFFFF ;
    if ( ich > SFGD_FEB_NCHANNELS-1 ) {
        std::stringstream ss;
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
            std::stringstream ss;
            ss << "ERROR in MDpartEventSFGD::GetHitAmplitudeId(): ";
            ss << "Wrong argument: " << t;
            throw MDexception( ss.str() );

    }

    return rv;
}

void MDpartEventSFGD::Dump() {
    std::cout << *this;
}



std::ostream &operator<<(std::ostream &s, MDpartEventSFGD &pe) {

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


