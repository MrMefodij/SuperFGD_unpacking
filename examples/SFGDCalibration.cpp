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
#include <map>
using namespace std;



int main(int argc, char **argv){
    // Проверяем, что файл открылся
    ifstream fList("febs_files_list.list");
    if (!fList){cout<< "Error opening input file"<< endl;}
    
    // Create root file
    string sFileName;
    string rootFileOutput="Channels_signal.root";
    TFile wfile(rootFileOutput.c_str(), "recreate");
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);
    int j = 0;
    map<string,vector<Peaks>> data;
    // Going through data file
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
            Int_t hg = (int)(cl.Calibration_Gain());
            if(hg > 0){
                cout << channel <<" hg= "<<(int)(cl.Calibration_Gain())<<" error: "<<cl.Calibration_Gain_Error()<<endl;
            }
            c1->Update();
            hFEBCH[iCh]->Write(); 
            delete hFEBCH[iCh];
        }
        ++j;
    }
        fList.close();


    return 0;
}

//fList.seekg(0); // возврат к началу файла
