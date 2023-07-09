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

#ifndef __MDPARTEVENT_SFGD_H
#define __MDPARTEVENT_SFGD_H


#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <iostream>

#include "MDdataContainer.h"
#include "MDdataWordSFGD.h"

#define SFGD_FEB_NCHANNELS 256
#define SFGD_FEBS_NUM 3

class MDpartEventSFGD : public MDdataContainer {

 public:

  MDpartEventSFGD(void *d = 0 , unsigned int* time = 0, unsigned int* tag = 0);
  virtual ~MDpartEventSFGD() {}


  void SetDataPtr(void *d, uint32_t aSize=0);
  void Dump();
  void Init();

  void AddTimeHit(MDdataWordSFGD &dw);
  void AddAmplitudeHit(MDdataWordSFGD &dw);

  unsigned int GetTriggerTime()                                     { return _gtsTime; }
  unsigned int GetNLeadingEdgeHits(unsigned int ich)                { return _nLeadingEdgeHits[ich]; }
  unsigned int GetNTrailingEdgeHits(unsigned int ich)               { return _nTrailingEdgeHits[ich]; }
  unsigned int GetHitTime(unsigned int ih, unsigned int ich, char t);
  unsigned int GetHitTimeId(unsigned int ih, unsigned int ich, char t);

  unsigned int GetLeadingTime(unsigned int ih, unsigned int ich)    { return GetHitTime(ih, ich, 'l'); }
  unsigned int GetTrailingTime(unsigned int ih, unsigned int ich)   { return GetHitTime(ih, ich, 't'); }
  unsigned int GetLeadingTimeId(unsigned int ih, unsigned int ich)  { return GetHitTimeId(ih, ich, 'l'); }
  unsigned int GetTrailingTimeId(unsigned int ih, unsigned int ich) { return GetHitTimeId(ih, ich, 't'); }

  unsigned int GetHitAmplitude(unsigned int ich, char t);
  unsigned int GetHitAmplitudeId(unsigned int ich, char t);
  bool         LGAmplitudeHitExists(unsigned int ich)               {return _lgHit[ich];}
  bool         HGAmplitudeHitExists(unsigned int ich)               {return _hgHit[ich];}

  unsigned int GetTriggerTag()                                      {return _gtsTag;}
  unsigned int GetTriggerTagId()                                    {return _gtsTagId;}
  
  std::vector<unsigned int> GetLeadingTimes(unsigned int ich)       {return _leadingEdgeHitTime[ich]; }
  std::vector<unsigned int> GetTrailingTimes(unsigned int ich)      {return _trailingEdgeHitTime[ich]; }

  unsigned int getNumDataWords() {return _nDataWords;}

  void SetTriggerEvents(std::vector <MDpartEventSFGD*> *te)         {_trigEvents = te;}

  bool GateTrailerExist()                                         {return _gateTrailerExist;}

private:

    unsigned int _gtsTime;
    unsigned int _gtsTag;
    unsigned int _gtsTagId;
//    unsigned int _gateHeaderNumber;
//    unsigned int _gateHeaderBoardID;

    bool _lgHit[SFGD_FEB_NCHANNELS];
    bool _hgHit[SFGD_FEB_NCHANNELS];

//    bool _spillHeaderA;

    unsigned int _lgHitAmplitude[SFGD_FEB_NCHANNELS];
    unsigned int _hgHitAmplitude[SFGD_FEB_NCHANNELS];
    unsigned int _lgHitAmplitudeId[SFGD_FEB_NCHANNELS];
    unsigned int _hgHitAmplitudeId[SFGD_FEB_NCHANNELS];

    unsigned int _nLeadingEdgeHits[SFGD_FEB_NCHANNELS];  /** Number of leading edge hits per channel. */
    unsigned int _nTrailingEdgeHits[SFGD_FEB_NCHANNELS]; /** Number of trailing edge hits per channell.*/

    std::vector<unsigned int>  _leadingEdgeHitTime[SFGD_FEB_NCHANNELS];      /// A vector of leading edge hit timess per channel
    std::vector<unsigned int>  _trailingEdgeHitTime[SFGD_FEB_NCHANNELS];     /// A vector of trailing edge hit times per channel
    std::vector<unsigned int>  _leadingEdgeHitId[SFGD_FEB_NCHANNELS];        /// A vector of leading edge hit ids per channel
    std::vector<unsigned int>  _trailingEdgeHitId[SFGD_FEB_NCHANNELS];       /// A vector of trailing edge hit ids per channel

    unsigned int _nDataWords;

    std::vector <MDpartEventSFGD*> * _trigEvents;
    unsigned int* _previousTrTime;
    unsigned int* _previousTrTag;

    bool _gateTrailerExist = false;

    friend ostream &operator<<(std::ostream &s, MDpartEventSFGD &df);
};

#endif
