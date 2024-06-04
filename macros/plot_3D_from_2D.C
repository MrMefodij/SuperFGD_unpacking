void plot_3D_from_2D(const char* fileName, const char* eventnumber) {

  // Open the ROOT file containing 2D histograms
  TFile* file = new TFile(fileName, "READ");

  // Check if the file is opened successfully
  if (!file || file->IsZombie()) {
      std::cerr << "Error opening the ROOT file." << std::endl;
      return;
  }
   // Create a new ROOT file for saving
   TFile* out = new TFile("eventDisplay_3D.root", "RECREATE");

    // Check if the file is opened successfully
    if (!out || out->IsZombie()) {
      std::cerr << "Error opening the output ROOT file." << std::endl;
      return;
    }

    TDirectory* dir = file->GetDirectory(eventnumber);;

    string dirName = dir->GetName();
    string histNameXY = "Event_HG_"+dirName+"_XY";
          string histNameYZ = "Event_HG_"+dirName+"_YZ";
          string histNameXZ = "Event_HG_"+dirName+"_XZ";
    string histNameT  = "EventTime_"+dirName;

    // Get the TH2F histograms from the file
    TH2* hXY = dynamic_cast<TH2*>(dir->Get(histNameXY.c_str()));
    TH2* hYZ = dynamic_cast<TH2*>(dir->Get(histNameYZ.c_str()));
    TH2* hXZ = dynamic_cast<TH2*>(dir->Get(histNameXZ.c_str()));
    TH1* hT  = dynamic_cast<TH1*>(dir->Get(histNameT.c_str()));

    //Z is X3D, X is Y3D, Y is Z3D

    // Create the TH3F histogram

    TH3F *hist3D = new TH3F("hist3D", "SFGD Event",
                          hYZ->GetNbinsX(), hYZ->GetXaxis()->GetXmin(), hYZ->GetXaxis()->GetXmax(),
                          hXY->GetNbinsX(), hXY->GetXaxis()->GetXmin(), hXY->GetXaxis()->GetXmax(),
                          hXY->GetNbinsY(), hXY->GetYaxis()->GetXmin(), hXY->GetYaxis()->GetXmax());

    // Fill the TH3F histogram with content from the TH2F histograms
    for (Int_t binX = 1; binX <= hist3D->GetNbinsY(); ++binX) {
        for (Int_t binY = 1; binY <= hist3D->GetNbinsZ(); ++binY) {
            for (Int_t binZ = 1; binZ <= hist3D->GetNbinsX(); ++binZ) {
                Double_t limit = 0.0;
                Double_t content = hXY->GetBinContent(binX, binY) +
                                 hYZ->GetBinContent(binZ, binY) +
                                 hXZ->GetBinContent(binX, binZ);
                if (hXY->GetBinContent(binX, binY)<=limit || hYZ->GetBinContent(binZ, binY)<=limit || hXZ->GetBinContent(binX, binZ)<=limit){
                  content = 0.0;
                } //comment this out if statement out for event display with 'fibers'
                if (content<=limit){
                  content = 0.0;
                }
                hist3D->SetBinContent(binZ, binX, binY, content);
            }
        }
    }

  hist3D->GetXaxis()->SetTitle("Z [cm]");
  hist3D->GetYaxis()->SetTitle("X [cm]");
  hist3D->GetZaxis()->SetTitle("Y [cm]");

  out->cd();
  hist3D->Write();
  out->Close();

  // Close the input file
  file->Close();
}
