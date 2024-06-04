//
// Created by amefodev on 02.04.2024.
//

#ifndef UNPACKING_PTHREAD_UNPACKING_H
#define UNPACKING_PTHREAD_UNPACKING_H

#include <TTree.h>
#include "MDdataFile.h"
#include "ToaEventDummy.h"
#include "SFGD_defines.h"

void UnpackFile(const std::string&  fileName, const std::string & outputFilePrefix, pthread_mutex_t* std_out_lock);

struct unpacked_structure{
    unpacked_structure();
    std::vector<ToaEventDummy> _FEBs[SFGD_FEBS_NUM];
    std::vector<unsigned int> _availableFeb;
    unsigned int _oaEventDummySize = 0;
};


class Pthread_Unpacking {
public:
    Pthread_Unpacking(MDdateFile * dFile, unsigned int *ocbEventNumber);
    void GetToaEventDummy(char* eventBuffer);
    void FillTTree();
    void AllEventsWrite();
private:
    bool _firstEventNumber = true;
    bool _allFebInitialized = false;
    TTree _AllEvents = TTree("AllEvents", "The ROOT tree of events");


    MDdateFile * const _dFile;
    char * _eventBuffer;
    unsigned int * _ocbEventNumber;

    unpacked_structure _gate;
    std::ostringstream _sFEBnum;
    std::string _sFEB;
};

#endif //UNPACKING_PTHREAD_UNPACKING_H
