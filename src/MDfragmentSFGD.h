/* This file is part of BabyMINDdaq software package. This software
 * package is designed for internal use for the Baby MIND detector
 * collaboration and is tailored for this use primarily.
 *
 * BabyMINDdaq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BabyMINDdaq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BabyMINDdaq.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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

  void SetDataPtr( void *d, uint32_t aSize=0 );
  void SetPreviousSpill(bool prSpillEx = false, unsigned int prSpill=0);
  void Dump();
  void Init();
  void Clean();

  unsigned int GetBoardId()             {return _boardId;}
  unsigned int GetSpillTag()            {return _spillTag;}
  unsigned int GetSpillTime()           {return _spillTime;}

  unsigned int GetSpillTimeGTrig()      {return _spillTimeGTrig;}
  
  unsigned int GetSpillTrailTad()       {return _spillTrailTag;}
  unsigned int GetSpillTrailTime()      {return _spillTrailTime;}
  
  unsigned int GetNumOfTriggers()       {return _trigEvents.size();}
  MDpartEventSFGD*    GetTriggerEventPtr(unsigned int evId);

 private:
     
  unsigned int _boardId;
  unsigned int _spillTag;
  unsigned int _humidity;
  unsigned int _temperature;
  unsigned int _spillTime;
  

  //unsigned int _spillDAQType;
  unsigned int _spillTimeGTrig;
  
  unsigned int _spillTrailTag;
  //unsigned int _spillHumidity;
  //unsigned int _spillTemp;
  unsigned int _spillTrailTime;
  
  unsigned int _previousTrTime = 0;
  unsigned int _previousTrTag = 0;
  
  bool _previousSpillTagExist;
  unsigned int _previousSpillTag ;
  
  std::vector <MDpartEventSFGD*> _trigEvents;
};

// ostream &operator<<(std::ostream &s, MDfragmentSFGD &df);

#endif
