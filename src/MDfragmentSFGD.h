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

#ifndef __MDFRAGMENT_SFGD_H
#define __MDFRAGMENT_SFGD_H

#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <iostream>

#include "MDdataContainer.h"
#include "MDpartEventSFGD.h"

class MDfragmentSFGD : public MDdataContainer {
public:

    MDfragmentSFGD(void *d = 0 ) : MDdataContainer(d) {}
    virtual ~MDfragmentSFGD() { this->Clean(); }

    void SetDataPtr( void *d, uint32_t aSize=0);
//    void SetPreviousSpill(bool prSpillEx = false, unsigned int prSpill=0);

    void Dump();
    void Init();
    void Clean();

    unsigned int GetBoardId() const                         {return _boardId;}
    unsigned int GetGateNumber() const                      {return _gateNumber;}
    unsigned int GetGateTime() const                        {return _gateTime;}
    unsigned int GetGateTimeFrGTS() const                   {return _gateTimeFrGts;}
    unsigned int GetGateType() const                        {return _gateType;}
  
    unsigned int GetGateTrailNumber() const                 {return _gateTrailNumber;}
    unsigned int GetGateTrailTime() const                   {return _gateTrailTime;}
  
    unsigned int GetNumOfTriggers() const                   {return _trigEvents.size();}

    void SetGateNumber(unsigned int gateNum)                {_gateNumber = gateNum;}
    void SetGateTimeFromGTS(unsigned int gateTimeFrGts)     {_gateTimeFrGts = gateTimeFrGts;}

    MDpartEventSFGD* GetTriggerEventPtr(unsigned int evId);

private:
     
    unsigned int _boardId = 0;
    unsigned int _gateType = 0;
    unsigned int _gateNumber = 0;
    unsigned int _gateTime = 0;
    unsigned int _gateTimeFrGts = 0;
  
    unsigned int _gateTrailNumber = 0;
    unsigned int _gateTrailTime = 0;

    unsigned int _previousGtsTime = 0;
    unsigned int _previousGtsTag = 0;
  
//    bool         _previousSpillTagExist = false;
//    unsigned int _previousSpillTag  = 0;
  
    std::vector <MDpartEventSFGD*> _trigEvents = {};
};

// ostream &operator<<(std::ostream &s, MDfragmentSFGD &df);

#endif
