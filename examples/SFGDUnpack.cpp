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

#include <stdio.h>
#include <string.h>
#include <exception>
#include <fstream>
#include <vector>
#include "TFile.h"
#include "TDirectory.h"
#include "TH1D.h"
#include "TSystem.h"
#include "TMacro.h"
#include <TTree.h>
#include "MDfragmentSFGD.h"
#include "MDpartEventSFGD.h"
#include "MDargumentHandler.h"
#include "MDdataFile.h"

using namespace std;

string GetLocation(string str)
{

    int i = str.rfind("_Slot_");
    string way = str.substr(0,i);
    return way;
}

char *dataBuff;
uint32_t* dataPtr;

int main( int argc, char **argv ) {
    string sFileName;

    vector<string> vFileNames;
    ifstream fList("febs_files_list.list");
    while (!fList.eof()) {
        fList >> sFileName;
        //cout << sFileName << endl;
        vFileNames.push_back(sFileName);
    }
    vFileNames.pop_back();

    for (int i = 0; i< vFileNames.size();i++){
        cout << vFileNames.at(i)<<endl;
    }

    string rootFileOutput=GetLocation(vFileNames[0].c_str());
    rootFileOutput+="_all.root";
    cout << rootFileOutput<<endl;

    TFile rfile(rootFileOutput.c_str(), "recreate");

    TTree* FEBtree[65];

    ostringstream sFEBnum;
    string sFEB;


    bool firstSpillTagExist = false;
    unsigned int firstSpillTag = 0;

    for (vector<string>::iterator itFileName=vFileNames.begin(); itFileName != vFileNames.end(); itFileName++) {
        sFileName = *itFileName;
        cout <<endl<< sFileName << endl;

        ifstream finData(sFileName.c_str());
        string stringBuf;
        string filepath;
        string filename;

        // The following shows how to use the MDargumentHandler class
        // to deal with the main arguments
        // Define the arguments
        MDargumentHandler argh("Example of unpacking application.");
        argh.AddArgument("help","print this message","h");
        argh.AddArgument("directory","path for the data file","d","<string>","." );
        argh.AddArgument("file","Name of a data file","f","<string>","mandatory");


        // Check the user arguments consistancy
        // All mandatory arguments should be provided and
        // There should be no extra arguments
        //if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}

        // Treat arguments, obtain values to be used later
        if ( argh.GetValue("help") ) {argh.Usage(); return 0;}
        if ( argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
        filepath = stringBuf;
        if ( argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
        filename = stringBuf;
        filename = sFileName;
        string rootFilename = sFileName;


        MDdateFile dfile(filename);
// Open the file and loop over events.
        Int_t BordID=0;
        char *eventBuffer;
        uint32_t gtsTagBeforeSpill;
        bool _previousSpillTagExist = false;
        unsigned int _previousSpillTag = 0;
        if ( dfile.open() ) { // There is a valid files to unpack
            dfile.init();

            int xEv(0);
            do { // Loop over all spills
                eventBuffer =  dfile.GetNextEvent(gtsTagBeforeSpill);
                try {
                    MDfragmentSFGD  spill;

                    spill.SetPreviousSpill(_previousSpillTagExist,_previousSpillTag);
                    spill.SetDataPtr(eventBuffer);

                    MDpartEventSFGD *event;
                    int nTr = spill.GetNumOfTriggers();
//                    cout << "nTr : " << nTr << endl;
                    BordID = (Int_t)spill.GetBoardId();

                    for (int i=0; i<nTr; ++i) {
                        event = spill.GetTriggerEventPtr(i);
                        //event->Dump();
                        for (int ich=0; ich<SFGD_FEB_NCHANNELS; ++ich) {
                            int nlHits = 0;
                            int ntHits = 0;
                            nlHits = event->GetNLeadingEdgeHits(ich);
                            for (unsigned int ih=0; ih<nlHits; ++ih) {
                                bool TrailTimeExist = false;
                                int IDevent = event->GetHitTimeId(ih, ich, 'l');
                                ntHits = event->GetNTrailingEdgeHits(ich);

                            }
                        }
                    }
                } catch (MDexception & lExc)  {
                    std::cerr <<  lExc.GetDescription() << endl
                              << "Unpacking exception\n"
                              << "Spill skipped!\n\n"
                              << "Here it should fill with negative numbers. \n\n";
                    if (_previousSpillTag==0){
                        _previousSpillTag = firstSpillTag -1;
                    }
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

        FEBtree[BordID]-> Write("",TObject::kOverwrite);
        cout << "Number of spills on FEB "<< BordID<< ": "<< FEBtree[BordID]->GetEntries()<<endl;
        FEBtree[BordID]->Delete();

        dfile.close();
        delete dataBuff;
    }
    rfile.Close();
    fList.close();
    return 0;
}
