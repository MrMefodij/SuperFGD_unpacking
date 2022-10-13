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
       
        TSpectrum *s = new TSpectrum(2*npeaks);
        Int_t nfound = s->Search(hFEBCH[iCh],2,"",0.01);
        printf("Found %d candidate peaks to fit\n",nfound);

//             TH1 *hb = s->Background(hFEBCH[iCh],20,"same");
//             if (hb) c1->Update();
//             //estimate linear background using a fitting method
//             TF1 *fline = new TF1("fline","pol1",0,1000);
//             hFEBCH[iCh]->Fit("fline","qn");
//             // Loop on all found peaks. Eliminate peaks at the background level
//             par[0] = fline->GetParameter(0);
//             par[1] = fline->GetParameter(1);
//             npeaks = 0;
//             Double_t *xpeaks;
//             xpeaks = s->GetPositionX();
//             for (auto p=0;p<nfound;p++) {
//                Double_t xp = xpeaks[p];
//                Int_t bin = hFEBCH[iCh]->GetXaxis()->FindBin(xp);
//                Double_t yp = hFEBCH[iCh]->GetBinContent(bin);
//                if (yp-TMath::Sqrt(yp) < fline->Eval(xp)) continue;
//                par[3*npeaks] = yp; // "height"
//                par[3*npeaks+1] = xp; // "mean"
//                par[3*npeaks+2] = 3; // "sigma"
//          #if defined(__PEAKS_C_FIT_AREAS__)
//                par[3*npeaks] *= par[3*npeaks+2 * (TMath::Sqrt(TMath::TwoPi())); // "area"
//          #endif /* defined(__PEAKS_C_FIT_AREAS__) */
//                npeaks++;
//             }
//             printf("Found %d useful peaks to fit\n",npeaks);
//             printf("Now fitting: Be patient\n");
//             TF1 *fit = new TF1("fit",fpeaks,0,1000,2+3*npeaks);
//             // We may have more than the default 25 parameters
//             TVirtualFitter::Fitter(hFEBCH[iCh],10+3*npeaks);
//             fit->SetParameters(par);
//             fit->SetNpx(1000);
//             hFEBCH[iCh]->Fit("fit");
                                    
        c1->Update();
        delete hFEBCH[iCh];
    }

}

    fList.close();


    return 0;
}

//fList.seekg(0); // возврат к началу файла
