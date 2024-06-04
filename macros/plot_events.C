#include "plot_crates.C"
void plot_events(const char* fileWithPath, string runSubrun, string plotType="", string triggerType="beam", string outPath="./", bool plotCrates = false) {
  // Arguments:
  // fileWithPath = input file name (and path)
  // runSubrun = run and subrun numbers e.g. "00000768_0019", used in output file name
  // Options:
  // plotType = whether to plot with ToT "", HG "HG_", LG "LG_", or pe "pe_"
  // triggerType = trigger used in run e.g. "beam" or "cosmics"
  // outPath = path to directory to save output file
  // plotCrates = whether to plot crate boundaries and hash uninstrumented regions (true) or not (false)

  gROOT->SetBatch(kTRUE);
  
  // Open the ROOT file
  TFile* file = new TFile(fileWithPath, "READ");

  // Create a canvas with four pads
  TCanvas* canvas = new TCanvas("canvas", "Separate Histograms", 800, 600);
  canvas->Divide(2,2);
  gStyle->SetOptStat(0);
  canvas->Print(Form("%s/%s_events_%s%s.pdf[",outPath.c_str(),triggerType.c_str(),plotType.c_str(),runSubrun.c_str()));
    
  // Check if the file is open successfully
  if (!file || file->IsZombie()) {
    std::cerr << "Error opening file: " << fileWithPath << std::endl;
    return;
  }

  // Get the list of keys in the top-level directory
  TList* keys = file->GetListOfKeys();

  // Loop through the keys
  TIter next(keys);
  TKey* key;
  while ((key = dynamic_cast<TKey*>(next()))) {
    // Check if the key corresponds to a directory
    if (key->IsFolder()) {
      // Convert the key to a TDirectoryFile
      TDirectoryFile* dir = dynamic_cast<TDirectoryFile*>(key->ReadObj());

      string dirName = dir->GetName();
      string histNameXY = "Event_"+plotType+dirName+"_XY";
      string histNameYZ = "Event_"+plotType+dirName+"_YZ";
      string histNameXZ = "Event_"+plotType+dirName+"_XZ";
      string histNameT  = "EventTime_"+dirName;

      if(!dir->Get(histNameXY.c_str())) continue;
	    
      TH2F* hXY = dynamic_cast<TH2F*>(dir->Get(histNameXY.c_str()));
      TH2F* hYZ = dynamic_cast<TH2F*>(dir->Get(histNameYZ.c_str()));
      TH2F* hXZ = dynamic_cast<TH2F*>(dir->Get(histNameXZ.c_str()));
      TH1F* hT  = dynamic_cast<TH1F*>(dir->Get(histNameT.c_str()));

      TH2F* hZY = swap_histogram_axis(hYZ);
	    
      canvas->cd(1);
      gPad->SetRightMargin(0.2);
      hZY->Draw("COLZ");
      if (plotCrates) crate_boundary_YZ_swapped();

      canvas->cd(2);
      gPad->SetRightMargin(0.2);
      hXZ->Draw("COLZ");
      if (plotCrates){
	crate_boundary_XZ();
	slash_crate_0_4_8_XZ();
      }

      canvas->cd(3);
      hT->Draw("HIST");

      canvas->cd(4);
      gPad->SetRightMargin(0.2);
      hXY->Draw("COLZ");
      if (plotCrates){
	crate_boundary_XY();
	slash_crate_1_XY();
      }

      canvas->Draw();
      canvas->Print(Form("%s/%s_events_%s%s.pdf",outPath.c_str(),triggerType.c_str(),plotType.c_str(),runSubrun.c_str()));
	    
      // Delete the TDirectoryFile object if needed
      delete dir;
    }
  }

  canvas->Print(Form("%s/%s_events_%s%s.pdf]",outPath.c_str(),triggerType.c_str(),plotType.c_str(),runSubrun.c_str()));

  // Close the file
  file->Close();
  delete file;
}
