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
 * along with BabyMINDdaq.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//
// Created by amefodev on 13.06.2023. mrmefodij@gmail.com
//

#include <exception>
#include <fstream>
#include <vector>
#include <TTree.h>
#include <TFile.h>
#include <TSystem.h>
#include <TMacro.h>
#include "MDfragmentSFGD.h"
#include "MDpartEventSFGD.h"
#include "MDargumentHandler.h"
#include "MDdataFile.h"
#include "Files_Reader.h"
#include "ToaEventDummy.h"
#include <iostream>

using namespace std;


int main( int argc, char **argv ) {
    string stringBuf;
    int    intBuf;

    // The following shows how to use the MDargumentHandler class
    // to deal with the main arguments
    // Define the arguments
    MDargumentHandler argh("Example of unpacking application.");
    argh.AddArgument("help","print this message","h");
    argh.AddArgument("directory","Path for a data file","d","<string>","." );
    argh.AddArgument("file","Name of a data file","f","<string>","mandatory");
    argh.AddArgument("begin","Initial position in input file","b","<int>","0");
    argh.AddArgument("nwords","Number of data words to be processed","n","<int>","0");

    // Check the user arguments consistancy
    // All mandatory arguments should be provided and
    // There should be no extra arguments
    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}

    // Treat arguments, obtain values to be used later
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}

    if ( argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
    string filename = stringBuf;

    if ( argh.GetValue("begin", intBuf) != MDARGUMENT_STATUS_OK ) return -1;
    int pos = intBuf*4;

    if ( argh.GetValue("nwords", intBuf) != MDARGUMENT_STATUS_OK ) return -1;
    int nWords = intBuf;

    std::ifstream ifs((filename).c_str());
    if ( ifs.fail() ) {
        cerr << "Can not open file " << filename << endl;
        return 1;
    }

    string rootFileOutput=GetLocation(filename.c_str(), ".bin");
    rootFileOutput+="_plots.root";
    cout << rootFileOutput<<endl;

    TFile rfile(rootFileOutput.c_str(), "recreate");
    std::vector<ToaEventDummy> FEBs[SFGD_FEBS_NUM];
    for (Int_t i=0;i<SFGD_FEBS_NUM;++i) {
        FEBs[i].clear();
    }

    ostringstream sFEBnum;
    string sFEB;

    TTree AllEvents("AllEvents", "The ROOT tree of events");
    for (int ih = 0; ih < SFGD_FEBS_NUM; ++ih) {
        sFEBnum.str("");
        sFEBnum << ih;
        sFEB = "FEB_"+sFEBnum.str();
        AllEvents.Branch((sFEB).c_str(), "std::vector<ToaEventDummy>", &FEBs[ih]);
    }
    MDdateFile dfile(filename);
// Open the file and loop over events.
    unsigned int BordID=0;
    char *eventBuffer;
    unsigned int ocbEventNumber = 0;
    bool firstEventNumber = true;
    if ( dfile.open() ) { // There is a valid files to unpack
        dfile.init();
        do { // Loop over all spills
            eventBuffer =  dfile.GetNextEvent();

            if ( dfile.GetOcbEventNumber() != ocbEventNumber  && !firstEventNumber){
                AllEvents.Fill();
                for (int i = 0; i < SFGD_FEBS_NUM; ++i) {
                    FEBs[i].clear();
                }
            } else{
                firstEventNumber = false;
            }
            ocbEventNumber =  dfile.GetOcbEventNumber();
            cout << "OCB Event Number: " << ocbEventNumber << endl;

            try {
                MDfragmentSFGD  spill;
                spill.SetDataPtr(eventBuffer);
                int nTr = spill.GetNumOfTriggers();
                BordID = spill.GetBoardId();
                ToaEventDummy event(ocbEventNumber, BordID, spill.GetGateNumber(),
                    spill.GetGateTime(), spill.GetGateTimeFrGTS(), spill.GetGateTrailTime());

                for (int i=0; i<nTr; ++i) {
                    MDpartEventSFGD *trEv = spill.GetTriggerEventPtr(i);
                    unsigned int gtsTime = trEv->GetTriggerTime();
//                        trEv->Dump();
                    for (int ich=0; ich<SFGD_FEB_NCHANNELS; ++ich) {
                        unsigned int nlHits = 0;
                        nlHits = trEv->GetNLeadingEdgeHits(ich);
                        for (unsigned int ih=0; ih<nlHits; ++ih) {
                            bool trailTimeExist = false;
                            unsigned int hitId = trEv->GetHitTimeId(ih, ich, 'l');
                            unsigned int hitLeadTime =  trEv->GetHitTime(ih, ich, 'l');
                            unsigned int hitTrailTime = 0;
                            unsigned int ntHits = trEv->GetNTrailingEdgeHits(ich);

                            for(unsigned int ith=0; ith < ntHits; ith++){
                                if (hitId==trEv->GetHitTimeId(ith, ich, 't') && trEv->GetTrailingTime(ith, ich) >= hitLeadTime){
                                    hitTrailTime = trEv->GetTrailingTime(ith, ich);
                                    trailTimeExist = true;
                                }
                            }
                            if (trailTimeExist == false){
                                if (i+1 <nTr){
                                    trEv = spill.GetTriggerEventPtr(i+1);
                                    ntHits = trEv->GetNTrailingEdgeHits(ich);
                                    unsigned int ith=0;
                                    while (trailTimeExist == false && ith < ntHits){
                                        if (hitId==trEv->GetHitTimeId(ith, ich, 't')){
                                            hitTrailTime = trEv->GetTrailingTime(ith, ich) + 4000;
                                            trailTimeExist = true;
                                        }
                                        ith++;
                                    }
                                    trEv = spill.GetTriggerEventPtr(i);
                                }
                            }
//                                cout << ich << " " << hitLeadTime << " " << hitTrailTime << " ";
                            bool hgAmplExist = false;
                            unsigned int hitHgADC = 0;

                            if (hitId == trEv->GetHitAmplitudeId(ich, 'h') ){
                                hitHgADC = trEv->GetHitAmplitude(ich, 'h');
                                hgAmplExist = true;
                            }
                            if (hgAmplExist == false){
                                if (i+1 <nTr){
                                    trEv = spill.GetTriggerEventPtr(i+1);
                                    if (trEv->HGAmplitudeHitExists(ich)){
                                        if (hitId == trEv-> GetHitAmplitudeId(ich, 'h')){
                                            hitHgADC = trEv->GetHitAmplitude(ich, 'h');
                                            hgAmplExist = true;
                                        }
                                    }
                                    trEv = spill.GetTriggerEventPtr(i);
                                }
                            }
//                                cout << hitHgADC << " " ;
                            bool lgAmplExist = false;
                            unsigned int hitLgADC = 0;

                            if (hitId==trEv->GetHitAmplitudeId(ich, 'l')){
                                hitLgADC = trEv->GetHitAmplitude(ich, 'l');
                                lgAmplExist = true;
                            }
                            if ( lgAmplExist == false){
                                if (i+1 <nTr){
                                    trEv = spill.GetTriggerEventPtr(i+1);
                                    if (trEv->LGAmplitudeHitExists(ich)){
                                        if (hitId==trEv->GetHitAmplitudeId(ich, 'l')){
                                            hitLgADC = trEv->GetHitAmplitude(ich, 'l');
                                            lgAmplExist = true;
                                        }
                                    }
                                    trEv = spill.GetTriggerEventPtr(i);
                                }
                            }
//                                cout << hitLgADC << " " << endl;
                            event.SetOaEvent(TSFGDigit(gtsTime, ich, hitLeadTime, trailTimeExist, hitTrailTime,
                                                           hitHgADC,hitLgADC,false));
                        }
                    }
                }
                cout << "Number of hits: " << event.GetNumberHits() << endl;
                FEBs[BordID].push_back(std::move(event));
//                    std::cout << "EventNumber: " <<FEBs[BordID].back().GetEventNumber() << "  BoardId: "<<FEBs[BordID].back().GetboardId() << " Gate Number: " <<
//                    FEBs[BordID].back().GetGateNumber() << " GateTime: " << FEBs[BordID].back().GetGateTime() << " GateTimeFromGTS: " <<
//                    FEBs[BordID].back().GetGateTimeFrGts() << " GateTrailTime: " << FEBs[BordID].back().GetGateTrailTime() <<std::endl;
            } catch (MDexception & lExc)  {
                std::cerr <<  lExc.GetDescription() << endl
                << "Unpacking exception\n"
                << "Spill skipped!\n\n"
                << "Here it should fill with negative numbers. \n\n";
            } catch(std::exception & lExc) {
                std::cerr << lExc.what() << std::endl
                << "Standard exception\n"
                << "Spill skipped!\n\n";
            } catch(...) {
                std::cerr << "Unknown exception occurred...\n"
                << "Spill skipped!\n\n";
            }
        } while ( eventBuffer );
    }
    FEBs[BordID].pop_back();

    for (Int_t ih=0; ih<SFGD_FEBS_NUM; ++ih) {
        if (!FEBs[ih].empty()) {
            cout<<"FEB_"<< ih << " size: " << FEBs[ih].size() <<endl;
        }
    }

    AllEvents.Write("",TObject::kOverwrite);
    dfile.close();
    rfile.Close();
    return 0;
}
