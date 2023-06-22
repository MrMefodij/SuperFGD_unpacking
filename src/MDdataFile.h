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
    uint32_t               _curPos;            // current stream position in file
    uint32_t               _fileSize;
    uint32_t               _nBytesRead;

    int                    _lastSpill;

    MDdateFile(std::string fn);
    ~MDdateFile();

    void SetFileName(std::string fn) { _fileName = fn ;}
    std::string GetFileName()        { return _fileName;}

    bool  open();
    void  close();

    void  init();
    char* GetNextEvent();

    unsigned int GetOcbEventNumber();
    uint32_t GetStreamPos() {
        _curPos = _ifs.tellg();
        return _curPos;
    }
    void GoTo(uint32_t pos);
    char* GetSpill(uint32_t pos, uint32_t size);

    void reset();

private:
    struct GateHeadersPositions{
        uint32_t headerA;
        uint32_t headerB;
        bool headerAEx;
        bool headerBEx;
    };
    uint32_t GetGateHeaderPosition(GateHeadersPositions position){
        if (!position.headerAEx || !position.headerBEx){
            std::cout<< "Gate header A or B doesn't exist"<<std::endl;
//            throw MDexception("Gate header A or B doesn't exist");
            return std::min(position.headerA, position.headerB);
        }
        return std::min(position.headerA, position.headerB);
    }

    std::vector<uint32_t> _gts_tag_spill;
    std::vector<GateHeadersPositions> _spill_header_pos;
    std::vector<uint32_t> _spill_size;
    bool insideSpill = false;
    unsigned int _gtsTagBeforeSpillGate = 0;

    std::vector<uint32_t> _event_header_pos;
    unsigned int _ocb_event_number;
    std::vector<unsigned int> _ocb_event_number_vector;
    std::vector<uint32_t> _ocb_event_size;
};

#endif
