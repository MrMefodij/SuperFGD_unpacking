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
#include "Connection_Map.h"
#include "ToaEventDummy.h"

#define SFGD_FEBS_NUM 280

using namespace std;

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
    rootFileOutput+="_plots.root";
    cout << rootFileOutput<<endl;

    TFile rfile(rootFileOutput.c_str(), "recreate");
    std::vector<ToaEventDummy> FEBs[SFGD_FEBS_NUM];
    for (Int_t i=0;i<SFGD_FEBS_NUM;++i) {
        FEBs[i].clear();
    }

    string mapFile = "../connection_map/map.txt";
    Connection_Map map(mapFile);
    try {
       map.Init();
    } catch (const exception& e) {
        cerr << "Unable to open file " << mapFile << endl;
        exit(1);
    }


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

        TH1I  h_lgah("h_lg_amp_hit_ch", "hit channels", SFGD_FEB_NCHANNELS, 0, SFGD_FEB_NCHANNELS);
        TH1I  h_hgah("h_lh_amp_hit_ch", "hit channels", SFGD_FEB_NCHANNELS, 0, SFGD_FEB_NCHANNELS);
        TH1I  h_lth("h_le_time_hit_ch", "hit channels", SFGD_FEB_NCHANNELS, 0, SFGD_FEB_NCHANNELS);
        TH1I  h_tth("h_te_time_hit_ch", "hit channels", SFGD_FEB_NCHANNELS, 0, SFGD_FEB_NCHANNELS);
        TH1I  h_lga("h_lg_ampl", "hit ampl.", 200, 0, 5000);
        TH1I  h_hga("h_hg_ampl", "hit ampl.", 200, 0, 5000);

        MDdateFile dfile(filename);
// Open the file and loop over events.
        unsigned int BordID=0;
        char *eventBuffer;
        bool _previousSpillTagExist = false;
        unsigned int _previousSpillTag = 0;
        if ( dfile.open() ) { // There is a valid files to unpack
            dfile.init();
            int xEv(0);
            do { // Loop over all spills
                eventBuffer =  dfile.GetNextEvent();
                try {
                    MDfragmentSFGD  spill;
                    spill.SetPreviousSpill(_previousSpillTagExist,_previousSpillTag);
                    spill.SetDataPtr(eventBuffer);
                    int nTr = spill.GetNumOfTriggers();
//                    cout << "nTr : " << nTr << endl;
                    BordID = spill.GetBoardId();
                    ToaEventDummy temp(0, BordID, spill.GetGateNumber(),
                    spill.GetGateTime(), spill.GetGateTimeFrGTS(), spill.GetGateTrailTime());

                    for (int i=0; i<nTr; ++i) {
                        MDpartEventSFGD *trEv = spill.GetTriggerEventPtr(i);
//                        event->Dump();
                        for (int ich=0; ich<SFGD_FEB_NCHANNELS; ++ich) {
                            int nHits = trEv->GetNLeadingEdgeHits(ich);
                            if (nHits)
                                h_lth.Fill(ich, nHits);

                            nHits = trEv->GetNTrailingEdgeHits(ich);
                            if (nHits)
                                h_tth.Fill(ich, nHits);

                            if (trEv->LGAmplitudeHitExists(ich)) {
                                int q = trEv->GetHitAmplitude(ich, 'l');
                                h_lga.Fill(q);
                                h_lgah.Fill(ich);
                            }

                            if (trEv->HGAmplitudeHitExists(ich)) {
                                int q = trEv->GetHitAmplitude(ich, 'h');
                                h_hga.Fill(q);
                                h_hgah.Fill(ich);
                            }
                        }
                    }
                    FEBs[BordID].push_back(std::move(temp));
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

        TTree* FEBtree[SFGD_FEBS_NUM];
        ostringstream sFEBnum;
        string sFEB;
//        gInterpreter->GenerateDictionary("vector<ToaEventDummy>", "../oaEventDummy/ToaEventDummy.h");
//        gInterpreter->GenerateDictionary("ToaEventDummy", "../oaEventDummy/ToaEventDummy.h");
//        gInterpreter->GenerateDictionary("vector<ToaEventDummy>", "vector");
        for (Int_t ih=0; ih<SFGD_FEBS_NUM; ih++) {
            if (!FEBs[ih].empty()) {
                sFEBnum.str("");
                sFEBnum << ih;
                sFEB = "FEB_" + sFEBnum.str();
                FEBtree[ih] = new TTree(sFEB.c_str(), sFEB.c_str());
                FEBtree[ih]->Branch((sFEB).c_str(),"vector<ToaEventDummy>",&FEBs[ih]);
                FEBtree[ih]->Write("", TObject::kOverwrite);
            }
        }

        h_lgah.Write();
        h_hgah.Write();
        h_lth.Write();
        h_tth.Write();
        h_lga.Write();
        h_hga.Write();
        dfile.close();
        delete dataBuff;
    }

    rfile.Close();
    fList.close();
    return 0;
}
