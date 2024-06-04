//
// Created by amefodev on 02.04.2024.
//

#include <TFile.h>
#include <pthread.h>
#include "Pthread_Unpacking.h"
#include "MDfragmentSFGD.h"

void UnpackFile(const std::string&  fileName, const std::string& outputFilePrefix, pthread_mutex_t* std_out_lock){
    std::ifstream ifs((fileName).c_str());
    if (ifs.fail()) {
        std::cerr << "Can not open file " << fileName << std::endl;
        return;
    }

    std::string rootFileOutput = outputFilePrefix + "_plots.root";

    pthread_mutex_lock(std_out_lock);
    std::cout << "ROOT output: " << rootFileOutput<<std::endl;
    pthread_mutex_unlock(std_out_lock);

    TFile rfile(rootFileOutput.c_str(), "recreate");
    MDdateFile dfile(fileName);
// Open the file and loop over events.
    unsigned int BordID = 0;
    char *eventBuffer = nullptr;
    unsigned int ocbEventNumber = 0;
    Pthread_Unpacking pthread(&dfile, &ocbEventNumber);

    if (dfile.open()) { // There is a valid files to unpack
        dfile.init();
        unsigned int currentSpillSize[SFGD_FEBS_NUM] = {0};
        do { // Loop over all spills
            eventBuffer = dfile.GetNextEvent();

            pthread.FillTTree();
            pthread.GetToaEventDummy(eventBuffer);
            ocbEventNumber = dfile.GetOcbEventNumber();
//                cout << "OCB Event Number: " << ocbEventNumber << endl;

        } while (eventBuffer);
    }

    rfile.cd();
    pthread.AllEventsWrite();

    dfile.close();
    rfile.Close();
    delete eventBuffer;
}

unpacked_structure::unpacked_structure() {
    for (int i = 0; i < SFGD_FEBS_NUM; ++i) {
        _FEBs[i].clear();
    }
}

Pthread_Unpacking::Pthread_Unpacking(MDdateFile * dFile, unsigned int * ocbEventNumber)
        : _dFile(dFile),_ocbEventNumber(ocbEventNumber){

}

void Pthread_Unpacking::GetToaEventDummy(char* eventBuffer) {
    *_ocbEventNumber = _dFile->GetOcbEventNumber();
//    std::cout << "OCB Event Number: " << *_ocbEventNumber << std::endl;
    try {
        MDfragmentSFGD spill;
        spill.SetDataPtr(eventBuffer);
        int nTr = spill.GetNumOfTriggers();
        unsigned int BordID = spill.GetBoardId();
        //10ns = ((~10ms * 1000) = ~10us + ~10us)*1000 + ~10ns = 10ns
        unsigned long long int GateTimeNS{((unsigned long long int)spill.GetGateTime() * 1000 +  (unsigned long long int)spill.GetFirstGTSTag()%1000 - 1) * 1000 + (unsigned long long int)spill.GetGateTimeFrGTS()};
//        std::cout << AS_KV(BordID) << " " << AS_KV(spill.GetGateNumber()) << " " << AS_KV(GateTimeNS) <<std::endl;
        ToaEventDummy event(*_ocbEventNumber, BordID, spill.GetGateType(), spill.GetGateNumber(),
                            GateTimeNS, spill.GateTimeFrGTSEx(), spill.GetGateTimeFrGTS(), spill.GetGateTrailTime());
        event.ReserveEventVectorSize((int)_dFile->GetEventSize()/2);
        std::vector<TSFGDigit *> pointerToMaxToT[SFGD_FEB_NCHANNELS];
//                    std::fill(pointerToMaxToT->begin(), pointerToMaxToT->end(), nullptr);
        for (int i = 0; i < SFGD_FEB_NCHANNELS; ++i) {
            pointerToMaxToT[i] = {NULL};
        }
        unsigned int maxToT[SFGD_FEB_NCHANNELS] = {0};
        unsigned int hitHgADC[SFGD_FEB_NCHANNELS] = {0};
        unsigned int hitLgADC[SFGD_FEB_NCHANNELS] = {0};
        for (int i = 0; i < nTr; ++i) {
            MDpartEventSFGD *trEv = spill.GetTriggerEventPtr(i);
            unsigned int gtsTime = trEv->GetTriggerTime();
//                        trEv->Dump();
            for (int ich = 0; ich < SFGD_FEB_NCHANNELS; ++ich) {
                unsigned int nlHits = 0;
                nlHits = trEv->GetNLeadingEdgeHits(ich);
                for (unsigned int ih = 0; ih < nlHits; ++ih) {
                    bool trailTimeExist = false;
                    unsigned int hitId = trEv->GetHitTimeId(ih, ich, 'l');
                    unsigned int hitLeadTime = trEv->GetLeadingTime(ih, ich);
                    unsigned int timeFromGateStart = hitLeadTime + gtsTime + 8000 - spill.GetGateTimeFrGTS()*8;
                    unsigned int hitTrailTime = 0;
                    unsigned int ntHits = trEv->GetNTrailingEdgeHits(ich);

                    for (unsigned int ith = 0; ith < ntHits; ith++) {
                        if (hitId == trEv->GetHitTimeId(ith, ich, 't') &&
                            trEv->GetTrailingTime(ith, ich) >= hitLeadTime) {
                            hitTrailTime = trEv->GetTrailingTime(ith, ich);
                            trailTimeExist = true;
                        }
                    }
                    if (trailTimeExist == false) {
                        if (i + 1 < nTr) {
                            trEv = spill.GetTriggerEventPtr(i + 1);
                            ntHits = trEv->GetNTrailingEdgeHits(ich);
                            unsigned int ith = 0;
                            while (trailTimeExist == false && ith < ntHits) {
                                if (hitId == trEv->GetHitTimeId(ith, ich, 't')) {
                                    hitTrailTime = trEv->GetTrailingTime(ith, ich) + 4000;
                                    trailTimeExist = true;
                                }
                                ith++;
                            }
                            trEv = spill.GetTriggerEventPtr(i);
                        }
                    }
                    unsigned int ToT=1;
                    if (trailTimeExist){
                        ToT = hitTrailTime - hitLeadTime;
                    }
                    event.SetOaEvent(TSFGDigit(gtsTime, ich, hitLeadTime, trailTimeExist, hitTrailTime,
                                               hitHgADC[ich], hitLgADC[ich], ToT, timeFromGateStart));
                    if (ToT > 1 && ToT == maxToT[ich]){
                        pointerToMaxToT[ich].push_back(event.GetPointerToLastEvent());
                    } else if (ToT > 1 && ToT > maxToT[ich]){
                        maxToT[ich] = ToT;
                        pointerToMaxToT[ich].clear();
                        pointerToMaxToT[ich].push_back(event.GetPointerToLastEvent());
                    }
                }
                if (trEv->HGAmplitudeHitExists(ich)){
                    hitHgADC[ich] = trEv->GetHitAmplitude(ich, 'h');
                }
                if (trEv->LGAmplitudeHitExists(ich)) {
                    hitLgADC[ich] = trEv->GetHitAmplitude(ich, 'l');
                }
                if (pointerToMaxToT[ich].size() && pointerToMaxToT[ich].back() != nullptr) {
//                                cout << AS_KV(ich) << endl;
                    if (  hitHgADC[ich] > 0 ) {
                        for (auto& ptr : pointerToMaxToT[ich]) {
                            ptr->SetHighGainADC(hitHgADC[ich]);
                        }
                    }
                    if (  hitLgADC[ich] > 0 ) {
                        for (auto& ptr : pointerToMaxToT[ich]) {
                            ptr->SetLowGainADC(hitLgADC[ich]);
                        }
                    }
                }
            }
        }

        _gate._oaEventDummySize+=event.GetNumberHits();
//        std::cout << "Number of hits: " << event.GetNumberHits() << " : " << _gate._oaEventDummySize << std::endl <<std::endl;
//                    event.PrintToaEventDummy();
        _gate._FEBs[BordID].push_back(event);
        for (int ich = 0; ich < SFGD_FEB_NCHANNELS; ++ich) {
            pointerToMaxToT[ich].clear();
        }
    } catch (MDexception &lExc) {
        std::cerr << lExc.GetDescription() << std::endl
                  << "Unpacking exception\n"
                  << "Spill skipped!\n\n";
    } catch (std::exception &lExc) {
        std::cerr << lExc.what() << std::endl
                  << "Standard exception\n"
                  << "Spill skipped!\n\n";
    } catch (...) {
        std::cerr << "Unknown exception occurred...\n"
                  << "Spill skipped!\n\n";
    }
}

void Pthread_Unpacking::FillTTree(){
    if (_dFile->GetOcbEventNumber() != *_ocbEventNumber && !_firstEventNumber) {
        if (_allFebInitialized){
            _AllEvents.Fill();
            _gate._oaEventDummySize = 0;
            for (const auto i : _gate._availableFeb) {
                _gate._FEBs[i].clear();
            }
        } else {
            for (unsigned int i = 0; i < SFGD_FEBS_NUM; ++i) {
                if (!_gate._FEBs[i].empty()){
                    _gate._availableFeb.push_back(i);
                    _sFEBnum.str("");
                    _sFEBnum << i;
                    _sFEB = "FEB_" + _sFEBnum.str();
                    _AllEvents.Branch((_sFEB).c_str(), "std::vector<ToaEventDummy>", &_gate._FEBs[i]);
                }
            }
            _AllEvents.Branch("oaEventDummySize", &_gate._oaEventDummySize, "oaEventDummySize/i");
            _AllEvents.Fill();
            for (const auto i : _gate._availableFeb) {
                _gate._FEBs[i].clear();
            }
            _allFebInitialized = true;
        }
    } else {
        _firstEventNumber = false;
    }
}

void Pthread_Unpacking::AllEventsWrite() {
    _AllEvents.Write("", TObject::kOverwrite);
}