#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <TCanvas.h>
#include <TTree.h>
#include <TF1.h>
#include <TMath.h>
#include <TROOT.h>
#include "Calibration.h"
#include "Files_Reader.h"
#include "MDpartEventSFGD.h"
#include "MDargumentHandler.h"
#include <map>
using namespace std;



int main(int argc, char **argv){
    ifstream fList("febs_files_list.list");
    if (!fList){cout<< "Error opening input file"<< endl;}
    
    // Create root file

    string stringBuf;
    // The following shows how to use the MDargumentHandler class
    // to deal with the main arguments
    // Define the arguments
    MDargumentHandler argh("Example of sfgd calibration.");
    argh.AddArgument("help","print this message","h");
    argh.AddArgument("directory","Path for a data file","d","<string>","." );
    argh.AddArgument("file","Name of a data file","f","<string>","mandatory");

    // Check the user arguments consistancy
    // All mandatory arguments should be provided and
    // There should be no extra arguments
    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}

    // Treat arguments, obtain values to be used later
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}

    if ( argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
    string filename = stringBuf;

    std::ifstream ifs((filename).c_str());
    if ( ifs.fail() ) {
        cerr << "Can not open file " << filename << endl;
        return 1;
    }

    string rootFileOutput=GetLocation(filename.c_str(), ".bin");
    rootFileOutput+="_channels_signal.root";
    cout << rootFileOutput<<endl;

    TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);
    // Going through data file
    fList.seekg(0);
    Calibration cl;
    File_Reader file_reader;
    file_reader.ReadFile(filename);
    
    // find numbers of measured FEB
    set<Int_t> NFEB = file_reader.GetFEBNumbers();
    
    // get histograms with peaks
    for(const int &ih : NFEB){
        TDirectory *FEBdir = wfile->mkdir(("FEB_"+to_string(ih)).c_str());;
        FEBdir->cd();
        for (Int_t iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            TH1F* hFEBCH = file_reader.Get_hFEBCH(ih,iCh);
            string feb_channel = "FEB_" + to_string(ih) + "_Channel_" +  to_string(iCh);
            hFEBCH =  cl.SFGD_Calibration(hFEBCH, feb_channel);
            auto *legend = cl.Calibration_Legend();
            legend->Draw();
            c1->Update();
            c1->Write(feb_channel.c_str());
            delete hFEBCH;
        }
    } 
  
    // find channels with std more than 3 sigma 
    map<string,Double_t> gain = cl.GetGain();
    string connection;
    TH1F* hGain = new TH1F("Gain_distrubution", "Gain_distrubution",  101, 0, 100);
    for (auto g : gain) {
        hGain->Fill(g.second);
    }
    TF1 * fit = new TF1("fit","gaus");
    // adjust the fitting boundaries
    hGain->Fit("fit","","",10,24);
    Double_t mean_gain = fit->GetParameter(1);
    Double_t std_gain = fit->GetParameter(2);
    wfile->cd();
    hGain->Write();
    cout << "Mean_gain: "<<mean_gain<<", std_gain: "<<std_gain<<endl;

    for(auto ih : NFEB){   
        for (auto iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            connection = "FEB_" + to_string(ih) + "_Channel_" +  to_string(iCh);
            double gain_value = gain.at(connection);
            if((gain_value < mean_gain - 3*std_gain) || (gain_value > mean_gain + 3*std_gain))
                 if(gain_value == 0) cout<< "Problem in " << connection <<" number of peaks less than 2"<<endl;
          
                   else{ cout << "Problem in "<<connection <<": "<< gain_value <<endl;}
        }
    }

    wfile->Close();
    fList.close();
    return 0;
}
