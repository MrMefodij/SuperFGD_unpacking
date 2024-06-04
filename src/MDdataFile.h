/* This file is part of SuperFGD software package. This software
 * package is designed for internal use for the SuperFGD detector
 * collaboration and is tailored for this use primarily.
 *
 * Unpacking is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Unpacking is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SuperFGD Unpacking.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//
// Created by amefodev on 13.06.2023. mrmefodij@gmail.com
//


#ifndef __MDDATEFILE_H
#define __MDDATEFILE_H

#include <string>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "MDexception.h"

class MDdateFile {
protected:
    std::ifstream          _ifs;               // Input File Stream
    char*                  _eventBuffer;

public:
    std::string            _fileName;
    unsigned long long int               _curPos;            // current stream position in file
    unsigned long long int               _fileSize;
    unsigned long long int               _nBytesRead;

    unsigned long long int                    _lastSpill;

    MDdateFile(std::string fn);
    ~MDdateFile();

    void SetFileName(std::string fn) { _fileName = fn ;}
    std::string GetFileName()        { return _fileName;}

    bool  open(bool print_values = true);
    void  close();

    void  init();
    char* GetNextEvent(bool print_values = false);
    unsigned int GetEventSize();

    unsigned int GetOcbEventNumber();
    unsigned long long int GetStreamPos() {
        _curPos = _ifs.tellg();
        return _curPos;
    }
    unsigned int GetHeaderB() const;
    void GoTo(unsigned long long int pos);
    char* GetSpill(unsigned long long int pos, unsigned long long int size);
    unsigned int GetCurrentSpillSize();
    void reset();

private:
    struct GateHeadersPositions{
        unsigned long long int headerA;
        unsigned long long int headerB;
        bool headerAEx;
        bool headerBEx;
    };
    uint32_t GetGateHeaderPosition(GateHeadersPositions position){
        return position.headerA;
    }

    std::vector<uint32_t> _gts_tag_spill;
    std::vector<GateHeadersPositions> _spill_header_pos;
//    std::vector<uint32_t> _spill_size;
    unsigned int _gtsTagBeforeSpillGate = 0;

    std::vector<uint32_t> _event_header_pos;
    unsigned int _ocb_event_number;
    std::vector<unsigned int> _ocb_event_number_vector;
    std::vector<unsigned int> _ocb_event_size;
    std::vector<unsigned int> _event_size;
    unsigned int _currentSpillSize;

};

#endif
