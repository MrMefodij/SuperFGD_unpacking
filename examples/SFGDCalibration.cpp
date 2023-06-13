#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <TCanvas.h>
#include <TTree.h>
#include <TF1.h>
#include <TMath.h>
#include <TROOT.h>

//#include "Peaks.h"
#include "Calibration.h"
#include "Files_Reader.h"
#include "MDpartEventSFGD.h"
#include <map>
using namespace std;



int main(int argc, char **argv){
    ifstream fList("febs_files_list.list");
    if (!fList){cout<< "Error opening input file"<< endl;}
    
    // Create root file
    string sFileName;
    getline(fList,sFileName);
    string rootFileOutput=GetLocation(sFileName.c_str());
    rootFileOutput+="_channels_signal.root";
    TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);
    // Going through data file
    fList.seekg(0);
    Calibration cl;
    while(getline(fList,sFileName)){
        // cout <<sFileName<<endl;
        File_Reader file_reader;
        file_reader.ReadFile(sFileName);
        set<Int_t> NFEB = file_reader.GetFEBNumbers();
        
        for(const int &ih : NFEB){
            // cout << ih<<endl;
            TDirectory *FEBdir = wfile->mkdir(("FEB_"+to_string(ih)).c_str());;
            FEBdir->cd();
            for (Int_t iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
                TH1F* hFEBCH = file_reader.Get_hFEBCH(ih,iCh);
                string feb_channel = "FEB_" + to_string(ih) + "_Channel_" +  to_string(iCh);
                hFEBCH =  cl.SFGD_Calibration(hFEBCH, feb_channel);
                if(cl.Calibration_Gain_Error() == cl.Calibration_Gain_Error() ){
                    auto *legend = cl.Calibration_Legend();
                    legend->Draw();
                }else cout <<"Problem in FEB "<<ih<<" channel "<<iCh<< endl;
                cl.Clear_Peaks();
                c1->Update();
                c1->Write(feb_channel.c_str());
                delete hFEBCH;
            }
        } 
    }  
    map<std::string,Double_t> gain = cl.GetGain();
    TH1F* hGain = new TH1F("Gain_distrubution","Gain_distrubution",  101, 0, 100);
    // TH1F* hGain = cl.Gain_Distrubution();
    for (auto it = gain.begin(); it != gain.end(); it++) {
       // cout << it->first<<endl;
       hGain->Fill(it->second);
    }
    TF1 * fit = new TF1("fit","gaus");
    hGain->Fit("fit","","",10,24);
    Double_t mean_gain = fit->GetParameter(1);
    Double_t std_gain = fit->GetParameter(2);
    cout << "Mean_gain: "<<mean_gain<<", std_gain: "<<std_gain<<endl;
    for (auto it = gain.begin(); it != gain.end(); it++) {
        if((it->second < mean_gain - 5*std_gain) || (it->second > mean_gain + 5*std_gain))
            cout << it->first <<": "<< it->second <<endl;
    }
    wfile->cd();
    hGain->Write();
    wfile->Close();
    fList.close();
    return 0;
}
