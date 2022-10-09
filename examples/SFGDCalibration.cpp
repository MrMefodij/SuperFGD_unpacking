#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>


#include <TCanvas.h>
#include <TTree.h>
#include <TRandom.h>
#include <TSpectrum.h>
#include <TVirtualFitter.h>
#include <TF1.h>
#include <TMath.h>
#include <TROOT.h>
#include<TParameter.h>

//#include "Peaks.h"

#include "Files_Reader.h"

using namespace std;

Int_t npeaks = 30;

Double_t fitf(Double_t *x,Double_t *par) {
   Double_t arg = 0;
   if (par[2]!=0) arg = (x[0] - par[1])/par[2];
   Double_t fitval = par[0]*TMath::Exp(-0.5*arg*arg);
   return fitval;
}

int main(int argc, char **argv){
    // Проверяем, что файл открылся
    ifstream fList("febs_files_list.txt");
    if (!fList){cout<< "Error opening input file"<< endl;}
    
    // Считываем название файла и создаем output файл root
    string sFileName;
    string rootFileOutput="Channels_signal.root";
    TFile wfile(rootFileOutput.c_str(), "recreate");
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);
    
    // начинаем проходится файлу, чтобы записать новый файл с данными
while(getline(fList,sFileName)){
    cout <<sFileName<<endl;
    Int_t channels_num = 256;
    TH1F *hFEBCH[channels_num];
    File_Reader file_reader;
    string gainFileOutput = file_reader.Read_MDdataWordSFGD(sFileName);
    file_reader.ReadFile(sFileName);
    Int_t FEB = file_reader.Fill_FEB();
    TDirectory *FEBdir = file_reader.Create_directory(wfile);
     

    for (Int_t iCh=0; iCh<channels_num; iCh++) {

        hFEBCH[iCh] = file_reader.Print_hFEBCH(iCh);
        TF1 *func = new TF1("fit",fitf,-3,3,3);
        func->SetParameters(500,hFEBCH[iCh]->GetMean(),hFEBCH[iCh]->GetRMS());
        func->SetParNames ("Constant","Mean_value","Sigma");
        hFEBCH[iCh]->Draw();
        hFEBCH[iCh]->Fit("fit","R");
        hFEBCH[iCh]->GetYaxis()->SetTitle("Number of events");
        hFEBCH[iCh]->GetXaxis()->SetTitle("HG ADC channels");
        hFEBCH[iCh]->Write();
        c1->Update();
        delete hFEBCH[iCh];
    }

}

    fList.close();


    return 0;
}

//fList.seekg(0); // возврат к началу файла
