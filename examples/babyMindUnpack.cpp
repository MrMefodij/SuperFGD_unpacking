#include <exception>
#include <vector>
#include "TFile.h"
#include "TH1D.h"
#include "TSystem.h"
#include "TMacro.h"
#include <TTree.h>
#include "MDfragmentBM.h"
#include "MDpartEventBM.h"
#include "MDargumentHandler.h"
#include "MDdataFile.h"

using namespace std;

string GetLocation(string str)
{
     
    int i = str.rfind("_Slot_");
    string way = str.substr(0,i);  
    return way;
}

struct vectorsTree
{
  vector<double> FEBSN;
  vector<double> SpillNum;
  vector<double> SpillTime;
  vector<double> SpillTimeGTrig;
  vector<double> GTrigTag;
  vector<double> GTrigTime;
  vector<double> hitsChannel;
  vector<double> hitAmpl;
  vector<double> hitLGAmpl;
  vector<double> hitLeadTime;
  vector<double> hitTrailTime;
  vector<double> hitTimeDif;
  vector<double> hitTimefromSpill;
  vector<double> SpillTrailTime;

};



char *dataBuff;
uint32_t* dataPtr;

int main( int argc, char **argv ) {
 string sFileName;
  
  vector<string> vFileNames;
  ifstream fList("febs_files_list.list");
  while (!fList.eof()) {
    fList >> sFileName;
    //cout << sFileName << endl;
    vFileNames.push_back(sFileName);
  }
  vFileNames.pop_back();
  
  for (int i = 0; i< vFileNames.size();i++){
      cout << vFileNames.at(i)<<endl;
  }
  
   vectorsTree FEB[30];
   
   string rootFileOutput=GetLocation(vFileNames[0].c_str());
  rootFileOutput+="_all.root";
  cout << rootFileOutput<<endl;
  
  TFile rfile(rootFileOutput.c_str(), "recreate");
  
  TTree* FEBtree[65];
  
  ostringstream sFEBnum;
  string sFEB;

  for (Int_t ih=0; ih<=30; ih++) {
        sFEBnum.str("");
        sFEBnum << ih;
        sFEB = "FEB_"+sFEBnum.str();
        FEBtree[ih] = new TTree(sFEB.c_str(),sFEB.c_str()); 
        
        FEBtree[ih]->Branch((sFEB+"_SN").c_str(),"vector<double>",&FEB[ih].FEBSN);
        FEBtree[ih]->Branch((sFEB+"_SpillTag").c_str(),"vector<double>",&FEB[ih].SpillNum);
        FEBtree[ih]->Branch((sFEB+"_SpillTime").c_str(),"vector<double>",&FEB[ih].SpillTime);
        FEBtree[ih]->Branch((sFEB+"_SpillTimeGTrig").c_str(),"vector<double>",&FEB[ih].SpillTimeGTrig);
        FEBtree[ih]->Branch((sFEB+"_GTrigTag").c_str(),"vector<double>",&FEB[ih].GTrigTag);
        FEBtree[ih]->Branch((sFEB+"_GTrigTime").c_str(),"vector<double>",&FEB[ih].GTrigTime);
        FEBtree[ih]->Branch((sFEB+"_hitsChannel").c_str(),"vector<double>",&FEB[ih].hitsChannel);
        FEBtree[ih]->Branch((sFEB+"_hitAmpl").c_str(),"vector<double>",&FEB[ih].hitAmpl);
        FEBtree[ih]->Branch((sFEB+"_hitLGAmpl").c_str(),"vector<double>",&FEB[ih].hitLGAmpl);
        FEBtree[ih]->Branch((sFEB+"_hitLeadTime").c_str(),"vector<double>",&FEB[ih].hitLeadTime);
        FEBtree[ih]->Branch((sFEB+"_hitTrailTime").c_str(),"vector<double>",&FEB[ih].hitTrailTime);
        FEBtree[ih]->Branch((sFEB+"_hitTimeDif").c_str(),"vector<double>",&FEB[ih].hitTimeDif);
        FEBtree[ih]->Branch((sFEB+"_hitTimefromSpill").c_str(),"vector<double>",&FEB[ih].hitTimefromSpill);
        FEBtree[ih]->Branch((sFEB+"_SpillTrailTime").c_str(),"vector<double>",&FEB[ih].SpillTrailTime);
        //FEBtree[ih]->Branch((sFEB+"_SpillTemperature").c_str(), "vector<double>",&FEB[ih].SpillTemperature);
  }
  
  for (vector<string>::iterator itFileName=vFileNames.begin(); itFileName != vFileNames.end(); itFileName++) {
    sFileName = *itFileName;
    cout <<endl<< sFileName << endl;
    
    ifstream finData(sFileName.c_str());
    string stringBuf;
    string filepath;
    string filename;
  
  // The following shows how to use the MDargumentHandler class
  // to deal with the main arguments
  // Define the arguments
  MDargumentHandler argh("Example of unpacking application.");
  argh.AddArgument("help","print this message","h");
  argh.AddArgument("directory","path for the data file","d","<string>","." );
  argh.AddArgument("file","Name of a data file","f","<string>","mandatory");
  

  // Check the user arguments consistancy
  // All mandatory arguments should be provided and
  // There should be no extra arguments
  //if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return -1;}

  // Treat arguments, obtain values to be used later
  if ( argh.GetValue("help") ) {argh.Usage(); return 0;}
  if ( argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
  filepath = stringBuf;
  if ( argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
  filename = stringBuf;
  filename = sFileName;
  string rootFilename = sFileName;
 
  
  
  //TFile rfile("histos.root", "recreate");

 
  
  for (Int_t i=0;i<=30;i++){
    FEB[i].FEBSN.clear();
    FEB[i].SpillNum.clear();
    FEB[i].SpillTime.clear();
    FEB[i].SpillTimeGTrig.clear();
    FEB[i].hitsChannel.clear();
    FEB[i].hitAmpl.clear();
    FEB[i].hitLeadTime.clear();
    FEB[i].GTrigTag.clear();
    FEB[i].GTrigTime.clear();
    FEB[i].hitLGAmpl.clear();
    FEB[i].hitTrailTime.clear();
    FEB[i].hitTimeDif.clear();
    FEB[i].hitTimefromSpill.clear();
    FEB[i].SpillTrailTime.clear();
    //FEB[i].SpillTemperature.clear();
    }
  
  MDdateFile dfile(filename);
// Open the file and loop over events.
  Int_t BordID=0;
  char *eventBuffer;
  bool _previousSpillTagExist = false;
  unsigned int _previousSpillTag = 0;
  if ( dfile.open() ) { // There is a valid files to unpack
    dfile.init();

    int xEv(0);
    do { // Loop over all spills
      eventBuffer =  dfile.GetNextEvent();
      try {
        MDfragmentBM   spill;
       
        spill.SetPreviousSpill(_previousSpillTagExist,_previousSpillTag);
        spill.SetDataPtr(eventBuffer);
        
        MDpartEventBM *event;
        int nTr = spill.GetNumOfTriggers();
        BordID = (Int_t)spill.GetBoardId();
        
        for (int i=0; i<nTr; ++i) {
          event = spill.GetTriggerEventPtr(i);
          //event->Dump();
          // if (spill.GetSpillTag()!=156){
          for (int ich=0; ich<BM_FEB_NCHANNELS; ++ich) {
              int nlHits = 0;
              int ntHits = 0;
              nlHits = event->GetNLeadingEdgeHits(ich);
              for (unsigned int ih=0; ih<nlHits; ++ih) {
                  bool TrailTimeExist = false;
                  int IDevent = event->GetHitTimeId(ih, ich, 'l');
                  FEB[spill.GetBoardId()].FEBSN.push_back(spill.GetBoardId());
                  FEB[spill.GetBoardId()].SpillNum.push_back(spill.GetSpillTag());
                    _previousSpillTagExist = true;
                    _previousSpillTag = spill.GetSpillTag();
                  FEB[spill.GetBoardId()].SpillTimeGTrig.push_back(spill.GetSpillTimeGTrig());
                  FEB[spill.GetBoardId()].SpillTime.push_back(spill.GetSpillTime());
                  FEB[spill.GetBoardId()].GTrigTag.push_back(event->GetTriggerTag());
                  FEB[spill.GetBoardId()].GTrigTime.push_back(event->GetTriggerTime());
                  FEB[spill.GetBoardId()].hitsChannel.push_back(ich);
                  FEB[spill.GetBoardId()].hitLeadTime.push_back(event->GetLeadingTime(ih, ich));
                   if (FEB[spill.GetBoardId()].SpillTimeGTrig.back() > 516){
                        FEB[spill.GetBoardId()].hitTimefromSpill.push_back(-4 * FEB[spill.GetBoardId()].SpillTimeGTrig.back() + 4000* (1 + FEB[spill.GetBoardId()].GTrigTime.back()) + event->GetLeadingTime(ih, ich));
                   }
                   else {
                        FEB[spill.GetBoardId()].hitTimefromSpill.push_back(-4 * FEB[spill.GetBoardId()].SpillTimeGTrig.back() + 4000* ( FEB[spill.GetBoardId()].GTrigTime.back()) + event->GetLeadingTime(ih, ich));
                   }
                  FEB[spill.GetBoardId()].SpillTrailTime.push_back(spill.GetSpillTrailTime() );
                  //FEB[spill.GetBoardId()].SpillTemperature.push_back(spill.GetSpillTrailTemp());
                  
                  ntHits = event->GetNTrailingEdgeHits(ich);
                  for(unsigned int ith=0; ith < ntHits; ith++){
                      if (IDevent==event->GetHitTimeId(ith, ich, 't') && TrailTimeExist==false && event->GetTrailingTime(ith, ich) >= FEB[spill.GetBoardId()].hitLeadTime.back()){
                          FEB[spill.GetBoardId()].hitTrailTime.push_back(event->GetTrailingTime(ith, ich));
                          FEB[spill.GetBoardId()].hitTimeDif.push_back(FEB[spill.GetBoardId()].hitTrailTime.back() - FEB[spill.GetBoardId()].hitLeadTime.back() );
                          TrailTimeExist = true;
                      }
                  }
                  if (TrailTimeExist==false){
                      if (i+1 <nTr){
                        event = spill.GetTriggerEventPtr(i+1);
                        ntHits = event->GetNTrailingEdgeHits(ich);
                        unsigned int ith=0;
                            while (TrailTimeExist==false && ith < ntHits){
                                if (IDevent==event->GetHitTimeId(ith, ich, 't')){
                                    FEB[spill.GetBoardId()].hitTrailTime.push_back(event->GetTrailingTime(ith, ich));
                                    FEB[spill.GetBoardId()].hitTimeDif.push_back(FEB[spill.GetBoardId()].hitTrailTime.back() - FEB[spill.GetBoardId()].hitLeadTime.back() + 4000);
                                    TrailTimeExist = true;
                                }
                            ith++;
                            }
                        event = spill.GetTriggerEventPtr(i);
                      }
                  }
                  
                 if (TrailTimeExist==false){
                    FEB[spill.GetBoardId()].hitTrailTime.push_back(-1);
                    FEB[spill.GetBoardId()].hitTimeDif.push_back(-1);
                    TrailTimeExist = true;
                }
                    
                    
                  
                  
                  bool HGAmplExist = false;
                  
                  if (IDevent == event-> GetHitAmplitudeId(ich, 'h') ){
                      FEB[spill.GetBoardId()].hitAmpl.push_back(event->GetHitAmplitude(ich, 'h'));
                      HGAmplExist = true;
                  }
                  
                  if ( HGAmplExist == false){
                      if (i+1 <nTr){
                        event = spill.GetTriggerEventPtr(i+1);
                        if (event-> HGAmplitudeHitExists(ich)){
                            if (IDevent==event-> GetHitAmplitudeId(ich, 'h')){
                                FEB[spill.GetBoardId()].hitAmpl.push_back(event->GetHitAmplitude(ich, 'h'));
                                HGAmplExist = true;
                            }
                        }
                      event = spill.GetTriggerEventPtr(i);
                    }
                  }
                  if (HGAmplExist==false){
                        FEB[spill.GetBoardId()].hitAmpl.push_back(0);
                        HGAmplExist = true;
                  }
                  
                  bool LGAmplExist = false;
                  if (IDevent==event-> GetHitAmplitudeId(ich, 'l')){
                      FEB[spill.GetBoardId()].hitLGAmpl.push_back(event->GetHitAmplitude(ich, 'l'));
                      LGAmplExist = true;
                  }
                  if ( LGAmplExist == false){
                      if (i+1 <nTr){
                            event = spill.GetTriggerEventPtr(i+1);
                            if (event-> LGAmplitudeHitExists(ich)){
                                if (IDevent==event-> GetHitAmplitudeId(ich, 'l')){
                                    FEB[spill.GetBoardId()].hitLGAmpl.push_back(event->GetHitAmplitude(ich, 'l'));
                                    LGAmplExist = true;
                                }
                            }
                            event = spill.GetTriggerEventPtr(i);
                      }
                  }
                  if ( LGAmplExist == false){
                        FEB[spill.GetBoardId()].hitLGAmpl.push_back(0);
                  }
                
              if (FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].SpillNum.size()          ||
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].hitsChannel.size()       || 
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].SpillTimeGTrig.size()    ||
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].hitAmpl.size()           || 
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].hitLeadTime.size()       || 
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].GTrigTag.size()          ||  
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].GTrigTime.size()         ||  
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].hitLGAmpl.size()         || 
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].hitTrailTime.size()      ||
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].hitTimeDif.size()        ||
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].hitTimefromSpill.size()  ||
                  FEB[spill.GetBoardId()].FEBSN.size() !=  FEB[spill.GetBoardId()].SpillTrailTime.size()) {
                  
                 cout <<FEB[spill.GetBoardId()].GTrigTag.back()<<endl;
                 cout <<"CRASH: " << FEB[spill.GetBoardId()].FEBSN.size()<< " " <<FEB[spill.GetBoardId()].SpillNum.size()<<" "<<FEB[spill.GetBoardId()].hitAmpl.size()<<" " <<FEB[spill.GetBoardId()].hitTrailTime.size()<<endl;
                 return 0;
                 }
                  
              }
          }
          //}
        }
      } catch (MDexception & lExc)  {
        std::cerr <<  lExc.GetDescription() << endl
                  << "Unpacking exception\n"
                  << "Spill skipped!\n\n";
                   
        FEB[BordID].FEBSN.push_back(BordID);
        FEB[BordID].SpillNum.push_back(_previousSpillTag + 1);
        FEB[BordID].SpillTime.push_back(-1);
        FEB[BordID].SpillTimeGTrig.push_back(-1);
        FEB[BordID].hitsChannel.push_back(-1);
        FEB[BordID].hitAmpl.push_back(-1);
        FEB[BordID].hitLeadTime.push_back(-1);
        FEB[BordID].GTrigTag.push_back(-1);
        FEB[BordID].GTrigTime.push_back(-1);
        FEB[BordID].hitLGAmpl.push_back(-1);
        FEB[BordID].hitTrailTime.push_back(-1);
        FEB[BordID].hitTimeDif.push_back(-1);
        FEB[BordID].hitTimefromSpill.push_back(-1);
        FEB[BordID].SpillTrailTime.push_back(-1);
        
        FEBtree[BordID]->Fill();
        cout<<"Number of events on FEB "<< BordID <<" is "<< FEB[BordID].FEBSN.size()<<endl;
        
        FEB[BordID].FEBSN.clear();
        FEB[BordID].SpillNum.clear();
        FEB[BordID].SpillTime.clear();
        FEB[BordID].SpillTimeGTrig.clear();
        FEB[BordID].hitsChannel.clear();
        FEB[BordID].hitAmpl.clear();
        FEB[BordID].hitLeadTime.clear();
        FEB[BordID].hitTrailTime.clear();
        FEB[BordID].hitTimeDif.clear();
        FEB[BordID].GTrigTag.clear();
        FEB[BordID].GTrigTime.clear();
        FEB[BordID].hitLGAmpl.clear();
        FEB[BordID].hitTimefromSpill.clear();
        FEB[BordID].SpillTrailTime.clear();

      } catch(std::exception & lExc) {
        std::cerr << lExc.what() << std::endl
                  << "Standard exception\n"
                  << "Spill skipped!\n\n";
      } catch(...) {
        std::cerr << "Unknown exception occurred...\n"
                  << "Spill skipped!\n\n";
      }
      if (FEB[BordID].FEBSN.size()){
          cout<<"Number of events on FEB "<< BordID <<" is "<< FEB[BordID].FEBSN.size()<<endl;
        FEBtree[BordID]->Fill();
    //    FEBtree[BordID]-> Write("",TObject::kOverwrite);
        //FEBtree[BordID]-> Write();
   //     FEBtree[BordID]->Delete();
        FEB[BordID].FEBSN.clear();
        FEB[BordID].SpillNum.clear();
        FEB[BordID].SpillTime.clear();
        FEB[BordID].SpillTimeGTrig.clear();
        FEB[BordID].hitsChannel.clear();
        FEB[BordID].hitAmpl.clear();
        FEB[BordID].hitLeadTime.clear();
        FEB[BordID].hitTrailTime.clear();
        FEB[BordID].hitTimeDif.clear();
        FEB[BordID].GTrigTag.clear();
        FEB[BordID].GTrigTime.clear();
        FEB[BordID].hitLGAmpl.clear();
        FEB[BordID].hitTimefromSpill.clear();
        FEB[BordID].SpillTrailTime.clear();

      }

      ++xEv;
       } while (xEv < 500);
    //} while ( eventBuffer );
  }
  
  FEBtree[BordID]-> Write("",TObject::kOverwrite);
  FEBtree[BordID]->Delete();
    
  dfile.close();
  delete dataBuff;
  }
  rfile.Close();
  fList.close();
  return 0;
}



