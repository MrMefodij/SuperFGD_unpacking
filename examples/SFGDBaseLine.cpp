#include <iostream>
#include <algorithm>
#include "TGraph.h"
#include <TTree.h>
#include <TROOT.h>
#include "SFGD_defines.h"
#include "Files_Reader.h"
#include "MDargumentHandler.h"
#include "XmlReaderWriter.h"
#include "BaseLineThresholdXmlOutput.h"
#include "BaseLine.h"



int main(int argc, char **argv){

    string stringBuf;
    MDargumentHandler argh("Example of sfgd baseline.");
    argh.Init();

    /// Check the user arguments consistancy
    /// All mandatory arguments should be provided and
    /// There should be no extra arguments

    if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}
    /// Treat arguments, obtain values to be used later
    if ( argh.GetValue("help") ) {argh.Usage(); return 0;}

    if (argh.GetMode() == "f") {
        if (argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else if (argh.GetMode() == "d") {
        if (argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK) return -1;
    } else {
        return -1;
    }

    vector<string> vFileNames = argh.GetDataFiles(stringBuf,".bin");
    std::sort(vFileNames.begin(), vFileNames.end());
    if ( vFileNames.empty() ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    TCanvas *c1 = new TCanvas("c1","",0,10,700,500);

    vector<vector<TH1F*>> hFEBCH_HG(SFGD_SLOT, vector<TH1F*>(SFGD_FEB_NCHANNELS));
    vector<vector<TH1F*>> hFEBCH_LG(SFGD_SLOT, vector<TH1F*>(SFGD_FEB_NCHANNELS));
    for(int i = 0; i < SFGD_SLOT; i++){
        for(int j = 0; j < SFGD_FEB_NCHANNELS; j++){
            std::string sCh = "FEB_"+std::to_string(i)+"_Channel_"+std::to_string(j);
            hFEBCH_HG[i][j] = new TH1F((sCh+"_HG").c_str(),sCh.c_str(),  701, 0, 700);
            hFEBCH_LG[i][j] = new TH1F((sCh+"_LG").c_str(),sCh.c_str(),  701, 0, 700);
        }
    }

    /// Read input files, writes down TH1F for HG/LG for each channel and finds baseline positions for each ASIC.
    BaseLine b;
    vector<int> HG_LG(2);
    File_Reader file_reader;
    set<unsigned int> NFEB;
    for(const std::string& filename : vFileNames){
        string FileOutput =GetLocation(filename, ".bin");
        size_t pos_1 = FileOutput.find("HG");
        size_t pos_2 = FileOutput.find("LG");
        HG_LG[0] = stoi(FileOutput.substr(pos_1 + 2,pos_2 - pos_1 - 3));
        HG_LG[1] = stoi(FileOutput.substr(pos_2 + 2));
        // Going through data file
//        File_Reader file_reader;
        file_reader.ReadFile_for_Baseline(filename,hFEBCH_HG,hFEBCH_LG);
        // find numbers of measured FEB
        if(!NFEB.empty()){
            if(NFEB != file_reader.GetFEBNumbers())
                std::cout << "FEB numbers are different"<<std::endl;
            set_intersection(NFEB.begin(), NFEB.end(), file_reader.GetFEBNumbers().begin(), file_reader.GetFEBNumbers().end(),
                                    std::inserter(NFEB, NFEB.begin()));
        }
        else{
            NFEB = file_reader.GetFEBNumbers();
        }
        //get histograms with peaks
        for (const unsigned int& ih: NFEB) {
            for (unsigned int iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
                if(hFEBCH_HG[ih & 0x0f][iCh]->GetEntries() > 0 && hFEBCH_LG[ih & 0x0f][iCh]->GetEntries() > 0){
                    TH1F* hfull[2] = {hFEBCH_HG[ih & 0x0f][iCh], hFEBCH_LG[ih & 0x0f][iCh]};
                    b.SFGD_BaseLine(hfull, {ih, iCh}, HG_LG);
                }
                else{
                    std::cout << "Problem in file: "<< filename<<" FEB_"<<ih<<"_Channel_"<<iCh<<std::endl;
                }
                hFEBCH_HG[ih & 0x0f][iCh]->Reset();
                hFEBCH_LG[ih & 0x0f][iCh]->Reset();
            }
        }
    }
    cout << "Reading files done"<<endl;
    for(int i = 0; i < SFGD_SLOT; i++){
        for(int j = 0; j < SFGD_FEB_NCHANNELS; j++){
            delete hFEBCH_HG[i][j];
            delete hFEBCH_LG[i][j];
        }
    }

    /// Print Baseline
    string rootFileOutput = GetLocation(vFileNames[0], ".bin");
    auto *wfile = new TFile((rootFileOutput+"_baseline.root").c_str(), "RECREATE");
    b.Print_BaseLine(wfile,vFileNames.size());

    /// Prepare data for creating xml files using baseline study results.
    std::map<Elems,std::vector<Baseline_values<unsigned int>>> xml_data = b.Find_BaseLine(rootFileOutput);
    XmlReaderWriter xmlFile_for_baseline;
    map<unsigned int,vector<AsicData>> tempBoard_for_baseline;
    for(auto xml : xml_data){
        if(xml.second.size() == 2) {
            AsicData asic_data = {xml.first._connection._asicId_channelId, xml.second[0]._par_1, xml.second[1]._par_1};
            tempBoard_for_baseline[xml.first._connection._boardId].push_back(asic_data);
        }
        if(xml.second.size() == 1) {
            if (xml.second[0]._par_2 == 2.0) {
                AsicData asic_data = {xml.first._connection._asicId_channelId, xml.second[0]._par_1, 0};
                tempBoard_for_baseline[xml.first._connection._boardId].push_back(asic_data);
            }
            if (xml.second[0]._par_2 == 3.0) {
                AsicData asic_data = {xml.first._connection._asicId_channelId, 0, xml.second[0]._par_1};
                tempBoard_for_baseline[xml.first._connection._boardId].push_back(asic_data);
            }
        }
    }
    cout << "Drawing baseline done "<<endl;

    /// Write xml file for baseline
    for(const auto& i : tempBoard_for_baseline){
        BoardData<AsicData> tempData;
        tempData.AddAsics(i.first,i.second);
        xmlFile_for_baseline.AddBoard(tempData);
    }
    xmlFile_for_baseline.WriteXml(rootFileOutput+"_baseline.xml");

    /// Prepare data for creating xml files using baseline study results.
    std::map<Connection, unsigned int> baseLine_threshold = b.Get_Recommended_Threshold();
    for(const auto& i : NFEB){
        for(unsigned int j = 0; j < SFGD_FEB_NCHANNELS/32; j++)
        baseLine_threshold.insert(std::pair<Connection,unsigned int>({i,j}, THRESHOLD_VALUE));
    }
    std::map<unsigned int,std::vector<BaseLineThreshold>> tempBoard_for_threshold;
    for(const auto& i : baseLine_threshold){
        tempBoard_for_threshold[i.first._boardId].push_back({i.first._asicId_channelId, i.second });
    }

    /// Write xml file for baseline threshold
    BoardData<BaseLineThreshold> tempBoardData;
    BaseLineThresholdXmlOutput xmlFile_for_threshold;
    for(const auto& i : tempBoard_for_threshold){
        tempBoardData.AddAsics(i.first,i.second);
        xmlFile_for_threshold.AddBoard(tempBoardData);
    }
    xmlFile_for_threshold.WriteXml(rootFileOutput+"_threshold.xml");

    cout << "Writing xml done "<<endl;

    delete wfile;
    delete c1;
    return 0;
}