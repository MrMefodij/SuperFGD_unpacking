//
// Created by Angelina Chvirova on 10.05.2024.
//
#include <string>
#include <iostream>
#include <TFile.h>
#include <TH2.h>
#include <TF1.h>
#include <TSpectrum.h>

#include "EventDisplay.h"
#include "MDargumentHandler.h"


using namespace std;
int main( int argc, char **argv ) {
    std::string stringBuf;
    // The following shows how to use the MDargumentHandler class
    // to deal with the main arguments
    // Define the arguments
    MDargumentHandler argh("Example of unpacking application.");
    argh.Init();
    
    if (argh.ProcessArguments(argc, argv)) {
        argh.Usage();
        return -1;
    }
    if (argh.GetValue("help")) {
        argh.Usage();
        return 0;
    }
    
    if (argh.GetMode() == "f") {
        if (argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else if (argh.GetMode() == "d") {
        if (argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else {
        return -1;
    }
    
    vector<string> vFileNames = argh.GetDataFiles(stringBuf, ".root", "FiberCalibration");
    std::sort(vFileNames.begin(), vFileNames.end());
    
    if (vFileNames.size() == 0) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }
    
    string rootFileOutput = "FiberParameters.root";
    cout << rootFileOutput << endl;
    TFile wfile(rootFileOutput.c_str(), "recreate");
    
    ofstream All;
    
    for (unsigned int file = 0; file < vFileNames.size(); ++file) {
        string filename = vFileNames.at(file);
        TFile *fileInput = new TFile((filename).c_str());
        if (!fileInput->IsOpen()) {
            cerr << "Can not open file " << filename << endl;
            return 1;
        }

        auto dirName = fileInput->GetDirectory("All_Fibers_X");
        auto filesNames = dirName->GetListOfKeys();
        for (auto a: *filesNames) {
            std::string y(a->GetName());
            cout << "Content: " << y << endl;
            TH2F *Fibers_X;
            std::string path("All_Fibers_X");
            path += "/";
            path += a->GetName();
            //                std::cout << path << endl;
            fileInput->GetObject(path.c_str(), Fibers_X);
            //                cout << Fibers->GetEntries() << endl;
            auto f = new TF1("Gaus fit", "gaus");
            TObjArray aSlices;
            Fibers_X->FitSlicesY(f, 0, -1, 10, "Q", &aSlices);
            auto Fiber_X = (TH1F *) aSlices[1];
            std::string str = {"test"};
            Fiber_X->SetTitle(str.c_str());
            TF1 *fit_func = new TF1("fit_func",
                                    "((([0] * exp (-1.0 * x / [2] ) + [1] * exp (-1.0 * x / [3])) + [4] * ([0] * exp (-1.0 * (390.0 - x) / [2]) + [1] * exp (-1.0 * (390.0 - x) / [3]))))",
                                    1, X_SIZE - 2);
            fit_func->SetLineWidth(3);

            if (Fiber_X->GetEntries() > 5) {
                fit_func->SetParLimits(0, 5, 20);
                fit_func->SetParName(0, "L_{s}");

                fit_func->SetParLimits(1, 12, 75);
                fit_func->SetParName(1, "L_{l}");

                fit_func->SetParLimits(2, 5, 60);
                fit_func->SetParName(2, "A_{s}");

                fit_func->SetParLimits(3, 250, 550);
                fit_func->SetParName(3, "A_{l}");

                fit_func->SetParLimits(4, 0.10, 0.25);
                fit_func->SetParName(4, "R");

                Fiber_X->Fit("fit_func", "MEQ", "");
                double Ls = fit_func->GetParameter(0);
                double Ls_err = fit_func->GetParError(0);

                double Ll = fit_func->GetParameter(1);
                double Ll_err = fit_func->GetParError(1);

                double As = fit_func->GetParameter(2);
                double As_err = fit_func->GetParError(2);

                double Al = fit_func->GetParameter(3);
                double Al_err = fit_func->GetParError(3);

                double R = fit_func->GetParameter(4);
                double R_err = fit_func->GetParError(4);

                All << " Ls:" << Ls << " Ls_err:" << Ls_err << " Ll:" << Ll << " Ll_err:" << Ll_err << " As:" << As
                    << " As_err:" << As_err << " Al:" << Al << " Al_err:" << Al_err << " R:" << R << " R_err" << R_err
                    << endl;


                Fiber_X->GetYaxis()->SetTitle("Observed Light [p.e.]");
                Fiber_X->GetXaxis()->SetTitle("Length [cm]");

            }
        }

//                wfile.cd();
//                Fibers_X->Write();
//                Fiber_X->Write();

        dirName = fileInput->GetDirectory("All_Fibers_Z");
        filesNames = dirName->GetListOfKeys();
        for (auto a: *filesNames) {
            std::string y(a->GetName());
            cout << "Content: " << y << endl;
            TH2F *Fibers_Z;
            std::string path("All_Fibers_Z");
            path += "/";
            path += a->GetName();
            //                std::cout << path << endl;
            fileInput->GetObject(path.c_str(), Fibers_Z);
            //                cout << Fibers->GetEntries() << endl;
            auto f = new TF1("Gaus fit", "gaus");
            TObjArray aSlices;
            Fibers_Z->FitSlicesY(f, 0, -1, 10, "Q", &aSlices);
            auto Fiber_Z = (TH1F *) aSlices[1];
            std::string str = {"test"};
            Fiber_Z->SetTitle(str.c_str());
            TF1 *fit_func = new TF1("fit_func",
                                    "((([0] * exp (-1.0 * x / [2] ) + [1] * exp (-1.0 * x / [3])) + [4] * ([0] * exp (-1.0 * (390.0 - x) / [2]) + [1] * exp (-1.0 * (390.0 - x) / [3]))))",
                                    1, X_SIZE - 2);
            fit_func->SetLineWidth(3);

            if (Fiber_Z->GetEntries() > 5) {
                fit_func->SetParLimits(0, 5, 20);
                fit_func->SetParName(0, "L_{s}");

                fit_func->SetParLimits(1, 12, 75);
                fit_func->SetParName(1, "L_{l}");

                fit_func->SetParLimits(2, 5, 60);
                fit_func->SetParName(2, "A_{s}");

                fit_func->SetParLimits(3, 250, 550);
                fit_func->SetParName(3, "A_{l}");

                fit_func->SetParLimits(4, 0.10, 0.25);
                fit_func->SetParName(4, "R");

                Fiber_Z->Fit("fit_func", "MEQ", "");
                double Ls = fit_func->GetParameter(0);
                double Ls_err = fit_func->GetParError(0);

                double Ll = fit_func->GetParameter(1);
                double Ll_err = fit_func->GetParError(1);

                double As = fit_func->GetParameter(2);
                double As_err = fit_func->GetParError(2);

                double Al = fit_func->GetParameter(3);
                double Al_err = fit_func->GetParError(3);

                double R = fit_func->GetParameter(4);
                double R_err = fit_func->GetParError(4);

                All << " Ls:" << Ls << " Ls_err:" << Ls_err << " Ll:" << Ll << " Ll_err:" << Ll_err << " As:" << As
                    << " As_err:" << As_err << " Al:" << Al << " Al_err:" << Al_err << " R:" << R << " R_err" << R_err
                    << endl;


                Fiber_Z->GetYaxis()->SetTitle("Observed Light [p.e.]");
                Fiber_Z->GetXaxis()->SetTitle("Length [cm]");

            }
        }

        //                wfile.cd();
        //                Fibers_Z->Write();
        //                Fiber_Z->Write();
    }
        wfile.Close();
        wfile.Delete();
        return 0;
}
