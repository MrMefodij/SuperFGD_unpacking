//
// Created by Maria on 07.04.2024.
//

#include <string>
#include <iostream>
#include <TFile.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraph.h>
#include "MDargumentHandler.h"
#include "ThresholdXmlOutput.h"
#include "SFGD_defines.h"
using namespace std;

#define correction_factor 32

struct FitGates{
    double start_;
    double end_;
    bool on_inflection_;
    int point_;
};

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

    std::vector<std::string> vFileNames = argh.GetDataFiles(stringBuf, ".root", "DAC10bt");
    std::sort(vFileNames.begin(), vFileNames.end());

    if (vFileNames.size() == 0) {
        std::cerr << "Can not open directory " << stringBuf << std::endl;
        return 1;
    }
    size_t pos = stringBuf.find("DAC10bt");
    std::string rootFileOutput = "";
    if (pos != string::npos)
        rootFileOutput = stringBuf.substr(0, pos) + "_";
    else {
        rootFileOutput = stringBuf;
    }
    rootFileOutput += "DAC_10bt_calibration.root";
    std::cout << rootFileOutput << std::endl;
    TFile wfile(rootFileOutput.c_str(), "recreate");
    ThresholdXmlOutput xmlFile;
    for (unsigned int file = 0; file < vFileNames.size(); ++file) {
        string filename = vFileNames.at(file);
        TFile *fileInput = new TFile((filename).c_str());
        if (!fileInput->IsOpen()) {
            cerr << "Can not open file " << filename << endl;
            return 1;
        }
        auto dirList = fileInput->GetListOfKeys();
        for (const TObject *obj: *dirList) {
            std::cout << "Getting TDir: " << obj->GetName() << " " << fileInput->GetDirectory(obj->GetName())->Class_Name() << std::endl;
                std::string path(obj->GetName());
                unsigned int ih = std::stoi(path.substr(3));
                TDirectory *FEBdir = wfile.mkdir(("FEB" + to_string(ih)).c_str());
                auto dirName = fileInput->GetDirectory(obj->GetName());
                for(unsigned int i = 1; i < SFGD_FEB_NCHANNELS / correction_factor + 1; ++i) {
                    TGraph *event;
                    fileInput->GetObject((path + "/;" + to_string(i)).c_str(), event);
                    double* _DAC10bt = event->GetX();
                    double* _channelHitGate = event->GetY();
                    double mean_attitude{0.0};
                    TGraph *DAC10bt_graph = new TGraph(event->GetN(), _DAC10bt, _channelHitGate);
                    DAC10bt_graph->SetTitle((path + " ASIC " + std::to_string(i - 1)).c_str());
                    for(auto j = 1; j < event->GetN(); ++j){
                        if(_channelHitGate[j] != 0) {
                            mean_attitude += _channelHitGate[j-1] / _channelHitGate[j];
                        }
                    }
                    mean_attitude /= event->GetN();
                    FitGates fg = {_DAC10bt[event->GetN() - 1],_DAC10bt[event->GetN() - 1], true, 0};
                    vector<unsigned int> thresholds;
                    TF1 *fit = new TF1("fit", "[0]*x+[1]");
                    for(auto j = 0; j < event->GetN() - 1; ++j) {
                        if (_channelHitGate[j+1] != 0) {
                            if (_channelHitGate[j] / _channelHitGate[j + 1] > mean_attitude  && ! fg.on_inflection_ ) {
                                if(fg.start_ < _DAC10bt[j-1]) {
                                    DAC10bt_graph->Fit(fit, "RQ+", "", fg.start_ - 0.5, _DAC10bt[j] + 0.5);
                                    DAC10bt_graph->Draw();
                                }
                                fg.on_inflection_ = true;
                                fg.end_ = _DAC10bt[j];
                                fg.point_ = j;
                            }

                            if (_channelHitGate[j] / _channelHitGate[j + 1] < mean_attitude && fg.on_inflection_) {
                                fg.on_inflection_ = false;
                                fg.start_ = _DAC10bt[j];
                                if(thresholds.empty() && fg.end_ != _DAC10bt[event->GetN() - 1] && ((_channelHitGate[j]  < 0.01 && _channelHitGate[j]  > 0.001) || (_channelHitGate[fg.point_]  > 0.01 && _channelHitGate[fg.point_] < 2))){
                                    thresholds.push_back(fg.end_ + 0.25 * (fg.start_ - fg.end_));
                                }
                            }
                        }
                    }
                    if(fg.start_ < _DAC10bt[event->GetN() - 2] && fg.end_ != _DAC10bt[event->GetN() - 1]) {
                        DAC10bt_graph->Fit(fit, "RQ+", "", fg.start_ - 0.5, _DAC10bt[event->GetN() - 1]);
                        DAC10bt_graph->Draw();
                    }
                    FEBdir->cd();
                    DAC10bt_graph->Write();
                    fit->Delete();
                    DAC10bt_graph->Delete();
                    event->Delete();
//                    std::sort(thresholds.begin(), thresholds.end());
                    if(thresholds.empty()){
                        std::cout <<"FEB "<<ih<<" ASIC "<<i - 1<<std::endl;
                    }
                    ThresholdData tempData{i - 1, thresholds};
                    std::vector<ThresholdData> tempBoard;
                    tempBoard.push_back(tempData);

                    BoardData<ThresholdData> tempBoardData;
                    tempBoardData.AddAsics(ih, tempBoard);
                    xmlFile.AddBoard(tempBoardData);
                }
            dirName->Delete();
            FEBdir->Close();

        }
    }
    xmlFile.WriteXml((stringBuf + "DAC10b_t.xml").c_str());
    std::cout << "XML file: " << (stringBuf + "DAC10b_t.xml") << std::endl;
    wfile.Close();
    return 0;
}