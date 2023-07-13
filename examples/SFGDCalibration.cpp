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
    
    // Create root file

    string stringBuf;
    MDargumentHandler argh("Example of sfgd baseline.");
    argh.Init();


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
    Calibration cl;
    File_Reader file_reader;
    vector<TH1F*> hFEBCH(SFGD_FEBS_NUM*SFGD_FEB_NCHANNELS);
    for(int i = 0; i < SFGD_FEBS_NUM; i++){
        for(int j = 0; j < SFGD_FEB_NCHANNELS; j++){
            std::string sCh = "FEB_"+std::to_string(i)+"_Channel_"+std::to_string(j);
            hFEBCH[SFGD_FEB_NCHANNELS*i + j] = new TH1F(sCh.c_str(),sCh.c_str(),  701, 0, 700);
        }
    }
    file_reader.ReadFile(filename, hFEBCH,2);
    
    // find numbers of measured FEB
    set<unsigned int> NFEB = file_reader.GetFEBNumbers();
    // get histograms with peaks
    for(const unsigned int &ih : NFEB){
        TDirectory *FEBdir = wfile->mkdir(("FEB_"+to_string(ih)).c_str());;
        FEBdir->cd();
        for (unsigned int iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            string feb_channel = "FEB_" + to_string(ih) + "_Channel_" +  to_string(iCh);
            hFEBCH[SFGD_FEB_NCHANNELS*ih + iCh]=  cl.SFGD_Calibration(hFEBCH[SFGD_FEB_NCHANNELS*ih + iCh], feb_channel);
            auto *legend = cl.Calibration_Legend();
            legend->Draw();
            c1->Update();
            c1->Write(feb_channel.c_str());
            delete hFEBCH[SFGD_FEB_NCHANNELS*ih + iCh];
        }
    }

    // find channels with std more than 3 sigma 
    map<string,double> gain = cl.GetGain();
    string connection;
    TH1F* hGain = new TH1F("Gain_distrubution", "Gain_distrubution",  101, 0, 100);
    for (auto g : gain) {
        if (g.second != 0) hGain->Fill(g.second);
    }
    TF1 * fit = new TF1("fit","gaus");
    // adjust the fitting boundaries
    hGain->Fit("fit","","");
    double mean_gain = fit->GetParameter(1);
    double std_gain = fit->GetParameter(2);
    wfile->cd();
    hGain->Write();
    cout << "Mean_gain: "<<mean_gain<<", std_gain: "<<std_gain<<endl;

    for(auto ih : NFEB){   
        for (auto iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            connection = "FEB_" + to_string(ih) + "_Channel_" +  to_string(iCh);
            double gain_value = gain.at(connection);
            if((gain_value < mean_gain - 3*std_gain) || (gain_value > mean_gain + 3*std_gain)){
                if(gain_value == 0) cout<< "Problem in " << connection <<" number of fitted peaks less than 3"<<endl;
                else{ cout << "Problem in "<<connection <<": "<< gain_value <<endl;}
            }
        }
    }
//    for(auto i = 0; i < hFEBCH.size(); i++)
//        delete hFEBCH[i];
    wfile->Close();
    return 0;
}
