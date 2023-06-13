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

////////
// 
// 
// 
// Проверить, что кол-во событий (высота пиков) определяется параметром 1
// 
// 
// 
////////


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
    map<string,vector<Peaks>> data;
    // Going through data file
    fList.seekg(0);
    while(getline(fList,sFileName)){
        cout <<sFileName<<endl;
        File_Reader file_reader;
        file_reader.ReadFile(sFileName);
        set<Int_t> NFEB = file_reader.GetFEBNumbers();
        
        for(const int &ih : NFEB){
            cout << ih<<endl;
            TDirectory *FEBdir = wfile->mkdir(("FEB_"+to_string(ih)).c_str());;
            FEBdir->cd();
            for (Int_t iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
                TH1F* hFEBCH = file_reader.Get_hFEBCH(ih,iCh);
                Calibration cl;
                hFEBCH =  cl.SFGD_Calibration(hFEBCH);
                // hFEBCH->Draw();
                string feb_channel = "FEB_" + to_string(ih) + "_Channel_" +  to_string(iCh);
                data[feb_channel] = cl.Calibration_Par();
                // hFEBCH->SetStats(0);
            // write legend
                if(cl.Calibration_Gain_Error() == cl.Calibration_Gain_Error() ){
                    auto *legend = cl.Calibration_Legend();
                    legend->Draw();
                }else cout <<"Problem in FEB "<<ih<<" channel "<<iCh<< endl;
                c1->Update();
                c1->Write(feb_channel.c_str());
                delete hFEBCH;
            }
        } 
       wfile->Close();
    }  
    fList.close();
    return 0;
}
