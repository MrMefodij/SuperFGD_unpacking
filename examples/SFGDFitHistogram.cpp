//
// Created by Maria on 15.04.2024.
//
#include <string>
#include <iostream>
#include <TFile.h>
#include <TH2.h>
#include <Files_Reader.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TSpectrum2.h>

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

    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}

    if (argh.GetMode() == "f") {
        if (argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else if (argh.GetMode() == "d") {
        if (argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else {
        return -1;
    }

    vector<string> vFileNames  = argh.GetDataFiles(stringBuf,".root","AttenuationLength");
    std::sort(vFileNames.begin(), vFileNames.end());

    if ( vFileNames.size() == 0 ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }
    size_t pos = stringBuf.find("AttenuationLength");
    std::string rootFileOutput = "";
    if(pos != string::npos)
        rootFileOutput = stringBuf.substr(0, pos) + "";
    else{
        rootFileOutput = stringBuf;
    }
    rootFileOutput += "HitRes.root";
    cout << rootFileOutput << endl;
    TFile wfile(rootFileOutput.c_str(), "recreate");

    ostringstream length;
    string sLength;

    unsigned int point_num = 0;
    TGraphErrors* gr_ll_ls = new TGraphErrors();
    for(unsigned int file = 0; file < vFileNames.size(); ++file) {
        string filename = vFileNames.at(file);
        TFile *fileInput = new TFile((filename).c_str());
        if (!fileInput->IsOpen()) {
            cerr << "Can not open file " << filename << endl;
            return 1;
        }
        auto dirList = fileInput->GetListOfKeys();
        for (const TObject *obj: *dirList) {
                cout << "Getting TDir: " << obj->GetName() << " "
                     << fileInput->GetDirectory(obj->GetName())->Class_Name() << endl;
                auto dirName = fileInput->GetDirectory(obj->GetName());
                if(obj->GetName()== std::string("Att_Len_XandZ")) {
                    auto filesNames = dirName->GetListOfKeys();
                    for (auto *file: *filesNames) {
                        std::string str(file->GetName());
                        std::string eventNum(obj->GetName());
                            if (str.find("PE_Z_at_Y") != str.npos && str.find("pfx") == std::string::npos) {
                                size_t pos = str.find("at_Y");
                                unsigned int y = std::stoi(str.substr(pos + 4));
                                TH2F *event;
                                std::string path(obj->GetName());
                                path += "/";
                                path += file->GetName();
                                fileInput->GetObject(path.c_str(), event);
                                auto f = new TF1("Gaus fit", "gaus");
                                TObjArray aSlices;
                                event->FitSlicesY(f, 0, -1, 10, "Q", &aSlices);
                                auto m = (TH1F *) aSlices[1];
                                m->SetTitle(str.c_str());
                                TF1 *fit_func = new TF1("fit_func",
                                                        "((([0]*exp(-1.0*x/[2])+[1]*exp(-1.0*x/[3])) + [4]*([0]*exp(-1.0*(390.0-x)/[2]) + [1]*exp(-1.0*(390.0-x)/[3]))))",
                                                        2, X_SIZE-2);

                                fit_func->SetLineWidth(3);

                                if (m->GetEntries() > 5) {
                                    fit_func->SetParLimits(0, 5, 20);
                                    fit_func->SetParName(0, "L_{s}");

                                    fit_func->SetParLimits(1, 12, 75);
                                    fit_func->SetParName(1, "L_{l}");

                                    fit_func->SetParLimits(2, 5, 60);
                                    fit_func->SetParName(2, "A_{s}");

                                    fit_func->SetParLimits(3, 250, 450);
                                    fit_func->SetParName(3, "A_{l}");

                                    fit_func->SetParLimits(4, 0.10, 0.25);
                                    fit_func->SetParName(4, "R");
                                    m->Fit("fit_func", "MEQ", "");
                                    m->GetXaxis()->SetLimits(0., X_SIZE);
                                    m->GetYaxis()->SetTitle("Observed Light [p.e.]");
                                    m->GetXaxis()->SetTitle("Length [cm]");

                                    if(56-y == 20){
                                        gr_ll_ls->AddPoint(y,
                                                           31.34 + 5.972);
                                        gr_ll_ls->SetPointError(point_num, 0,
                                                                0.21+0.195);
                                    }
                                    else {
                                        gr_ll_ls->AddPoint(56 - y,
                                                           fit_func->GetParameter(0) + fit_func->GetParameter(1));
                                        gr_ll_ls->SetPointError(point_num, 0,
                                                                fit_func->GetParError(0) + fit_func->GetParError(1));
                                    }

                                    ++point_num;
                                    wfile.cd();
                                    event->Write();
                                    m->Write();

                                    delete f;
                                    delete fit_func;
                                }
                            }
                    }

                    filesNames->Delete();
                }

                    delete dirName;
                }
            delete fileInput;

    }



    wfile.cd();
    double res = 0;
    double res_err = 0;
    TF1 *fit = new TF1("fit", "[0]");
    gr_ll_ls->Fit("fit", "MEQ", "", 0,20);
    res = fit->GetParameter(0);
    res_err = std::abs(fit->GetParError(0));
    gr_ll_ls->Fit("fit", "MEQ+", "", 21,55);
    res = fit->GetParameter(0) - res;
    res_err += std::abs(fit->GetParError(0));

    gr_ll_ls->Write("L_{s}+L_{l}");
    delete gr_ll_ls;
    delete fit;
    wfile.cd();
    wfile.Close();
    wfile.Delete();
    std::cout <<"Diff "<< res<<"+/-"<<std::abs(res_err)<<std::endl;
    return 0;
}
