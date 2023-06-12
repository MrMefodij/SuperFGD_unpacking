/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "MDdataFile.h"
#include "MDdataWordSFGD.h"
#include "MDexception.h"

using namespace std;

MDdateFile::MDdateFile(string fn)
            :_eventBuffer(NULL), _fileName(fn),_curPos(0),_fileSize(0),_nBytesRead(0), _lastSpill(-1) {}

MDdateFile::~MDdateFile() {
    if (_eventBuffer) {
        free(_eventBuffer);
    }
}

bool MDdateFile::open() {
    string fullName = _fileName;
    _ifs.open( fullName.c_str() );
    if ( _ifs.fail() ) {
        cerr << "Can not open file " << fullName.c_str() << endl;
        return false;
    }
    uint32_t end;
    _curPos = _ifs.tellg();
    _ifs.seekg (0, ios::end);
    end = _ifs.tellg();
    _fileSize = end - _curPos;
    cout << " File size " << _fileSize << endl;
    if ( _fileSize%4 != 0 ) {
        cerr << " File size is not a multiple of 4. The file " << fullName.c_str() << " is rejected!" << endl;
        return false;
    }
    _ifs.seekg (0, ios::beg); // go back to the begining ( = _curPos )
    return true;
}

void MDdateFile::close() {
    _eventBuffer = 0;
    _ifs.close();
}


void MDdateFile::init() {
    this->reset();
    _curPos = _ifs.tellg();
    if (_eventBuffer) {
        delete _eventBuffer;
    }
    _eventBuffer = new char[4];
    _spill_header_pos.push_back({0,0, false, false});
    while (!_ifs.eof()) {
        _ifs.read( _eventBuffer, 4 );
        MDdataWordSFGD dw(_eventBuffer);
//        cout << dw << endl;
        switch (dw.GetDataType()) {
            case MDdataWordSFGD::GTSHeader:
                if (!insideSpill) {
                    _gtsTagBeforeSpillGate = dw.GetGtsTag();
                }
                break;
            case MDdataWordSFGD::GateHeader:
                insideSpill = true;
//                cout << dw << endl;
                if (dw.GetGateHeaderID() == 0){
                    _curPos = _ifs.tellg();
                    _spill_header_pos.back().headerA = _curPos - 4;
                    _spill_header_pos.back().headerAEx = true;
                } else if (dw.GetGateHeaderID() == 1){
                    _curPos = _ifs.tellg();
                    _spill_header_pos.back().headerB = _curPos - 4;
                    _spill_header_pos.back().headerBEx = true;
                }
                break;

            case MDdataWordSFGD::GateTrailer:
                _curPos = _ifs.tellg();
                _spill_size.push_back(_curPos - GetGateHeaderPosition(_spill_header_pos.back()) + 4);
//                cout << dw << endl;
//                cout <<_curPos/4 << " Spill size: " << _spill_size.back()/4 << endl;
                _ifs.read( _eventBuffer, 4 );
                _spill_header_pos.push_back({0,0, false, false});
                _gts_tag_spill.push_back(_gtsTagBeforeSpillGate);
                insideSpill = false;
                break;
            default:
                break;
        }
        if (_ifs.eof()){
            _curPos = _fileSize;
            _spill_size.push_back(_curPos - GetGateHeaderPosition(_spill_header_pos.back()));
            _spill_header_pos.push_back({_curPos,_curPos});
            _gts_tag_spill.push_back(_gtsTagBeforeSpillGate);
            insideSpill = false;
        }
    }
    this->reset();
}

char* MDdateFile::GetNextEvent() {

  if ( (unsigned int)(++_lastSpill) >= _spill_size.size() )
   return NULL;

  uint32_t spillSize = _spill_size[_lastSpill];
  uint32_t spillPos  = std::min(_spill_header_pos[_lastSpill].headerA, _spill_header_pos[_lastSpill].headerB);
  cout << "GetNextEvent  pos: " << spillPos/4 << "  size: " << spillSize/4 
       << " in DW units (4 bytes)" << endl;
  return GetSpill(spillPos, spillSize);
}

void MDdateFile::GoTo(uint32_t pos) {
  _ifs.seekg (pos , ios::beg);
}

char* MDdateFile::GetSpill(uint32_t pos, uint32_t size) {
  _ifs.seekg (pos , ios::beg);

  if (_eventBuffer) delete _eventBuffer;
    _eventBuffer = new char[size];

  if ( !_ifs.read(_eventBuffer, size ) ) { // read full Spill event
    throw MDexception("Unexpected End of File while trying to read event");
//     cerr << "Unexpected End of File while trying to read event" << endl;
//     cerr << _ifs.gcount() << " bits read.\n";
//     return NULL;
  }

  return _eventBuffer;
}

void MDdateFile::reset() {
  /* go back to the begining */
  _nBytesRead = 0;
  _ifs.clear();
  _ifs.seekg (0, ios::beg);
  _lastSpill = -1;
}
