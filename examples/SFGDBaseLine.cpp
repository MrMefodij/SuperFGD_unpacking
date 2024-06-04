//
//  Created by Maria on 11.07.2023 kolupanova@inr.ru
//

#include <iostream>
#include <algorithm>
#include <thread>
#include "TGraph.h"
#include <TTree.h>
#include <TROOT.h>
#include "SFGD_defines.h"
#include "BaseLine.h"
#include "Files_Reader.h"
#include "MDargumentHandler.h"
#include "XmlReaderWriter.h"

#include "CrateSlotAsic_missingChs.h"


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

    /// Missing channels (read txt)
    std::string missingFile = "../connection_map/crateSlotAsic_missingChs.txt";
    if (getenv("UNPACKING_ROOT") != nullptr) {
        missingFile = (std::string) getenv("UNPACKING_ROOT") + "/connection_map/crateSlotAsic_missingChs.txt";
    }
    CrateSlotAsic_missingChs crateSlotAsicMissingChs(missingFile);
    try {
        crateSlotAsicMissingChs.Init();
    } catch (const std::exception &e) {
        std::cerr << "Unable to open file " << missingFile << std::endl;
        exit(1);
    }

    std::ofstream fout((stringBuf+"/statistic.txt").c_str());
    vector<string> vFileNames = argh.GetDataFiles(stringBuf,".bin");
    std::sort(vFileNames.begin(), vFileNames.end());
    if ( vFileNames.empty() ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }
    unsigned int CURRENT_NUM_FEBs;
    std::cout << "Write number of crates (one or more):"<<std::endl;
    std::cin >> CURRENT_NUM_FEBs;

    /// Create TH1F for each SFGD_FEB_NCHANNELS in SFGD_SLOT
    if (CURRENT_NUM_FEBs == 1)
        CURRENT_NUM_FEBs = SFGD_SLOT;
    else if (CURRENT_NUM_FEBs > 1)
        CURRENT_NUM_FEBs = SFGD_FEBS_NUM;

    /// Read input files, writes down TH1F for HG/LG for each channel and finds baseline positions for each ASIC.
    BaseLine b(CURRENT_NUM_FEBs, vFileNames.size());

    std::thread th[vFileNames.size()];
    for(int i = 0; i < vFileNames.size(); ++i){
        th[i] = std::thread(&BaseLine::doBaselineStudy,  std::ref(b), vFileNames[i], i, std::ref(crateSlotAsicMissingChs));
    }
    for(int i = 0; i < vFileNames.size(); ++i){
        th[i].join();
    }
    std::cout << "Reading files done"<< std::endl;

    /// Print Baseline
    string rootFileOutput = GetLocation(vFileNames[0], ".bin");
    auto *wfile = new TFile((stringBuf+"/baseline.root").c_str(), "RECREATE");
    b.Print_BaseLine(wfile,vFileNames.size());
    auto bl = b.GetBaselineValues();
//    for(const auto& i: bl){
//        std::cout <<i.first._DAC << " FEB " <<i.first._connection._boardId <<" ASIC "<<i.first._connection._asicId_channelId<<": "<<32 - i.second.size()<<std::endl;
//    }

    /// Prepare data for creating xml files using baseline study results.
    std::map<Elems,std::vector<Baseline_values<unsigned int>>> xml_data = b.Find_BaseLine(stringBuf);
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
    xmlFile_for_baseline.WriteXml(stringBuf+"/baseline.xml");

    cout << "Writing xml done "<<endl;
    wfile->Close();
    return 0;
}