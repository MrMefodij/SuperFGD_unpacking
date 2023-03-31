#include <exception>
#include <vector>
#include "TFile.h"
#include "TH1D.h"
#include "TSystem.h"
#include <TTree.h>
#include "MDfragmentSFGD.h"
#include "MDpartEventSFGD.h"
#include "MDargumentHandler.h"
#include "MDdataFile.h"

using namespace std;

string GetLocation(string str){
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

    // Treat arguments, obtain values to be used later
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}
  if ( argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
  filepath = stringBuf;
  if ( argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
  filename = stringBuf;
  filename = sFileName;
  string rootFilename = sFileName;

  
  MDdateFile dfile(filename);
  char * eventBuffer;
  uint32_t gtsTagBeforeSpill;
  bool _previousSpillTagExist = false;
  unsigned int _previousSpillTag = 0;
  if ( dfile.open() ) { // There is a valid files to unpack
    dfile.init();

    int xEv(0);
    do { // Loop over all spills
      eventBuffer =  dfile.GetNextEvent(gtsTagBeforeSpill);
//      cout << gtsTagBeforeSpill <<endl;x
      try {
        MDfragmentSFGD   spill;
       
        spill.SetPreviousSpill(_previousSpillTagExist,_previousSpillTag);
        spill.SetDataPtr(eventBuffer, gtsTagBeforeSpill);
        MDpartEventSFGD *event;
        int nTr = spill.GetNumOfTriggers();
        
        for (int i=0; i<nTr; ++i) {
          event = spill.GetTriggerEventPtr(i);
          event->Dump();
          // if (spill.GetGateNumber()!=156){
        }
      } catch (MDexception & lExc)  {
        std::cerr <<  lExc.GetDescription() << endl
                  << "Unpacking exception\n"
                  << "Spill skipped!\n\n";

      } catch(std::exception & lExc) {
        std::cerr << lExc.what() << std::endl
                  << "Standard exception\n"
                  << "Spill skipped!\n\n";
      } catch(...) {
        std::cerr << "Unknown exception occurred...\n"
                  << "Spill skipped!\n\n";
      }

      ++xEv;
       } while (xEv < 500);
    //} while ( eventBuffer );
  }
    
  dfile.close();
  delete dataBuff;
  }
  rfile.Close();
  fList.close();
  return 0;
}