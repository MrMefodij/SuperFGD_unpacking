#include <iostream>
#include <fstream>

#include <TCanvas.h>
#include <TTree.h>
#include <TF1.h>
#include <TROOT.h>
#include "Calibration.h"
#include "Files_Reader.h"
#include "SFGD_defines.h"
#include "MDargumentHandler.h"
#include <map>


int main(int argc, char **argv){

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

    /// Create root file
    string rootFileOutput=GetLocation(filename.c_str(), ".bin");
    rootFileOutput+="_channels_signal.root";
    cout << rootFileOutput<<endl;

    TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);

    /// Create TH1F for each SFGD_FEB_NCHANNELS in SFGD_SLOT
    vector<vector<TH1F*>> hFEBCH (SFGD_SLOT, vector<TH1F*>(SFGD_FEB_NCHANNELS));
    for(int i = 0; i < SFGD_SLOT; i++){
        for(int j = 0; j < SFGD_FEB_NCHANNELS; j++){
            std::string sCh = "Slot_"+std::to_string(i)+"_Channel_"+std::to_string(j);
            hFEBCH[i][j] = new TH1F(sCh.c_str(),sCh.c_str(),  701, 0, 700);
        }
    }

    /// Going through data file
    Calibration cl;
    File_Reader file_reader;

    /// parameter HG_LG: for HG - 2, for LG - 3, default - 2
    file_reader.ReadFile_for_Calibration(filename, hFEBCH,2);
    
    /// Find numbers of measured FEB
    set<unsigned int> NFEB = file_reader.GetFEBNumbers();

    /// Get histograms with peaks
    for(const unsigned int &ih : NFEB){
        TDirectory *FEBdir = wfile->mkdir(("FEB_"+to_string(ih)).c_str());
        FEBdir->cd();
        for (unsigned int iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
            unsigned int slot_id = ih & 0x0f;
            string feb_channel = "FEB_" + to_string(ih) + "_Channel_" + to_string(iCh);
            cl.SFGD_Calibration(hFEBCH[slot_id][iCh], feb_channel);
            if(hFEBCH[slot_id][iCh]->GetEntries() > 0) {
                auto *legend = cl.Calibration_Legend();
                legend->Draw();
                c1->Update();
                c1->Write(feb_channel.c_str());
            }
            delete hFEBCH[slot_id][iCh];
        }
    }

    /// Find channels with std more than 3 sigma
    map<string,double> gain = cl.GetGain();
    string connection;
    TH1F* hGain = new TH1F("Gain_distrubution", "Gain_distrubution",  400, 0, 100);
    for (auto g : gain) {
        if (g.second != 0) hGain->Fill(g.second);
    }
    TF1 * fit = new TF1("fit","gaus");

    /// Adjust the fitting boundaries
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
                if(gain_value == 0) cout<< "Problem in " << connection <<" number of fitted peaks less than 4"<<endl;
                else{ cout << "Problem in "<<connection <<": "<< gain_value <<endl;}
            }
        }
    }

    wfile->Close();
    return 0;
}
