#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "TGraph.h"
#include <TCanvas.h>
#include <TTree.h>
#include <TF1.h>
#include <TMath.h>
#include <TROOT.h>
#include "BaseLine.h"
#include "Files_Reader.h"
#include "MDpartEventSFGD.h"
#include "MDargumentHandler.h"
#include "XmlReaderWriter.h"
#include <map>
using namespace std;


Double_t fpeaks(Double_t *x, Double_t *par) {
   return par[0]*x[0] + par[1];
}

int main(int argc, char **argv){

    string stringBuf;
    MDargumentHandler argh("Example of sfgd baseline.");
    argh.AddArgument("help","print this message","h");
    argh.AddArgument("directory","Path for a data file","d","<string>","." );
//    argh.AddArgument("file","Name of a data file","f","<string>","mandatory");

    // Check the user arguments consistancy
    // All mandatory arguments should be provided and
    // There should be no extra arguments

    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}
    // Treat arguments, obtain values to be used later
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}
    if ( argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
    vector<string> vFileNames = argh.GetDataFiles(stringBuf,".bin");
    if ( vFileNames.size() == 0 ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    set<Int_t> NFEB;
    vector<vector<TH1F*>> hFEBCH_HG(SFGD_FEBS_NUM, vector<TH1F*>(SFGD_FEB_NCHANNELS));
    vector<vector<TH1F*>> hFEBCH_LG(SFGD_FEBS_NUM, vector<TH1F*>(SFGD_FEB_NCHANNELS));
    for(int i = 0; i < SFGD_FEBS_NUM; i++){
        for(int j = 0; j < SFGD_FEB_NCHANNELS; j++){
            std::string sCh = "FEB_"+std::to_string(i)+"_Channel_"+std::to_string(j);
            hFEBCH_HG[i][j] = new TH1F((sCh+"_HG").c_str(),sCh.c_str(),  701, 0, 700);
            hFEBCH_LG[i][j] = new TH1F((sCh+"_LG").c_str(),sCh.c_str(),  701, 0, 700);
        }
    }

    BaseLine b;
    for(auto filename : vFileNames){
        string FileOutput =GetLocation(filename.c_str(), ".bin");
        cout << FileOutput <<endl;
        string toErase = "feb_7_11_13_BS_";
        size_t pos = FileOutput.find(toErase);
        if (pos != std::string::npos)
        // If found then erase it from string
            FileOutput.erase(0, pos+toErase.length());
        Int_t HG_LG = atoi(FileOutput.c_str());
        // Going through data file
        File_Reader file_reader;
        file_reader.ReadFile(filename,hFEBCH_HG,hFEBCH_LG);
        // find numbers of measured FEB
        NFEB = file_reader.GetFEBNumbers();
        //get histograms with peaks
        for (const int ih: NFEB) {
            for (Int_t iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
                b.SFGD_BaseLine(hFEBCH_HG[ih][iCh], hFEBCH_LG[ih][iCh], {ih,iCh},HG_LG);
                hFEBCH_HG[ih][iCh]->Reset();
                hFEBCH_LG[ih][iCh]->Reset();

            }
        }
    }
    cout << "Reading files done"<<endl;
    for(int i = 0; i < SFGD_FEBS_NUM; i++){
        for(int j = 0; j < SFGD_FEB_NCHANNELS; j++){
            delete hFEBCH_HG[i][j];
            delete hFEBCH_LG[i][j];
        }
    }

    string rootFileOutput = GetLocation(vFileNames[0].c_str(), ".bin");
    std::map<Elems,std::vector<Baseline_values<Int_t>>> xml_data = b.Find_BaseLine(rootFileOutput);
    XmlReaderWriter xmlFile;
    map<int,vector<AsicData>> tempBoard;
    for(auto xml : xml_data){
        if(xml.second.size() == 2) {
            AsicData asic_data = {xml.first.ASIC_Channel, xml.second[0].par_1, xml.second[1].par_1};
            tempBoard[xml.first.FEB].push_back(asic_data);
        }
        if(xml.second.size() == 1) {
            if (xml.second[0].par_2 == 2.0) {
                AsicData asic_data = {xml.first.ASIC_Channel, xml.second[0].par_1, 0};
                tempBoard[xml.first.FEB].push_back(asic_data);
            }
            if (xml.second[0].par_2 == 3.0) {
                AsicData asic_data = {xml.first.ASIC_Channel, 0, xml.second[0].par_1};
                tempBoard[xml.first.FEB].push_back(asic_data);
            }
        }
    }
    cout << "Drawing baseline done "<<endl;
    for(auto i : tempBoard){
        BoardData tempData;
        tempData.AddAsics(i.first,i.second);
        xmlFile.AddBoard(tempData);
    }
    xmlFile.WriteXml((rootFileOutput+".xml").c_str());

    return 0;
}

