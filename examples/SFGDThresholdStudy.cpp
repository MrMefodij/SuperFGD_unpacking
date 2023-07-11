//
// Created by amefodev on 06.07.2023.
//
#include <string>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include <boost/filesystem.hpp>

#include "MDargumentHandler.h"
#include "ToaEventDummy.h"
#include "Files_Reader.h"

namespace fs = boost::filesystem;

int main( int argc, char **argv ) {
    std::string stringBuf;

    // The following shows how to use the MDargumentHandler class
    // to deal with the main arguments
    // Define the arguments
    MDargumentHandler argh("Example of unpacking application.");
    argh.Init();

    if (argh.ProcessArguments(argc, argv)) {
        argh.Usage();
        return -1;
    }

    // Treat arguments, obtain values to be used later
    if (argh.GetValue("help")) {
        argh.Usage();
        return 0;
    }

    if (argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    if (argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK) return -1;

    std::vector<ToaEventDummy>* FEBs[SFGD_FEBS_NUM];
    for (Int_t i=0;i<SFGD_FEBS_NUM;++i) {
        FEBs[i]=0;
    }
    vector<string> vFileNames  = argh.GetDataFiles(stringBuf,".root","plots");

    if ( vFileNames.size() == 0 ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    for(auto fileName : vFileNames) {
        TFile *FileInput = new TFile((fileName).c_str());
        if (!FileInput->IsOpen()) {
            cerr << "Can not open file " << fileName << endl;
            return 1;
        }

        TTree *AllEvents = (TTree *) FileInput->Get("AllEvents");
        ostringstream sFEBnum;
        string sFEB;
        for (int ih = 0; ih < SFGD_FEBS_NUM; ++ih) {
            sFEBnum.str("");
            sFEBnum << ih;
            sFEB = "FEB_" + sFEBnum.str();
            AllEvents->SetBranchAddress((sFEB).c_str(), &FEBs[ih]);
        }
        int nEntries = AllEvents->GetEntries();
        std::cout << "entries at Feb  is " << nEntries << std::endl;

        FileInput->Close();
    }
    for (int i = 0; i < SFGD_FEBS_NUM; ++i) {
        delete FEBs[i];
    }
}