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

#ifndef __MDDATACONTAINER_H
#define __MDDATACONTAINER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <utility>
#include <vector>
#include <string>

#include "MDexception.h"


class MDdataContainer {
 protected:
  unsigned char* _data;
  unsigned int   _size;
  bool           _valid;

 public:
  MDdataContainer(void *d=0, unsigned int s=1)
  :_data((unsigned char*)d),_size(s),_valid(false) {}

  MDdataContainer( MDdataContainer& dc) {
    _data  = dc.GetDataPtr();
    _size  = dc.GetSize();
    _valid = dc.IsValid();
  }

  virtual ~MDdataContainer(){}
  unsigned char* GetDataPtr(unsigned int i=0) {
    if ( i<_size ) return &_data[i];
    else return NULL;
  }

  uint32_t* Get32bWordPtr(unsigned int i=0) {
    if ( (i*4) < _size ) return (uint32_t*)&_data[i*4];
    else {
      std::stringstream ss;
      ss << "ERROR in MDdataContainer::Get32bWordPtr - the size is exceeded ";
      ss << "(i=" << i << " size=" << _size <<").";
      throw MDexception(ss.str());
      return NULL;
    }
  }

  bool           IsValid( void ){ return _valid; }
  unsigned int   GetSize() {return _size;}

  virtual void SetDataPtr( void *d, uint32_t aSize=1 ) {
    _data = (unsigned char*)d;
    _valid = false;
    _size = 1;
  }

  void Validate()   { _valid = true;}
  void UnValidate() { _valid = false;}
  void SetSize(unsigned int s) {_size = s;}

  virtual void Dump(){
    if (_size%4) {
      std::cerr << " Not 32 bits data !! Trying to ignore \n" ;
      return;
    }

    unsigned int iword;
    unsigned int nword=_size>>2;

    for (iword=0 ; iword < nword ; ++iword) {
        std::cout << std::noshowbase << std::hex ;
        std::cout << std::setfill('0') << std::setw(8)
           << *Get32bWordPtr(iword) << "  ";
    }
      std::cout << std::endl;
      std::cout << std::dec << std::noshowbase << std::setfill(' ') ;
  }
};

#endif
