#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>


#include <TCanvas.h>
#include <TTree.h>
#include <TRandom.h>
#include <TFractionFitter.h>
#include <TVirtualFitter.h>
#include <TF1.h>
#include <TMath.h>
#include <TROOT.h>
#include <TSpectrum.h>

//#include "Peaks.h"

#include "Files_Reader.h"

using namespace std;

Int_t npeaks = 30;

Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = par[0] + par[1]*x[0];
   for (Int_t p=0;p<npeaks;p++) {
      Double_t norm  = par[3*p+2]; // "height" or "area"
      Double_t mean  = par[3*p+3];
      Double_t sigma = par[3*p+4];
#if defined(__PEAKS_C_FIT_AREAS__)
      norm /= sigma * (TMath::Sqrt(TMath::TwoPi())); // "area"
#endif /* defined(__PEAKS_C_FIT_AREAS__) */
      result += norm*TMath::Gaus(x[0],mean,sigma);
   }
   return result;
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
    Int_t npeaks = 30;
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

        Double_t par[3000];
        for (Int_t iCh=0; iCh<channels_num; iCh++) {
            
            hFEBCH[iCh] = file_reader.Print_hFEBCH(iCh);
            hFEBCH[iCh]->Draw();
            hFEBCH[iCh]->GetYaxis()->SetTitle("Number of events");
            hFEBCH[iCh]->GetXaxis()->SetTitle("HG ADC channels");
            hFEBCH[iCh]->Write();
               // Generate n peaks at random
            Double_t par[3000];
            TSpectrum *s = new TSpectrum(2*npeaks);
            Int_t nfound = s->Search(hFEBCH[iCh],2,"",0.001);
//            TH1 *hb = s->Background(hFEBCH[iCh],20,"same");
            c1->Update();
            npeaks = 0;
            TF1 *fline = new TF1("fline","pol1",0,1000);
            hFEBCH[iCh]->Fit("fline","qn");
            par[0] = fline->GetParameter(0);
            par[1] = fline->GetParameter(1);
            Double_t *xpeaks = s->GetPositionX();
            for (auto p=0;p<nfound;p++) {
                  Double_t xp = xpeaks[p];
                  Int_t bin = hFEBCH[iCh]->GetXaxis()->FindBin(xp);
                  Double_t yp = hFEBCH[iCh]->GetBinContent(bin);
                  if (yp-TMath::Sqrt(yp) < fline->Eval(xp)) continue;
                  par[3*npeaks+2] = yp; // "height"
                  par[3*npeaks+3] = xp; // "mean"
                  par[3*npeaks+4] = 3; // "sigma"
            #if defined(__PEAKS_C_FIT_AREAS__)
                  par[3*npeaks+2] *= par[3*npeaks+4] * (TMath::Sqrt(TMath::TwoPi())); // "area"
            #endif /* defined(__PEAKS_C_FIT_AREAS__) */
                  npeaks++;
                  }

               // We may have more than the default 25 parameters
            TVirtualFitter::Fitter(hFEBCH[iCh],10+3*npeaks);

            for (auto p=0;p<nfound;p++) {
                  TF1 *fit_1 = new TF1("fit_1",fpeaks,par[3*p+3]-3*par[3*p+4],par[3*p+3]+3*par[3*p+4],2+3*npeaks);
                  fit_1->SetParameters(par);
                  fit_1->SetNpx(1000);
                  hFEBCH[iCh]->Fit("fit_1","r+");
                   c1->Update();
                  }
            delete hFEBCH[iCh];
        }
    }
        fList.close();


    return 0;
}

//fList.seekg(0); // возврат к началу файла
