//
// Created by amefodev on 21.08.2023.
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
    rootFileOutput += "AttenuationLength.root";
    cout << rootFileOutput << endl;
    TFile wfile(rootFileOutput.c_str(), "recreate");


    TH1F LY ("LY","LY for all fibers", 512, 0, 4096);
    TH1F LY_LG ("LY_LG","LY_LG for all fibers", 512, 0, 4096);
    TH1F LY_vert_f ("LY_vert_f", "LY for vertical fibers", 512, 0, 4096);
    TH2F *sumLY = new  TH2F("Attenuation Length","Attenuation Length", X_SIZE, 0, X_SIZE, 256, 0, 4096);

    TH1F *LY_Length_HG[X_SIZE];
    TH1F *LY_Length_HG_all_Z[X_SIZE];
    TH1F *LY_Length_HG_all_X[X_SIZE];
    TH2F *LY_Length_HG_Z[Y_SIZE];
    TH2F *LY_Length_HG_X[Y_SIZE];

    TH1F *LY_Length_PE[X_SIZE];
    TH1F *LY_Length_PE_all_Z[X_SIZE];
    TH1F *LY_Length_PE_all_X[X_SIZE];
    TH2F *LY_Length_PE_Z[Y_SIZE];
    TH2F *LY_Length_PE_X[Y_SIZE];

    TH2F AllEventsMap_YX("AllEventsMap_YX","AllEventsMap_YX", X_SIZE, 0, X_SIZE, Y_SIZE, 0, Y_SIZE);
    TH2F AllEventsMap_XZ("AllEventsMap_XZ","AllEventsMap_XZ", Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);
    TH2F AllEventsMap_YZ("AllEventsMap_YZ","AllEventsMap_YZ", Z_SIZE, 0, Z_SIZE, Y_SIZE, 0, Y_SIZE);

    TH2F AllEventsMap_PE_YX("AllEventsMap_PE_YX","AllEventsMap_PE_YX", X_SIZE, 0, X_SIZE, Y_SIZE, 0 , Y_SIZE);
    TH2F AllEventsMap_PE_XZ("AllEventsMap_PE_XZ","AllEventsMap_PE_XZ", Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);
    TH2F AllEventsMap_PE_YZ("AllEventsMap_PE_YZ","AllEventsMap_PE_YZ", Z_SIZE, 0, Z_SIZE, Y_SIZE, 0, Y_SIZE);

    TH2F AllEventsCount_YX("AllEventsCount_YX","AllEventsCount_YX", X_SIZE, 0, X_SIZE, Y_SIZE, 0 , Y_SIZE);
    TH2F AllEventsCount_XZ("AllEventsCount_XZ","AllEventsCount_XZ", Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);
    TH2F AllEventsCount_YZ("AllEventsCount_YZ","AllEventsCount_YZ", Z_SIZE, 0, Z_SIZE, Y_SIZE, 0, Y_SIZE);

    TH2F AllEventsMap_YX_low_LY("AllEventsMap_YX_low_LY","AllEventsMap_YX_low_LY", X_SIZE, 0, X_SIZE, Y_SIZE, 0, Y_SIZE);
    TH2F AllEventsMap_XZ_low_LY("AllEventsMap_XZ_low_LY","AllEventsMap_XZ_low_LY", Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);
    TH2F AllEventsMap_YZ_low_LY("AllEventsMap_YZ_low_LY","AllEventsMap_YZ_low_LY", Z_SIZE, 0, Z_SIZE, Y_SIZE, 0, Y_SIZE);

    TH1F LY_X116Y56Z63 ("LY_X116Y56Z63 ","LY for X = 116, Y = 56, Z = 63", 512, 0, 4096);
    TH1F LY_X186Y56Z180 ("LY_X186Y56Z180","LY for X = 186, Y = 56, Z = 180", 512, 0, 4096);

    ostringstream length;
    string sLength;
    for (int i = 1; i < X_SIZE; ++i) {
        length.str("");
        length << i;
        sLength= "position_" + length.str();
        LY_Length_HG[i] = new TH1F((sLength + "_HG").c_str(),(sLength + "_HG").c_str(), 128, 0, 4096);
        LY_Length_HG_all_Z[i] = new TH1F((sLength + "_HG_Z").c_str(), (sLength + "_HG_Z").c_str(), 128, 0, 4096);
        LY_Length_HG_all_X[i] = new TH1F((sLength + "_HG_X").c_str(), (sLength + "_HG_X").c_str(), 128, 0, 4096);

        LY_Length_PE[i] = new TH1F((sLength + "_PE").c_str(),(sLength + "_PE").c_str(), 128, 0, 256);
        LY_Length_PE_all_Z[i] = new TH1F((sLength + "_PE_Z").c_str(), (sLength + "_PE_Z").c_str(), 128, 0, 256);
        LY_Length_PE_all_X[i] = new TH1F((sLength + "_PE_X").c_str(), (sLength + "_PE_X").c_str(), 128, 0, 256);

    }
    for (int j = 0; j <= Y_SIZE; ++j) {
        LY_Length_HG_Z[j] = new TH2F(("HG_Z_at_Y" + to_string(j)).c_str(), ("HG_Z_at_Y" + to_string(j)).c_str(), X_SIZE, 0, X_SIZE, 256, 0, 4096);
        LY_Length_HG_X[j] = new TH2F(("HG_X_at_Y" + to_string(j)).c_str(), ("HG_X_at_Y" + to_string(j)).c_str(), X_SIZE, 0, X_SIZE, 256, 0, 4096);

        LY_Length_PE_Z[j] = new TH2F(("PE_Z_at_Y" + to_string(j)).c_str(), ("PE_Z_at_Y" + to_string(j)).c_str(), X_SIZE, 0, X_SIZE, 40, 0, 80);
        LY_Length_PE_X[j] = new TH2F(("PE_X_at_Y" + to_string(j)).c_str(), ("PE_X_at_Y" + to_string(j)).c_str(), X_SIZE, 0, X_SIZE, 40, 0, 80);
    }

    std::vector<std::vector<std::pair<Double_t, Int_t>>> all_event_map_HG_YX;
    all_event_map_HG_YX.resize(X_SIZE);
    std::vector<std::vector<std::pair<Double_t, Int_t>>> all_event_map_HG_YZ;
    all_event_map_HG_YZ.resize(X_SIZE);
    std::vector<std::vector<std::pair<Double_t, Int_t>>> all_event_map_HG_XZ;
    all_event_map_HG_XZ.resize(X_SIZE);

    std::vector<std::vector<std::pair<Double_t, Int_t>>> all_event_map_PE_YX;
    all_event_map_PE_YX.resize(X_SIZE);
    std::vector<std::vector<std::pair<Double_t, Int_t>>> all_event_map_PE_YZ;
    all_event_map_PE_YZ.resize(X_SIZE);
    std::vector<std::vector<std::pair<Double_t, Int_t>>> all_event_map_PE_XZ;
    all_event_map_PE_XZ.resize(X_SIZE);

    for (int x = 0; x < X_SIZE; ++x) {
        all_event_map_HG_YX[x].resize(Y_SIZE);
        all_event_map_HG_YZ[x].resize(Y_SIZE);
        all_event_map_HG_XZ[x].resize(X_SIZE);

        all_event_map_PE_YX[x].resize(Y_SIZE);
        all_event_map_PE_YZ[x].resize(Y_SIZE);
        all_event_map_PE_XZ[x].resize(X_SIZE);
    }

    for(unsigned int file = 0; file < vFileNames.size(); ++file) {
        string filename = vFileNames.at(file);
        TFile *fileInput = new TFile((filename).c_str());
        if (!fileInput->IsOpen()) {
            cerr << "Can not open file " << filename << endl;
            return 1;
        }
        cout << filename << endl;
//        fileInput->ls();
        auto dirList = fileInput->GetListOfKeys();
        for (const TObject *obj: *dirList) {
            if (std::strcmp(obj->GetName(), std::string("Events_Time").c_str()) != 0 ) {
//                cout << "Getting TDir: " << obj->GetName() << " "<< fileInput->GetDirectory(obj->GetName())->Class_Name() << endl;
                auto dirName = fileInput->GetDirectory(obj->GetName());
                auto filesNames = dirName->GetListOfKeys();
                std::vector<std::vector<Double_t>> event_map_YX;
                event_map_YX.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_YZ;
                event_map_YZ.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_XZ;
                event_map_XZ.resize(X_SIZE);

                std::vector<std::vector<Double_t>> event_map_HG_YX;
                event_map_HG_YX.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_HG_YZ;
                event_map_HG_YZ.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_HG_XZ;
                event_map_HG_XZ.resize(X_SIZE);

                std::vector<std::vector<Double_t>> event_map_LG_YX;
                event_map_LG_YX.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_LG_YZ;
                event_map_LG_YZ.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_LG_XZ;
                event_map_LG_XZ.resize(X_SIZE);

                std::vector<std::vector<Double_t>> event_map_PE_YX;
                event_map_PE_YX.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_PE_YZ;
                event_map_PE_YZ.resize(X_SIZE);
                std::vector<std::vector<Double_t>> event_map_PE_XZ;
                event_map_PE_XZ.resize(X_SIZE);

                for (int x = 0; x < X_SIZE; ++x) {
                    event_map_YX[x].resize(Y_SIZE);
                    event_map_YZ[x].resize(Y_SIZE);
                    event_map_XZ[x].resize(X_SIZE);

                    event_map_HG_YX[x].resize(Y_SIZE);
                    event_map_HG_YZ[x].resize(Y_SIZE);
                    event_map_HG_XZ[x].resize(X_SIZE);

                    event_map_LG_YX[x].resize(Y_SIZE);
                    event_map_LG_YZ[x].resize(Y_SIZE);
                    event_map_LG_XZ[x].resize(X_SIZE);

                    event_map_PE_YX[x].resize(Y_SIZE);
                    event_map_PE_YZ[x].resize(Y_SIZE);
                    event_map_PE_XZ[x].resize(X_SIZE);
                }
                for (auto *file: *filesNames) {
                    std::string str(file->GetName());
                    std::string  eventNum(obj->GetName());
                    if (str.find("Event_" + eventNum) != str.npos) {
                        GetEventsAmpl(fileInput, obj, file, str, eventNum, event_map_YX, event_map_YZ, event_map_XZ,
                                      70);
                    }
                    if (str.find("Event_HG_" + eventNum) != str.npos) {
                        GetEventsAmpl(fileInput, obj, file, str, eventNum, event_map_HG_YX, event_map_HG_YZ,
                                      event_map_HG_XZ, 70);
                    }
                    if (str.find("Event_LG_" + eventNum) != str.npos) {
                        GetEventsAmpl(fileInput, obj, file, str, eventNum, event_map_LG_YX, event_map_LG_YZ,
                                      event_map_LG_XZ, 70);
                    }
                    if (str.find("Event_pe_" + eventNum) != str.npos) {
                        GetEventsAmpl(fileInput, obj, file, str, eventNum, event_map_PE_YX, event_map_PE_YZ,
                                      event_map_PE_XZ, 70);
                    }
                }

                auto event_positionX_HG = max_in_vector(&event_map_HG_YX, X_SIZE, Y_SIZE);
                auto event_positionZ_HG = max_in_vector(&event_map_HG_YZ, Z_SIZE, Y_SIZE);
                auto event_positionYX_HG = max_in_vector_vertical(&event_map_HG_XZ, Z_SIZE, X_SIZE);

                auto event_positionX_LG = max_in_vector(&event_map_LG_YX, X_SIZE, Y_SIZE);
                auto event_positionZ_LG = max_in_vector(&event_map_LG_YZ, Z_SIZE, Y_SIZE);
                auto event_positionYX_LG = max_in_vector_vertical(&event_map_LG_XZ, Z_SIZE, X_SIZE);

                auto event_positionX_PE = max_in_vector(&event_map_PE_YX, X_SIZE, Y_SIZE);
                auto event_positionZ_PE = max_in_vector(&event_map_PE_YZ, Z_SIZE, Y_SIZE);
                auto event_positionYX_PE = max_in_vector_vertical(&event_map_PE_XZ, Z_SIZE, X_SIZE);

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
                                LY.Fill(event_positionX_HG.max_LY_[y]);
                                LY_LG.Fill(event_positionX_LG.max_LY_[y]);
                                sumLY->Fill(event_positionZ_HG.position_[y], event_positionX_HG.max_LY_[y], 1); // here

                                LY_Length_HG[event_positionZ_HG.position_[y]]->Fill(event_positionX_HG.max_LY_[y]);
                                LY_Length_HG_all_Z[event_positionZ_HG.position_[y]]->Fill(event_positionX_HG.max_LY_[y]);
                                LY_Length_HG_Z[y]->Fill(event_positionZ_HG.position_[y], event_positionX_HG.max_LY_[y],
                                                        1);

                                all_event_map_HG_YX[event_positionX_HG.position_[y]][y].first += event_positionX_HG.max_LY_[y];
                                ++all_event_map_HG_YX[event_positionX_HG.position_[y]][y].second;

                                if (event_positionX_HG.position_[y] == 63 && event_positionZ_HG.position_[y] == 116){
                                    LY_X116Y56Z63.Fill(event_positionX_HG.max_LY_[y]);
                                }

                                if (event_positionX_HG.position_[y] == 186 && event_positionZ_HG.position_[y] == 180){
                                    LY_X186Y56Z180.Fill(event_positionX_HG.max_LY_[y]);
                                }

                                if (event_positionZ_PE.existY_[y] && event_positionX_PE.max_LY_[y] > 0) {
                                    all_event_map_PE_YX[event_positionX_PE.position_[y]][y].first += event_positionX_PE.max_LY_[y];
                                    ++all_event_map_PE_YX[event_positionX_PE.position_[y]][y].second;

                                    LY_Length_PE[event_positionZ_PE.position_[y]]->Fill(event_positionX_PE.max_LY_[y]);
                                    LY_Length_PE_all_Z[event_positionZ_PE.position_[y]]->Fill(event_positionX_PE.max_LY_[y]);
                                    LY_Length_PE_Z[y]->Fill(event_positionZ_PE.position_[y], event_positionX_PE.max_LY_[y],
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
                                LY.Fill(event_positionZ_HG.max_LY_[y]);
                                LY_LG.Fill(event_positionZ_LG.max_LY_[y]);
                                LY.Fill(event_positionZ_HG.max_LY_[y]);
                                LY_LG.Fill(event_positionZ_LG.max_LY_[y]);
                                sumLY->Fill(event_positionX_HG.position_[y], event_positionZ_HG.max_LY_[y], 1); //here

                                all_event_map_HG_YZ[event_positionZ_HG.position_[y]][y].first += event_positionZ_HG.max_LY_[y];
                                ++all_event_map_HG_YZ[event_positionZ_HG.position_[y]][y].second;
                                if (event_positionZ_HG.position_[y] < 95) {
                                    LY_Length_HG[event_positionX_HG.position_[y]]->Fill(event_positionZ_HG.max_LY_[y]);
                                    LY_Length_HG_all_X[event_positionX_HG.position_[y]]->Fill(event_positionZ_HG.max_LY_[y]);
                                    LY_Length_HG_X[y]->Fill(event_positionX_HG.position_[y], event_positionZ_HG.max_LY_[y], 1);
                                    if (event_positionX_PE.existY_[y] && event_positionZ_PE.position_[y] > 0) {
                                        all_event_map_PE_YZ[event_positionZ_PE.position_[y]][y].first += event_positionZ_PE.max_LY_[y];
                                        ++all_event_map_PE_YZ[event_positionZ_PE.position_[y]][y].second;

                                        LY_Length_PE[event_positionX_PE.position_[y]]->Fill(event_positionZ_PE.max_LY_[y]);
                                        LY_Length_PE_all_X[event_positionX_PE.position_[y]]->Fill(event_positionZ_PE.max_LY_[y]);
                                        LY_Length_PE_X[y]->Fill(event_positionX_PE.position_[y],event_positionZ_PE.max_LY_[y], 1);
                                    }
                                } else {

                                    LY_Length_HG[X_SIZE - event_positionX_HG.position_[y]]->Fill(
                                            event_positionZ_HG.max_LY_[y]);
                                    LY_Length_HG_all_X[X_SIZE - event_positionX_HG.position_[y]]->Fill(
                                            event_positionZ_HG.max_LY_[y]);
                                    LY_Length_HG_X[y]->Fill(X_SIZE - event_positionX_HG.position_[y],
                                                            event_positionZ_HG.max_LY_[y], 1);
                                    if (event_positionX_PE.existY_[y] && event_positionZ_PE.position_[y] > 0) {
                                        LY_Length_PE[X_SIZE - event_positionX_PE.position_[y]]->Fill(event_positionZ_PE.max_LY_[y]);
                                        LY_Length_PE_all_X[X_SIZE - event_positionX_PE.position_[y]]->Fill(event_positionZ_PE.max_LY_[y]);
                                        LY_Length_PE_X[y]->Fill(X_SIZE - event_positionX_PE.position_[y], event_positionZ_PE.max_LY_[y], 1);
                                    }
                                }
                            }
                        }
                    }

                    if (event_positionYX_HG.countX_ > event_positionYX_HG.countZ_) {
                        for (int z = 0; z < Z_SIZE; ++z) {
                            if (event_positionYX_HG.existX_[z] && (z == Z_SIZE || event_positionYX_HG.existX_[z + 1]) &&
                                (z == 0 || event_positionYX_HG.existX_[z - 1])) {
                                LY_vert_f.Fill(event_positionYX_HG.maxX_LY_[z]);
                                //                            if (event_positionYX_HG.maxX_LY_[z] > 300 && event_positionYX_HG.maxX_LY_[z] < 4095) {
                                all_event_map_HG_XZ[event_positionYX_HG.positionX_[z]][z].first += event_positionYX_HG.maxX_LY_[z];
                                ++all_event_map_HG_XZ[event_positionYX_HG.positionX_[z]][z].second;
                                if (event_positionYX_PE.existX_[z] && (z == Z_SIZE || event_positionYX_PE.existX_[z + 1]) &&
                                    (z == 0 || event_positionYX_PE.existX_[z - 1])) {
                                    all_event_map_PE_XZ[event_positionYX_PE.positionX_[z]][z].first += event_positionYX_PE.maxX_LY_[z];
                                    ++all_event_map_PE_XZ[event_positionYX_PE.positionX_[z]][z].second;
                                    //                            }
                                }
                            }
                        }
                    } else {
                        for (int x = 0; x < X_SIZE; ++x) {
                            if (event_positionYX_HG.existZ_[x] && (x == X_SIZE || event_positionYX_HG.existZ_[x + 1]) &&
                                (x == 0 || event_positionYX_HG.existZ_[x - 1])) {
                                LY_vert_f.Fill(event_positionYX_HG.maxZ_LY_[x]);
                                //                            if (event_positionYX_HG.maxZ_LY_[x] > 300 && event_positionYX_HG.maxZ_LY_[x] < 4095) {
                                all_event_map_HG_XZ[x][event_positionYX_HG.positionZ_[x]].first += event_positionYX_HG.maxZ_LY_[x];
                                ++all_event_map_HG_XZ[x][event_positionYX_HG.positionZ_[x]].second;
                                if (event_positionYX_PE.existZ_[x] && (x == X_SIZE || event_positionYX_PE.existZ_[x + 1]) &&
                                    (x == 0 || event_positionYX_PE.existZ_[x - 1])) {
                                    all_event_map_HG_XZ[x][event_positionYX_PE.positionZ_[x]].first += event_positionYX_PE.maxZ_LY_[x];
                                    ++all_event_map_HG_XZ[x][event_positionYX_PE.positionZ_[x]].second;
                                    //                            }
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

    TCanvas *c1 = new TCanvas("AttLen_HG_all_Z", "", 0, 10, 700, 500);
    TCanvas *c2 = new TCanvas("AttLen_HG_all_X", "", 0, 10, 700, 500);
    TCanvas *c3 = new TCanvas("AttLen_HG_X_Z", "", 0, 10, 700, 500);

    TCanvas *c4 = new TCanvas("AttLen_PE_all_Z", "", 0, 10, 700, 500);
    TCanvas *c5 = new TCanvas("AttLen_PE_all_X", "", 0, 10, 700, 500);
    TCanvas *c6 = new TCanvas("AttLen_PE_X_Z", "", 0, 10, 700, 500);


    TDirectory *hitDir_HG =  wfile.mkdir("Avr_LY_HG");
    auto sumLY_avr_err_HG = new TMultiGraph();
    auto sumLY_avr_err_HG_Z = new TGraphErrors();
    auto sumLY_avr_err_HG_X = new TGraphErrors();
    for (int x = 1; x < X_SIZE; ++x) {
        int npeaks_HG = 3;
        TSpectrum *s = new TSpectrum(2*npeaks_HG);
        int nfound_HG = s->Search(LY_Length_HG[x], 2, "", 1);
        double *xpeaks_HG = s->GetPositionX();
        std::sort(xpeaks_HG, xpeaks_HG + nfound_HG);
        TF1 * fit = new TF1("fit","gaus");
        if (xpeaks_HG[0] > 150 ) {
            fit = new TF1("fit", "gaus", xpeaks_HG[0] - 12 * 15, xpeaks_HG[0] + 12 * 15);
        } else {
            fit = new TF1("fit", "gaus", xpeaks_HG[1] - 12 * 15, xpeaks_HG[1] + 12 * 15);
        }
        LY_Length_HG[x]->Fit("fit","MER", "");
        double mean_LY_HG = fit->GetParameter(1);
        double mean_LY_err_HG = fit->GetParError(1);
        LY_Length_HG[x]->GetXaxis()->SetTitle("Observed Light HG [ADC]");
        LY_Length_HG[x]->GetYaxis()->SetTitle("N");
        hitDir_HG->cd();
        LY_Length_HG[x]->Write();
        LY_Length_HG[x]->Delete();

        if(LY_Length_HG_all_Z[x]->GetEntries() > 0) {
            nfound_HG = s->Search(LY_Length_HG_all_Z[x], 2, "", 10);
            xpeaks_HG = s->GetPositionX();
            std::sort(xpeaks_HG, xpeaks_HG + nfound_HG);
            if (xpeaks_HG[0] > 150 ) {
                fit = new TF1("fit", "gaus", xpeaks_HG[0] - 12 * 15, xpeaks_HG[0] + 12 * 15);
            } else {
                fit = new TF1("fit", "gaus", xpeaks_HG[1] - 12 * 15, xpeaks_HG[1] + 12 * 15);
            }
            LY_Length_HG_all_Z[x]->Fit("fit", "MER", "");
            LY_Length_HG_all_Z[x]->GetXaxis()->SetTitle("Observed Light HG [ADC]");
            LY_Length_PE_all_Z[x]->GetYaxis()->SetTitle("N");
            double mean_LY_HG_Z = fit->GetParameter(1);
            double mean_LY_err_HG_Z = fit->GetParError(1);
            sumLY_avr_err_HG_Z->AddPoint(x + BOX_UPSTREAM, mean_LY_HG_Z);
            sumLY_avr_err_HG_Z->SetPointError(x - 1, 0, mean_LY_err_HG_Z);
            LY_Length_HG_all_Z[x]->Write();
            LY_Length_HG_all_Z[x]->Delete();
        }

        if(LY_Length_HG_all_X[x]->GetEntries() > 0) {
            nfound_HG = s->Search(LY_Length_HG_all_X[x], 2, "", 10);
            xpeaks_HG = s->GetPositionX();
            std::sort(xpeaks_HG, xpeaks_HG + nfound_HG);
            if (xpeaks_HG[0] > 150 ) {
                fit = new TF1("fit", "gaus", xpeaks_HG[0] - 12 * 15, xpeaks_HG[0] + 12 * 15);
            } else {
                fit = new TF1("fit", "gaus", xpeaks_HG[1] - 12 * 15, xpeaks_HG[1] + 12 * 15);
            }
            LY_Length_HG_all_X[x]->Fit("fit", "MER", "");
            LY_Length_HG_all_X[x]->GetXaxis()->SetTitle("Observed Light HG [ADC]");
            LY_Length_HG_all_X[x]->GetYaxis()->SetTitle("N");
            double mean_LY_HG_X = fit->GetParameter(1);
            double mean_LY_err_HG_X = fit->GetParError(1);
            if (x < 95) {
                sumLY_avr_err_HG_X->AddPoint(x + BOX_RIGHT_FOAM + BOX_RIGHT_MPPCs, mean_LY_HG_X);
                sumLY_avr_err_HG_X->SetPointError(x - 1, 0, mean_LY_err_HG_X);
            } else {
                sumLY_avr_err_HG_X->AddPoint(x + BOX_LEFT_MPPCs, mean_LY_HG_X);
                sumLY_avr_err_HG_X->SetPointError(x - 1, 0, mean_LY_err_HG_X);
            }
            LY_Length_HG_all_X[x]->Write();
            LY_Length_HG_all_X[x]->Delete();
        }
    }

    TDirectory *hitDir_PE =  wfile.mkdir("Avr_LY_PE");
    auto sumLY_avr_err_PE = new TMultiGraph();
    auto sumLY_avr_err_PE_Z = new TGraphErrors();
    auto sumLY_avr_err_PE_X = new TGraphErrors();
    for (int x = 1; x < X_SIZE; ++x) {
        int npeaks_PE = 3;
        TSpectrum *s = new TSpectrum(2*npeaks_PE);
        int nfound_PE = s->Search(LY_Length_PE[x],2,"",1);
        double *xpeaks_PE = s->GetPositionX();
        std::sort(xpeaks_PE, xpeaks_PE + nfound_PE);
        TF1 *fit;
        if (xpeaks_PE[0] > 15 ) {
            fit = new TF1("fit", "gaus", xpeaks_PE[0] - 6, xpeaks_PE[0] + 6);
        } else {
            fit = new TF1("fit", "gaus", xpeaks_PE[1] - 6, xpeaks_PE[1] + 6);
        }
        LY_Length_PE[x]->Fit("fit","MER", "");
        double mean_LY_PE = fit->GetParameter(1);
        double mean_LY_err_PE = fit->GetParError(1);
        LY_Length_PE[x]->GetXaxis()->SetTitle("Observed Light [p.e.]");
        LY_Length_PE[x]->GetYaxis()->SetTitle("N");
        hitDir_PE->cd();
        LY_Length_PE[x]->Write();
        LY_Length_PE[x]->Delete();

        if(LY_Length_PE_all_Z[x]->GetEntries() > 0) {
            nfound_PE = s->Search(LY_Length_PE_all_Z[x], 2, "", 10);
            xpeaks_PE = s->GetPositionX();
            std::sort(xpeaks_PE, xpeaks_PE + nfound_PE);
            if (xpeaks_PE[0] > 15 ) {
                fit = new TF1("fit", "gaus", xpeaks_PE[0] - 6, xpeaks_PE[0] + 6);
            } else {
                fit = new TF1("fit", "gaus", xpeaks_PE[1] - 6, xpeaks_PE[1] + 6);
            }
            LY_Length_PE_all_Z[x]->Fit("fit", "MER", "");
            LY_Length_PE_all_Z[x]->GetXaxis()->SetTitle("Observed Light [p.e.]");
            LY_Length_PE_all_Z[x]->GetYaxis()->SetTitle("N");
            double mean_LY_PE_Z = fit->GetParameter(1);
            double mean_LY_err_PE_Z = fit->GetParError(1);
            sumLY_avr_err_PE_Z->AddPoint(x + BOX_UPSTREAM, mean_LY_PE_Z);
            sumLY_avr_err_PE_Z->SetPointError(x - 1, 0, mean_LY_err_PE_Z);
            LY_Length_PE_all_Z[x]->Write();
            LY_Length_PE_all_Z[x]->Delete();
        }

        if(LY_Length_PE_all_X[x]->GetEntries() > 0) {
            nfound_PE = s->Search(LY_Length_PE_all_X[x], 2, "", 10);
            xpeaks_PE = s->GetPositionX();
            std::sort(xpeaks_PE, xpeaks_PE + nfound_PE);
            if (xpeaks_PE[0] > 15 ) {
                fit = new TF1("fit", "gaus", xpeaks_PE[0] - 6, xpeaks_PE[0] + 6);
            } else {
                fit = new TF1("fit", "gaus", xpeaks_PE[1] - 6, xpeaks_PE[1] + 6);
            }
            LY_Length_PE_all_X[x]->Fit("fit", "MER", "");
            LY_Length_PE_all_X[x]->GetXaxis()->SetTitle("Observed Light [p.e.]");
            LY_Length_PE_all_X[x]->GetYaxis()->SetTitle("N");
            double mean_LY_PE_X = fit->GetParameter(1);
            double mean_LY_err_PE_X = fit->GetParError(1);
            if (x < 95) {
                sumLY_avr_err_PE_X->AddPoint(x + BOX_RIGHT_FOAM + BOX_RIGHT_MPPCs, mean_LY_PE_X);
                sumLY_avr_err_PE_X->SetPointError(x - 1, 0, mean_LY_err_PE_X);
            } else {
                sumLY_avr_err_PE_X->AddPoint(x + BOX_LEFT_MPPCs, mean_LY_PE_X);
                sumLY_avr_err_PE_X->SetPointError(x - 1, 0, mean_LY_err_PE_X);
            }
            LY_Length_PE_all_X[x]->Write();
            LY_Length_PE_all_X[x]->Delete();
        }
    }

    wfile.cd();
    sumLY->GetYaxis()->SetTitle("Observed Light HG [ADC]");
    sumLY->GetXaxis()->SetTitle("Length [cm]");

    TF1 *fit_func = new TF1("fit_func", "((([0] * exp (-1.0 * x / [2] ) + [1] * exp (-1.0 * x / [3])) + [4] * ([0] * exp (-1.0 * (390.0 - x) / [2]) + [1] * exp (-1.0 * (390.0 - x) / [3]))))", 1, 181);
    fit_func->SetLineColor(1);
    fit_func->SetLineWidth(3);

    fit_func->SetParLimits(2, 5, 60);
    fit_func->SetParName(2, "A_{s}");

    fit_func->SetParLimits(3, 250, 550);
    fit_func->SetParName(3, "A_{l}");

    fit_func->SetParLimits(4, 0.15, 0.25);
    fit_func->SetParName(4, "R");

//    Fit HG

    fit_func->SetParLimits(0, 5 * 15, 20 * 15);
    fit_func->SetParName(0, "L_{s}");

    fit_func->SetParLimits(1, 12 * 15, 75 * 15);
    fit_func->SetParName(1, "L_{l}");

    c1->cd();
    c1->SetGrid();
    sumLY_avr_err_HG_Z->SetMarkerStyle(7);
    sumLY_avr_err_HG_Z->SetMarkerSize(10);
    sumLY_avr_err_HG_Z->SetMarkerColor(4);
    sumLY_avr_err_HG_Z->SetLineColor(4);
    sumLY_avr_err_HG_Z->GetYaxis()->SetTitle("Observed Light HG [ADC]");
    sumLY_avr_err_HG_Z->GetXaxis()->SetTitle("Length Z [cm]");
    sumLY_avr_err_HG_Z->Draw("AP");

    c2->cd();
    c2->SetGrid();
    sumLY_avr_err_HG_X->SetMarkerStyle(7);
    sumLY_avr_err_HG_X->SetMarkerSize(10);
    sumLY_avr_err_HG_X->SetMarkerColor(2);
    sumLY_avr_err_HG_X->SetLineColor(2);
    sumLY_avr_err_HG_X->GetYaxis()->SetTitle("Observed Light HG [ADC]");
    sumLY_avr_err_HG_X->GetXaxis()->SetTitle("Length X [cm]");
    sumLY_avr_err_HG_X->Draw("AP");


    c3->cd();
    c3->SetGrid();
    sumLY_avr_err_HG->Add(sumLY_avr_err_HG_Z);
    sumLY_avr_err_HG->Add(sumLY_avr_err_HG_X);
    sumLY_avr_err_HG->GetYaxis()->SetTitle("Observed Light HG [ADC]");
    sumLY_avr_err_HG->GetXaxis()->SetTitle("Length [cm]");
    sumLY_avr_err_HG->SetTitle("Attenuation Length X and Z HG");
    sumLY_avr_err_HG->Draw("AP");
    sumLY_avr_err_HG->Fit(fit_func,"MER+");
    c3->Modified();
    c3->Write();
    c3->Delete();

    sumLY_avr_err_HG_Z->SetTitle("Attenuation Length Z HG");
    sumLY_avr_err_HG_Z->Fit(fit_func, "MER+");
    c1->Modified();
    c1->Write();
    c1->Delete();

    sumLY_avr_err_HG_X->SetTitle("Attenuation Length X HG");
    sumLY_avr_err_HG_X->Fit(fit_func, "MER+");
    c2->Modified();
    c2->Write();
    c2->Delete();

//    Fit PE

    fit_func->SetParLimits(0, 5, 20);
    fit_func->SetParName(0, "L_{s}");

    fit_func->SetParLimits(1, 12, 75);
    fit_func->SetParName(1, "L_{l}");

    c4->cd();
    c4->SetGrid();
    sumLY_avr_err_PE_Z->SetMarkerStyle(7);
    sumLY_avr_err_PE_Z->SetMarkerSize(10);
    sumLY_avr_err_PE_Z->SetMarkerColor(4);
    sumLY_avr_err_PE_Z->SetLineColor(4);
    sumLY_avr_err_PE_Z->GetYaxis()->SetTitle("Observed Light [p.e.]");
    sumLY_avr_err_PE_Z->GetXaxis()->SetTitle("Length Z [cm]");
    sumLY_avr_err_PE_Z->Draw("AP");

    c5->cd();
    c5->SetGrid();
    sumLY_avr_err_PE_X->SetMarkerStyle(7);
    sumLY_avr_err_PE_X->SetMarkerSize(10);
    sumLY_avr_err_PE_X->SetMarkerColor(2);
    sumLY_avr_err_PE_X->SetLineColor(2);
    sumLY_avr_err_PE_X->GetYaxis()->SetTitle("Observed Light [p.e.]");
    sumLY_avr_err_PE_X->GetXaxis()->SetTitle("Length X [cm]");
    sumLY_avr_err_PE_X->Draw("AP");

    c6->cd();
    c6->SetGrid();
    sumLY_avr_err_PE->Add(sumLY_avr_err_PE_Z);
    sumLY_avr_err_PE->Add(sumLY_avr_err_PE_X);
    sumLY_avr_err_PE->GetYaxis()->SetTitle("Observed Light [p.e.]");
    sumLY_avr_err_PE->GetXaxis()->SetTitle("Length [cm]");
    sumLY_avr_err_PE->SetTitle("Attenuation Length X and Z PE");
    sumLY_avr_err_PE->Draw("AP");
    sumLY_avr_err_PE->Fit(fit_func,"MER+");
    c6->Modified();
    c6->Write();
    c6->Delete();

    sumLY_avr_err_PE_Z->SetTitle("Attenuation Length Z PE");
    sumLY_avr_err_PE_Z->Fit(fit_func, "MER+");
    c4->Modified();
    c4->Write();
    c4->Delete();

    sumLY_avr_err_PE_X->SetTitle("Attenuation Length X PE");
    sumLY_avr_err_PE_X->Fit(fit_func, "MER+");
    c5->Modified();
    c5->Write();
    c5->Delete();

    TDirectory *hitDir_Ev =  wfile.mkdir("AllEvents");
    hitDir_Ev->cd();
    for (int x = 0; x < X_SIZE; ++x) {
        for (int y = 0; y < Y_SIZE; ++y) {
            if (all_event_map_HG_YX.at(x).at(y).second > 0) {
                AllEventsMap_YX.Fill(x, y, all_event_map_HG_YX.at(x).at(y).first / all_event_map_HG_YX.at(x).at(y).second);
                AllEventsMap_PE_YX.Fill(x, y,all_event_map_PE_YX.at(x).at(y).first / all_event_map_PE_YX.at(x).at(y).second);
                AllEventsCount_YX.Fill(x, y, all_event_map_HG_YX.at(x).at(y).second);
            }
        }
        for (int z = 0; z < Z_SIZE; ++z) {
            if (all_event_map_HG_XZ.at(z).at(x).second > 0){
                AllEventsMap_XZ.Fill(z, x, all_event_map_HG_XZ.at(z).at(x).first/all_event_map_HG_XZ.at(z).at(x).second);
                AllEventsMap_PE_XZ.Fill(z,x,all_event_map_PE_XZ.at(z).at(x).first/all_event_map_PE_XZ.at(z).at(x).second);
                AllEventsCount_XZ.Fill(z,x,all_event_map_HG_XZ.at(z).at(x).second);
            }
        }
    }
    for (int z = 0; z < Z_SIZE; ++z) {
        for (int y = 0; y < Y_SIZE; ++y) {
            if (all_event_map_HG_YZ.at(z).at(y).second > 0) {
                AllEventsMap_YZ.Fill(z, y, all_event_map_HG_YZ.at(z).at(y).first / all_event_map_HG_YZ.at(z).at(y).second);
                AllEventsMap_PE_YZ.Fill(z, y, all_event_map_PE_YZ.at(z).at(y).first / all_event_map_PE_YZ.at(z).at(y).second);
                AllEventsCount_YZ.Fill(z, y, all_event_map_HG_YZ.at(z).at(y).second);
            }
        }
    }


    AllEventsMap_YX.GetYaxis()->SetTitle("Y [cm]");
    AllEventsMap_YX.GetXaxis()->SetTitle("X [cm]");
    AllEventsMap_YX.GetZaxis()->SetTitle("avr HG [ADC]");

    AllEventsMap_PE_YX.GetYaxis()->SetTitle("Y [cm]");
    AllEventsMap_PE_YX.GetXaxis()->SetTitle("X [cm]");
    AllEventsMap_PE_YX.GetZaxis()->SetTitle("avr LY [p.e.]");

    AllEventsMap_YZ.GetYaxis()->SetTitle("Y [cm]");
    AllEventsMap_YZ.GetXaxis()->SetTitle("Z [cm]");
    AllEventsMap_YZ.GetZaxis()->SetTitle("avr HG [ADC]");

    AllEventsMap_PE_YZ.GetYaxis()->SetTitle("Y [cm]");
    AllEventsMap_PE_YZ.GetXaxis()->SetTitle("Z [cm]");
    AllEventsMap_PE_YZ.GetZaxis()->SetTitle("avr LY [p.e.]");

    AllEventsMap_PE_XZ.GetYaxis()->SetTitle("X [cm]");
    AllEventsMap_PE_XZ.GetXaxis()->SetTitle("Z [cm]");
    AllEventsMap_PE_XZ.GetZaxis()->SetTitle("avr LY [p.e.]");

    AllEventsCount_YX.GetYaxis()->SetTitle("Y [cm]");
    AllEventsCount_YX.GetXaxis()->SetTitle("X [cm]");
    AllEventsCount_YX.GetZaxis()->SetTitle("N");

    AllEventsCount_YZ.GetYaxis()->SetTitle("Y [cm]");
    AllEventsCount_YZ.GetXaxis()->SetTitle("Z [cm]");
    AllEventsCount_YZ.GetZaxis()->SetTitle("N");

    AllEventsCount_XZ.GetYaxis()->SetTitle("X [cm]");
    AllEventsCount_XZ.GetXaxis()->SetTitle("Z [cm]");
    AllEventsCount_XZ.GetZaxis()->SetTitle("N");

    AllEventsMap_YX.Write();
    AllEventsMap_YX.Delete();

    AllEventsMap_XZ.Write();
    AllEventsMap_XZ.Delete();

    AllEventsMap_YZ.Write();
    AllEventsMap_YZ.Delete();

    AllEventsMap_PE_YX.Write();
    AllEventsMap_PE_YX.Delete();

    AllEventsMap_PE_XZ.Write();
    AllEventsMap_PE_XZ.Delete();

    AllEventsMap_PE_YZ.Write();
    AllEventsMap_PE_YZ.Delete();

    AllEventsCount_YX.Write();
    AllEventsCount_YX.Delete();

    AllEventsCount_XZ.Write();
    AllEventsCount_XZ.Delete();

    AllEventsCount_YZ.Write();
    AllEventsCount_YZ.Delete();

    AllEventsMap_YX_low_LY.Write();
    AllEventsMap_YX_low_LY.Delete();

    AllEventsMap_XZ_low_LY.Write();
    AllEventsMap_XZ_low_LY.Delete();

    AllEventsMap_YZ_low_LY.Write();
    AllEventsMap_YZ_low_LY.Delete();

    wfile.cd();


    TF1 *fit = new TF1("fit", "[0] * exp((-1.0 * (x - [1])^2) / (2.0 * [2]^2 * (1 + [3] * (x - [1]) / [2])))", 100, 1500);

    fit->SetParName(0, "A");
    fit->SetParName(1, "m");
    fit->SetParName(2, "Sigma");
    fit->SetParName(3, "s");

    LY_X116Y56Z63.GetYaxis()->SetTitle("Observed Light HG [ADC]");
    LY_X116Y56Z63.GetXaxis()->SetTitle("N");
    LY_X116Y56Z63.Fit(fit, "MER+");
    LY_X116Y56Z63.Draw("");
    LY_X116Y56Z63.Write();


    LY_X186Y56Z180.GetYaxis()->SetTitle("Observed Light HG [ADC]");
    LY_X186Y56Z180.GetXaxis()->SetTitle("N");
    LY_X186Y56Z180.Fit(fit, "MER+");
    LY_X186Y56Z180.Draw("");
    LY_X186Y56Z180.Write();

    LY.GetXaxis()->SetTitle("Observed Light HG [ADC]");
    LY.GetYaxis()->SetTitle("N");
    LY.Write();

    LY_LG.GetXaxis()->SetTitle("Observed Light HG [ADC]");
    LY_LG.GetYaxis()->SetTitle("N");
    LY_LG.Write();

    LY_vert_f.GetXaxis()->SetTitle("Observed Light HG [ADC]");
    LY_vert_f.GetYaxis()->SetTitle("N");
    LY_vert_f.Write();

    TDirectory *AttLength =  wfile.mkdir("Att_Len_XandZ");
    AttLength->cd();
    for (int i = 0; i < Y_SIZE; ++i) {
        LY_Length_HG_Z[i]->GetXaxis()->SetTitle("Length Z [cm]");
        LY_Length_HG_Z[i]->GetYaxis()->SetTitle("Observed Light HG [ADC]");

        LY_Length_HG_X[i]->GetXaxis()->SetTitle("Length X [cm]");
        LY_Length_HG_X[i]->GetYaxis()->SetTitle("Observed Light HG [ADC]");

        LY_Length_HG_X[i]->Write();
        LY_Length_HG_X[i]->Delete();

        LY_Length_HG_Z[i]->Write();
        LY_Length_HG_Z[i]->Delete();

        LY_Length_PE_Z[i]->GetXaxis()->SetTitle("Length Z [cm]");
        LY_Length_PE_Z[i]->GetYaxis()->SetTitle("Observed Light [p.e.]");

        LY_Length_PE_X[i]->GetXaxis()->SetTitle("Length X [cm]");
        LY_Length_PE_X[i]->GetYaxis()->SetTitle("Observed Light [p.e.]");

        LY_Length_PE_X[i]->Write();
        LY_Length_PE_X[i]->Delete();

        LY_Length_PE_Z[i]->Write();
        LY_Length_PE_Z[i]->Delete();
    }

    wfile.Close();
    wfile.Delete();
    return 0;
}
