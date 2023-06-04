#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <TCanvas.h>
#include <TTree.h>
#include <TF1.h>
#include <TMath.h>
#include <TROOT.h>
#include <TLegend.h>

//#include "Peaks.h"
#include "Calibration.h"
#include "Files_Reader.h"
#include <map>
using namespace std;



int main(int argc, char **argv){
    // Проверяем, что файл открылся
    ifstream fList("febs_files_list.list");
    if (!fList){cout<< "Error opening input file"<< endl;}
    
    // Create root file
    string sFileName;
    getline(fList,sFileName);
    string rootFileOutput=GetLocation(sFileName.c_str());
    rootFileOutput+="_channels_signal.root";
    TFile wfile = TFile(rootFileOutput.c_str(), "RECREATE");
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);
    int j = 0;
    map<string,vector<Peaks>> data;
    //TTree *tCalibSFGD = new TTree("calibSFGD","calibSFGD");
    // Going through data file
    fList.seekg(0);
    while(getline(fList,sFileName)){
        cout <<sFileName<<endl;
        Int_t channels_num = 256;
        TH1F *hFEBCH[channels_num];
        File_Reader file_reader;
        string gainFileOutput = file_reader.Read_MDdataWordSFGD(sFileName);
        file_reader.ReadFile(sFileName);
        //Int_t FEB = file_reader.Fill_FEB(); //unused variable
        //TDirectory *FEBdir = file_reader.Create_directory(wfile);
        for (Int_t iCh=0; iCh<channels_num; iCh++) {
            hFEBCH[iCh] = file_reader.Print_hFEBCH(iCh);
            Calibration cl;
            hFEBCH[iCh] =  cl.SFGD_Calibration(hFEBCH[iCh]);
            hFEBCH[iCh]->Draw();
            hFEBCH[iCh]->GetYaxis()->SetTitle("Number of events");
            hFEBCH[iCh]->GetXaxis()->SetTitle("HG ADC channels");
            string channel = "Channel_" +  to_string(iCh) +'_' + to_string(j);
            data[channel] = cl.Calibration_Par();
            hFEBCH[iCh]->SetStats(0);
            // write legend
            if(cl.Calibration_Gain_Error() == cl.Calibration_Gain_Error() ){
                auto* legend = new TLegend(0.5,0.6,0.9,0.9);
                string header = "peaks found: " + to_string(data[channel].size());
                legend->SetHeader(header.c_str());
                legend -> SetFillColor(0);
                for(auto i = 0; i < data[channel].size();i++){
                    string name = "mean peak " + to_string(i) + /*" p. e."*/+" = " + 
                        to_string(data[channel][i].GetPosition()) + " +/- " + 
                        to_string(data[channel][i].GetPositionError());
                    legend->AddEntry((TObject*)0, name.c_str(), "");
                }
                string gain = "gain = " + to_string(cl.Calibration_Gain())
                    + " +/- " + to_string(cl.Calibration_Gain_Error());
                legend->AddEntry((TObject*)0, gain.c_str(), "");
                legend->Draw();
            }else {
                cout <<"Problem in channel: "<<iCh<< endl;
            }
            c1->Update();
            c1->Write(channel.c_str()); 
            delete hFEBCH[iCh];
        }
        ++j;
    }   
    fList.close();


    return 0;
}
