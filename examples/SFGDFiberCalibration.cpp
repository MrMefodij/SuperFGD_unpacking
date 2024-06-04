//
// Created by Angelina Chvirova on 26.03.2024.
//
#include <string>
#include <iostream>
#include <TFile.h>
#include <TH2.h>
#include <Files_Reader.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
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

    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}

    if (argh.GetMode() == "f") {
        if (argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else if (argh.GetMode() == "d") {
        if (argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else {
        return -1;
    }

    vector<string> vFileNames  = argh.GetDataFiles(stringBuf,".root","Events");
    std::sort(vFileNames.begin(), vFileNames.end());

    if ( vFileNames.size() == 0 ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }
    size_t pos = stringBuf.find("_Events");
    std::string rootFileOutput = "";
    if(pos != string::npos)
        rootFileOutput = stringBuf.substr(0, pos) + "_";
    else{
        rootFileOutput = stringBuf;
    }
    rootFileOutput += "FiberCalibration.root";
    cout << rootFileOutput << endl;
    TFile wfile(rootFileOutput.c_str(), "recreate");

    TH2F *Fibers_X[Y_SIZE][Z_SIZE];
    TH2F *Fibers_Z[Y_SIZE][X_SIZE];

    ostringstream FiberY, FiberX, FiberZ;
    string sFiber;
    for (int y = 0; y < Y_SIZE; ++y) {
        FiberY.str("");
        FiberY << y;
        for (int z = 0; z < Z_SIZE; ++z) {
            FiberZ.str("");
            FiberZ << z;
            sFiber = "FiberY" + FiberY.str() + "Z" + FiberZ.str();
            Fibers_X[y][z] = new TH2F(sFiber.c_str(), sFiber.c_str(), X_SIZE, 0, X_SIZE, Y_SIZE, 0, Y_SIZE);
        }
        for (int x = 0; x < X_SIZE; ++x) {
            FiberX.str("");
            FiberX << x;
            sFiber = "FiberY" + FiberY.str() + "X" + FiberX.str();
            Fibers_Z[y][x] = new TH2F(sFiber.c_str(), sFiber.c_str(), Z_SIZE, 0, Z_SIZE, Y_SIZE, 0, Y_SIZE);
        }
    }

    for (unsigned int file = 0; file < vFileNames.size(); ++file) {
        string filename = vFileNames.at(file);
        TFile *fileInput = new TFile((filename).c_str());
        if (!fileInput->IsOpen()) {
            cerr << "Can not open file " << filename << endl;
            return 1;
        }

        auto dirList = fileInput->GetListOfKeys();
        for (const TObject *obj: *dirList) {
            if (std::strcmp(obj->GetName(), std::string("Events_Time").c_str()) != 0) {
                cout << "Getting TDir: " << obj->GetName() << " "
                     << fileInput->GetDirectory(obj->GetName())->Class_Name() << endl;
                auto dirName = fileInput->GetDirectory(obj->GetName());
                auto filesNames = dirName->GetListOfKeys();

                std::vector<std::vector<Double_t>> event_map_HG_YX;
                event_map_HG_YX.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_HG_YZ;
                event_map_HG_YZ.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_HG_XZ;
                event_map_HG_XZ.resize(X_SIZE);

                std::vector<std::vector<Double_t>> event_map_PE_YX;
                event_map_PE_YX.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_PE_YZ;
                event_map_PE_YZ.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_PE_XZ;
                event_map_PE_XZ.resize(X_SIZE);

                for (int x = 0; x < X_SIZE; ++x) {

                    event_map_HG_YX[x].resize(Y_SIZE);
                    event_map_HG_YZ[x].resize(Y_SIZE);
                    event_map_HG_XZ[x].resize(X_SIZE);

                    event_map_PE_YX[x].resize(Y_SIZE);
                    event_map_PE_YZ[x].resize(Y_SIZE);
                    event_map_PE_XZ[x].resize(X_SIZE);
                }

                for (auto *file: *filesNames) {
                    std::string str(file->GetName());
                    std::string eventNum(obj->GetName());
                    if (str.find("Event_HG_" + eventNum) != str.npos) {
                        GetEventsAmpl(fileInput, obj, file, str, eventNum, event_map_HG_YX, event_map_HG_YZ,
                                      event_map_HG_XZ, 70);
                    }
                    if (str.find("Event_pe_" + eventNum) != str.npos) {
                        GetEventsAmpl(fileInput, obj, file, str, eventNum, event_map_PE_YX, event_map_PE_YZ,
                                      event_map_PE_XZ, 70);
                    }
                }

                auto event_positionX_HG = max_in_vector(&event_map_HG_YX, X_SIZE, Y_SIZE);
                auto event_positionZ_HG = max_in_vector(&event_map_HG_YZ, Z_SIZE, Y_SIZE);
                auto event_positionYX_HG = max_in_vector_vertical(&event_map_HG_XZ, Z_SIZE, X_SIZE);

                auto event_positionX_PE = max_in_vector(&event_map_PE_YX, X_SIZE, Y_SIZE);
                auto event_positionZ_PE = max_in_vector(&event_map_PE_YZ, Z_SIZE, Y_SIZE);
                auto event_positionYX_PE = max_in_vector_vertical(&event_map_PE_XZ, X_SIZE, Z_SIZE);

                for (int y = 0; y < Y_SIZE; ++y) {
                    if ((event_positionX_HG.existY_[y] && event_positionX_HG.position_[y] > 0 &&
                         event_positionX_HG.position_[y] <= X_SIZE &&
                         (y == 0 || event_positionX_HG.existY_[y - 1]) &&
                         (y == Y_SIZE - 1 || event_positionX_HG.existY_[y + 1]))
                        && (event_positionX_HG.position_[y] == 0 ||
                            event_positionX_HG.existX_[event_positionX_HG.position_[y - 1]])
                        && (event_positionX_HG.position_[y] == X_SIZE ||
                            event_positionX_HG.existX_[event_positionX_HG.position_[y + 1]])) {
                        if (event_positionZ_HG.position_[y] > 0 && event_positionZ_HG.position_[y] <= Z_SIZE) {
                            //                      Kodama
                            if (y > 0 && event_positionX_HG.position_[y] == event_positionX_HG.position_[y - 1] &&
                                event_positionX_HG.position_[y] == event_positionX_HG.position_[y + 1] &&
                                event_positionZ_HG.position_[y] == event_positionZ_HG.position_[y - 1] &&
                                event_positionZ_HG.position_[y] == event_positionZ_HG.position_[y + 1]) {
                                if (event_positionZ_PE.existY_[y] && event_positionX_PE.max_LY_[y] > 0) {
                                    Fibers_Z[y][event_positionX_PE.position_[y]]->Fill(event_positionZ_PE.position_[y],
                                                                                       event_positionX_PE.LY_summ_[y],
                                                                                       1);
                                }
                            }
                        }
                    }

                    if ((event_positionZ_HG.existY_[y] && event_positionZ_HG.position_[y] > 0 &&
                         event_positionZ_HG.position_[y] <= Z_SIZE &&
                         (y == 0 || event_positionZ_HG.existY_[y - 1]) &&
                         (y == Y_SIZE - 1 || event_positionZ_HG.existY_[y + 1]))
                        && (event_positionZ_HG.position_[y] == 0 ||
                            event_positionZ_HG.existX_[event_positionZ_HG.position_[y]])
                        && (event_positionZ_HG.position_[y] == Z_SIZE ||
                            event_positionZ_HG.existX_[event_positionZ_HG.position_[y + 1]])) {
                        if (event_positionX_HG.position_[y] > 0 && event_positionX_HG.position_[y] <= X_SIZE) {
                            //                      Kodama
                            if (y > 0 && event_positionZ_HG.position_[y] == event_positionZ_HG.position_[y - 1] &&
                                event_positionZ_HG.position_[y] == event_positionZ_HG.position_[y + 1] &&
                                event_positionX_HG.position_[y] == event_positionX_HG.position_[y - 1] &&
                                event_positionX_HG.position_[y] == event_positionX_HG.position_[y + 1]) {
                                if (event_positionZ_HG.position_[y] < 95) {
                                    if (event_positionX_PE.existY_[y] && event_positionZ_PE.position_[y] > 0) {
                                        Fibers_X[y][event_positionZ_PE.position_[y]]->Fill(event_positionX_PE.position_[y], event_positionZ_PE.LY_summ_[y], 1);
                                    }
                                } else {
                                    if (event_positionX_PE.existY_[y] && event_positionZ_PE.position_[y] > 0) {
                                        Fibers_X[y][event_positionZ_PE.position_[y]]->Fill(
                                                X_SIZE - event_positionX_PE.position_[y],
                                                event_positionZ_PE.LY_summ_[y], 1);
                                    }
                                }
                            }
                        }
                    }
                }
                filesNames->Delete();
                delete dirName;
            }
        }
        delete fileInput;
    }
    //    cout << "Nice" << endl;

    TDirectory *hitDir_Z = wfile.mkdir("All_Fibers_Z");
    hitDir_Z->cd();
    for (int y = 0; y < Y_SIZE; ++y) {
        for (int x = 0; x < X_SIZE; ++x) {
            Fibers_Z[y][x]->GetXaxis()->SetTitle("Length [cm]");
            Fibers_Z[y][x]->GetYaxis()->SetTitle("Observed Light Yield [p.e.]");
            Fibers_Z[y][x]->GetZaxis()->SetTitle("Count N");
            Fibers_Z[y][x]->SetTitle("Observed LY vs Z Fiber Length");
            Fibers_Z[y][x]->Write();
            Fibers_Z[y][x]->Delete();
        }
    }

    TDirectory *hitDir_X = wfile.mkdir("All_Fibers_X");
    hitDir_X->cd();
    for (int y = 0; y < Y_SIZE; ++y) {
        for (int z = 0; z < Z_SIZE; ++z) {
            Fibers_X[y][z]->GetXaxis()->SetTitle("Length [cm]");
            Fibers_X[y][z]->GetYaxis()->SetTitle("Observed Light Yield [p.e.]");
            Fibers_X[y][z]->GetZaxis()->SetTitle("Count N");
            Fibers_X[y][z]->SetTitle("Observed LY vs X Fiber Length");
            Fibers_X[y][z]->Write();
            Fibers_X[y][z]->Delete();
        }
    }

    wfile.Close();
    wfile.Delete();
    return 0;
}
