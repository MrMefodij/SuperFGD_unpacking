//
// Created by Maria on 12.07.2023.
//

#include <iostream>
#include "TGraph.h"
#include <TTree.h>
#include <TROOT.h>
#include <TLine.h>
#include "ThresholdXmlOutput.h"
#include "Files_Reader.h"
#include "MDargumentHandler.h"
#include "Calibration.h"
#include <TF1.h>
#include "SFGD_defines.h"


struct Threshold{
    int _DAC;
    int _ADC;
};

struct Coord{
    int _x_prev;
    int _x_next;
    double _y;
};

int main(int argc, char **argv){

    string stringBuf;
    MDargumentHandler argh("Example of sfgd threshold study.");
    argh.Init();

    // Check the user arguments consistancy
    // All mandatory arguments should be provided and
    // There should be no extra arguments

    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}
    // Treat arguments, obtain values to be used later
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}

    if (argh.GetMode() == "f") {
        if (argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else if (argh.GetMode() == "d") {
        if (argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else {
        return -1;
    }

    vector<string> vFileNames = argh.GetDataFiles(stringBuf,".bin");
    if ( vFileNames.empty() ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }
    std::vector<Threshold> threshold;
    vector<TH1F*> hFEBCH(SFGD_SLOT);
    for(int i = 0; i < SFGD_SLOT; i++){
        std::string sCh = "SLOT_"+std::to_string(i);//+"_Channel_"+std::to_string(channel_id);
        hFEBCH[i] = new TH1F((sCh).c_str(),sCh.c_str(),  701, 0, 700);
    }

    string rootFileOutput=GetLocation(stringBuf.c_str(), ".bin");
    rootFileOutput+="DAC10b.root";
    TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");
    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);
    Calibration cl;
    unsigned int board_Id;
    unsigned int channel_Id;
    for(const std::string& filename : vFileNames){
        string FileOutput =GetLocation(filename.c_str(), ".bin");
        size_t pos = FileOutput.find("_DAC");
        // Going through data file
        File_Reader file_reader;
        file_reader.ReadFile(filename,hFEBCH);
        // find numbers of measured FEB
        board_Id = file_reader.GetFEBNumber();
        channel_Id = file_reader.GetChannelNumber();
        //get histograms with peaks
        if(hFEBCH[board_Id& 0x0f]->GetEntries() > 1) {
            string feb_channel = "FEB_" + to_string(board_Id) + FileOutput.substr(pos);
            hFEBCH[board_Id & 0x0f]->Write(feb_channel.c_str());
            auto bin = hFEBCH[board_Id & 0x0f]->FindFirstBinAbove();
            while (hFEBCH[board_Id & 0x0f]->GetBinContent(bin) < 9 && hFEBCH[board_Id & 0x0f]->GetEntries() > 0) {
                hFEBCH[board_Id & 0x0f]->SetBinContent(bin, 0);
                bin = hFEBCH[board_Id & 0x0f]->FindFirstBinAbove();
            }
            if (bin > 0) {
                threshold.push_back({atoi(FileOutput.substr(pos + 8).c_str()), bin});
            } else {
                std::cout << "Nothing in " << FileOutput.substr(pos + 1) << std::endl;
            }
            hFEBCH[board_Id & 0x0f]->Reset();
        }
    }

    TGraph* g = new TGraph();
    c1->cd();
    g->SetTitle("FEBs DAC10b study");
    sort(threshold.begin(), threshold.end(), [](Threshold a, Threshold b)
    {
        return a._DAC < b._DAC;
    });
    unsigned int p_e = 0;
    map<unsigned int, vector<Threshold>> th;
    for(int i =0; i < threshold.size(); i++) {
        g->AddPoint(threshold[i]._DAC, threshold[i]._ADC);
        if(i!= 0 && abs(threshold[i]._ADC -threshold[i-1]._ADC) / (abs(threshold[i]._DAC - threshold[i-1]._DAC)) > 1 &&
           abs(threshold[i]._ADC -threshold[i-1]._ADC) / (abs(threshold[i]._DAC - threshold[i-1]._DAC)) < 4 ) {
            continue;
        }
        if(i ==0 || abs(threshold[i]._ADC -threshold[i-1]._ADC) / (abs(threshold[i]._DAC - threshold[i-1]._DAC)) > 1)
            p_e ++;
        th[p_e].push_back(threshold[i]);
    }
    std::vector<Coord> section;
    double mean = 0;
    vector<double> pars;
    for(auto i : th){
        TF1* f = new TF1("Linear fit","[0]",i.second[0]._DAC-2,i.second.back()._DAC+2);
        g->Fit(f,"qr+");
        double par;
        f->GetParameters(&par);
        if(section.size() > 0)
            mean += (par - section.back()._y);


        section.push_back({i.second[0]._DAC,i.second.back()._DAC,par});
        pars.push_back(par);
    }

    mean /= (section.size() - 1) * 4;
    std::vector<unsigned int> ADC;
    if(section.size() > 1) {
        c1->cd();
        for (auto i = 0; i < section.size() - 1; i++) {
            TF1 *f1 = new TF1("Square fit", "[2]*x*x + [1]*x + [0]", section[i]._x_next - 0.5 , section[i + 1]._x_prev + 0.5 );
            g->Fit(f1, "qr+");
            double par[3];
            f1->GetParameters(&par[0]);
//            double ADC_value = (-par[1] + sqrt(par[1] * par[1] - 4 * par[2] * (par[0] - (section[i]._y + mean)))) / 2 / par[2];
            double ADC_value = (-par[1] + sqrt(par[1] * par[1] - 4 * par[2] * (par[0] - (section[i]._y + (pars[i+1]-pars[i])/4)))) / 2 / par[2];
            std::cout << i + 1.25<<": "<<  round(ADC_value) << std::endl;
            std::cout << (pars[i+1]-pars[i])<<std::endl;
            ADC.push_back(round(ADC_value));
            c1->Update();
        }
    }


    ThresholdXmlOutput xmlFile;
    ThresholdData tempData {channel_Id / 32,ADC};
    vector<ThresholdData> tempBoard;
    tempBoard.push_back(tempData);
    BoardData<ThresholdData> tempBoardData;
    tempBoardData.AddAsics(board_Id,tempBoard);
    xmlFile.AddBoard(tempBoardData);
    xmlFile.WriteXml((stringBuf+"Threshold.xml").c_str());


    g->GetXaxis()->SetTitle("DAC10b");
    g->GetYaxis()->SetTitle("ADC channels");
    g->Draw("AC* same");
    c1->Update();

    for(auto i : ADC) {
        TLine *l1 = new TLine(i, 0, i, 500);
        l1->Draw();
    }
//    for(auto i : pars) {
//        TLine *l1 = new TLine(0, i, 500, i);
//        l1->Draw();
//    }

    c1->Write("FEBs DAC10b study");

    for(int i = 0; i < SFGD_SLOT; i++){
        delete hFEBCH[i];
    }
    wfile->Close();
    delete c1;
    return 0;
}

