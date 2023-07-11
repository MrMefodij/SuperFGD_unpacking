#include <iostream>
#include "TGraph.h"
#include <TTree.h>
#include <TROOT.h>
#include "Files_Reader.h"
#include "MDpartEventSFGD.h"
#include "MDargumentHandler.h"
#include "XmlReaderWriter.h"
#include "BaseLine.h"
#include <algorithm>


#define SFGD_NUM 50
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
    if ( vFileNames.empty() ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    set<unsigned int> NFEB;

    vector<vector<TH1F*>> hFEBCH_HG(SFGD_NUM, vector<TH1F*>(SFGD_FEB_NCHANNELS));
    vector<vector<TH1F*>> hFEBCH_LG(SFGD_NUM, vector<TH1F*>(SFGD_FEB_NCHANNELS));
    for(int i = 0; i < SFGD_NUM; i++){
        for(int j = 0; j < SFGD_FEB_NCHANNELS; j++){
            std::string sCh = "FEB_"+std::to_string(i)+"_Channel_"+std::to_string(j);
            hFEBCH_HG[i][j] = new TH1F((sCh+"_HG").c_str(),sCh.c_str(),  701, 0, 700);
            hFEBCH_LG[i][j] = new TH1F((sCh+"_LG").c_str(),sCh.c_str(),  701, 0, 700);
        }
    }

    BaseLine b;
    vector<int> HG_LG(2);
    int i = 0;
    for(const std::string& filename : vFileNames){
        string FileOutput =GetLocation(filename.c_str(), ".bin");
        size_t pos_1 = FileOutput.find("HG");
        size_t pos_2 = FileOutput.find("LG");
        HG_LG[0] = atoi(FileOutput.substr(pos_1+2,pos_2 - pos_1 - 3).c_str());
        HG_LG[1] = atoi(FileOutput.substr(pos_2 + 2).c_str());
        // Going through data file
        File_Reader file_reader;
        file_reader.ReadFile(filename,hFEBCH_HG,hFEBCH_LG);
        // find numbers of measured FEB
        NFEB = file_reader.GetFEBNumbers();
        //get histograms with peaks
        for (unsigned int ih: NFEB) {
            for (unsigned int iCh = 0; iCh < SFGD_FEB_NCHANNELS; iCh++) {
                b.SFGD_BaseLine(hFEBCH_HG[ih][iCh], hFEBCH_LG[ih][iCh], {ih,iCh},HG_LG);
                hFEBCH_HG[ih][iCh]->Reset();
                hFEBCH_LG[ih][iCh]->Reset();

            }
        }
        i++;
    }
    cout << "Reading files done"<<endl;
    for(int i = 0; i < SFGD_NUM; i++){
        for(int j = 0; j < SFGD_FEB_NCHANNELS; j++){
            delete hFEBCH_HG[i][j];
            delete hFEBCH_LG[i][j];
        }
    }
//    std::for_each(hFEBCH_HG.begin(),hFEBCH_HG.end(),[]( vector<TH1F*> a)
//    {
//        std::for_each(a.begin(),a.end(),[] (TH1F* b) {delete b;} );
//    });
//    hFEBCH_HG.clear();
//    hFEBCH_HG.shrink_to_fit();
//    hFEBCH_LG.clear();
//    hFEBCH_LG.shrink_to_fit();
//    destroy(hFEBCH_HG.begin(), hFEBCH_HG.end());
//    destroy(hFEBCH_LG.begin(), hFEBCH_LG.end());
    string rootFileOutput = GetLocation(vFileNames[0].c_str(), ".bin");
    std::map<Elems,std::vector<Baseline_values<int>>> xml_data = b.Find_BaseLine(rootFileOutput);
    XmlReaderWriter xmlFile;
    map<unsigned int,vector<AsicData>> tempBoard;
    for(auto xml : xml_data){
        if(xml.second.size() == 2) {
            AsicData asic_data = {xml.first._asicId_channelId, xml.second[0]._par_1, xml.second[1]._par_1};
            tempBoard[xml.first._boardId].push_back(asic_data);
        }
        if(xml.second.size() == 1) {
            if (xml.second[0]._par_2 == 2.0) {
                AsicData asic_data = {xml.first._asicId_channelId, xml.second[0]._par_1, 0};
                tempBoard[xml.first._boardId].push_back(asic_data);
            }
            if (xml.second[0]._par_2 == 3.0) {
                AsicData asic_data = {xml.first._asicId_channelId, 0, xml.second[0]._par_1};
                tempBoard[xml.first._boardId].push_back(asic_data);
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
    cout << "Writing xml done "<<endl;

    return 0;
}

