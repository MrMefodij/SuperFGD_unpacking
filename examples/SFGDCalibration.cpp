//
//  Created by Maria on 27.09.2022 kolupanova@inr.ru
//
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <numeric>
#include <map>
#include <TH2F.h>

#include <TCanvas.h>
#include <TTree.h>
#include <TF1.h>
#include <TLine.h>
#include <TROOT.h>
#include "Files_Reader.h"
#include "Calibration.h"
#include "SFGD_defines.h"
#include "MDargumentHandler.h"
#include "Connection_Map.h"
#include "CrateSlotAsic_missingChs.h"
#include "OutputHandler.h"
#include "PrintCrates.h"


int main(int argc, char **argv){
    std::string stringBuf;

    /// The following shows how to use the MDargumentHandler class
    /// to deal with the main arguments
    /// Define the arguments

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
    vector<string> vFileNames  = argh.GetDataFiles(stringBuf,".bin");

    if ( vFileNames.empty()) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    if ( argh.GetValue("output_directory", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
    string outputdir=stringBuf;
    OutputHandler outh;
    //outh.SetVerbose(1);

    /// If crate number == 1 => slot number will be used instead of board_id, if crate number > 1 => board_id will be used
    unsigned int CURRENT_NUM_FEBs;
    std::cout << "Write number of crates (one or more):"<<std::endl;
    std::cin >> CURRENT_NUM_FEBs;

    bool Calibrate = true;
    std::string s_calib = "" ;
    std::cout << "Do you need calibration (yes/no):"<<std::endl;
    std::cin >> s_calib;
    if(s_calib == "no"){
        Calibrate = false;
    }
    /// Create TH1F for each SFGD_FEB_NCHANNELS in SFGD_SLOT
    try {
        if (CURRENT_NUM_FEBs == 1)
            CURRENT_NUM_FEBs = SFGD_SLOT;
        else if (CURRENT_NUM_FEBs > 1)
            CURRENT_NUM_FEBs = SFGD_FEBS_NUM;
    } catch (...) {
        std::cout << "Write another value"<<std::endl;
        exit(1);
    }

    /// Create Canvas
    TCanvas *c1 = new TCanvas("c1", "", 0, 10, 700, 500);

    /// Connection map (read csv)
    string mapFile = "../connection_map/SFG_Geometry_Map_v18.csv";
    if (getenv("UNPACKING_ROOT") != nullptr) {
        mapFile = (string) getenv("UNPACKING_ROOT") + "/connection_map/SFG_Geometry_Map_v18.csv";
    }
    Connection_Map connectionMap(mapFile);
    try {
        connectionMap.Init();
    } catch (const exception &e) {
        std::cerr << "Unable to open file " << mapFile << std::endl;
        exit(1);
    }

    /// Missing channels (read txt)
    string missingFile = "../connection_map/crateSlotAsic_missingChs.txt";
    if (getenv("UNPACKING_ROOT") != nullptr) {
        missingFile = (string) getenv("UNPACKING_ROOT") + "/connection_map/crateSlotAsic_missingChs.txt";
    }
    CrateSlotAsic_missingChs crateSlotAsicMissingChs(missingFile);
    try {
        crateSlotAsicMissingChs.Init();
    } catch (const exception &e) {
        std::cerr << "Unable to open file " << missingFile << std::endl;
        exit(1);
    }

    for(const auto& fileName : vFileNames) {

        std::ifstream ifs((fileName).c_str());
        if (ifs.fail()) {
            std::cerr << "Can not open file " << fileName << std::endl;
            return 1;
        }

        string outputFilePrefix = outh.GetOutputFilePrefix(fileName, outputdir);

        string rootFileOutput = outputFilePrefix + "_channels_signal.root";
        string txtFileOutput = outputFilePrefix + ".txt";
        std::cout << "ROOT output: " << rootFileOutput << endl;
        std::cout << "TXT output:  " << txtFileOutput << endl;

        std::ofstream fout(txtFileOutput.c_str());

        /// Create root file
        TFile *wfile = new TFile(rootFileOutput.c_str(), "RECREATE");

        /// Histograms preparation
        vector<vector<TH1F *>> hFEBCH(CURRENT_NUM_FEBs, vector<TH1F *>(SFGD_FEB_NCHANNELS));
        for (int i = 0; i < CURRENT_NUM_FEBs; ++i) {
            for (int j = 0; j < SFGD_FEB_NCHANNELS; ++j) {
                std::string sCh;
                if (CURRENT_NUM_FEBs == SFGD_SLOT)
                    sCh = "SLOT_";
                else {
                    sCh = "FEB_";
                }
                sCh += std::to_string(i) + "_Channel_" + std::to_string(j);
                hFEBCH[i][j] = new TH1F(sCh.c_str(), sCh.c_str(), 4096, 0, 4096);
            }
        }

        TH2F* event_Upstream = new TH2F("Event_XY", "",Y_SIZE, 0, Y_SIZE, X_SIZE, 0, X_SIZE);
        TH2F* event_Side = new TH2F("Event_YZ", "",Z_SIZE, 0, Z_SIZE,Y_SIZE, 0, Y_SIZE);
        TH2F* event_Top = new TH2F("Event_XZ", "",Z_SIZE, 0, Z_SIZE, X_SIZE, 0, X_SIZE);

        /// Prepare TTree
        TTree *tree = new TTree("tree", "ttree");
        unsigned int crate, feb, slot, channel;
        double gain, gain_error, mean, median;
        unsigned int x, y, z, position, asic, lgpposition_id;
        string pcbPosition, lgpposition;
        tree->Branch("crate", &crate, "crate/i");
        tree->Branch("feb", &feb, "feb/i");
        tree->Branch("slot", &slot, "slot/i");
        tree->Branch("channel", &channel, "channel/i");
        tree->Branch("gain", &gain, "gain/D");
        tree->Branch("gain_error", &gain_error, "gain_error/D");
        tree->Branch("mean", &mean, "mean/D");
        tree->Branch("median", &median, "median/D");
        tree->Branch("x", &x, "x/i");
        tree->Branch("y", &y, "y/i");
        tree->Branch("z", &z, "z/i");
        tree->Branch("lgpposition_id", &lgpposition_id, "lgpposition_id/i");
        tree->Branch("lgpposition", &lgpposition);
        tree->Branch("asic", &asic, "asic/i");
        tree->Branch("position", &position, "position/i");
        tree->Branch("pcbPosition", &pcbPosition);

        /// Going through data file
        Calibration cl;
        File_Reader file_reader;

        std::cout << "Decoding the input file..." << std::endl;

        /// parameter HG_LG: for HG - 2, for LG - 3, default - 2
        /// HG_LG - is used in file reader if HG_LG == 2 -> HG, if HG_LG == 3 -> LG, by default file reader uses HG
        file_reader.ReadFile_for_Calibration(fileName, hFEBCH, HG_LG_input::HG);

        /// Find numbers of measured FEB
        set<unsigned int> NFEB = file_reader.GetFEBNumbers();
        std::cout << "Reading done\n";
        unsigned int start_slot = *NFEB.begin() & 0x0f;
        TH2F *GainMap = new TH2F("Gain_results_for_crate", "Gain results for crate", 256, (int) 0, (int) 256,
                                 NFEB.size() - start_slot, (int) *NFEB.begin() - start_slot,
                                 (int) *NFEB.begin() + NFEB.size() - start_slot);
        TH1F *hGain = new TH1F("Gain_distribution", "Gain_distribution", 400, 0, 100);
        /// Get histograms with peaks
        unsigned int empty_channels = 0;
        unsigned int curr_crate = *NFEB.begin() >> 4;
        std::cout << "Processing: crate " << curr_crate << std::endl;;
        TDirectory *Cratedir = wfile->mkdir(("Crate " + to_string(curr_crate)).c_str());
        for (unsigned int ih: NFEB) {
            unsigned int crate_number = ih >> 4;
            TH1F *allFEBs = new TH1F(("GetEntriesInFEB" + to_string(ih)).c_str(),
                                     ("GetEntriesInFEB" + to_string(ih)).c_str(), SFGD_FEB_NCHANNELS, 0,
                                     SFGD_FEB_NCHANNELS);

            if (crate_number != curr_crate) {
                Cratedir = wfile->mkdir(("Crate " + to_string(crate_number)).c_str());
                curr_crate = crate_number;
                std::cout << "Processing: crate " << curr_crate << std::endl;;
            }

            TDirectory *FEBdir = Cratedir->mkdir((" FEB " + to_string(ih)).c_str());
            unsigned int slot_id = ih & 0x0f;
            FEBdir->cd();
            for (unsigned int iCh = 0; iCh < SFGD_FEB_NCHANNELS; ++iCh) {
                Mapping map = {crate_number, slot_id, iCh / 32};
                unsigned int hfeb_num;
                if (CURRENT_NUM_FEBs == SFGD_SLOT) hfeb_num = slot_id;
                else {
                    hfeb_num = ih;
                }
                string feb_channel = "FEB_" + to_string(ih) + "_Channel_" + to_string(iCh);
                if (!crateSlotAsicMissingChs.Is_Missing_Chs(map, iCh) &&
                    !crateSlotAsicMissingChs.Is_Missing_FEB(ih) && !crateSlotAsicMissingChs.Is_Missing_ASIC(map)) {
                    if (hFEBCH[hfeb_num][iCh]->GetEntries() > 0) {
//                    std::cout << hFEBCH[hfeb_num][iCh]->GetEntries()<<std::endl;
                        allFEBs->SetBinContent(iCh + 1, hFEBCH[hfeb_num][iCh]->GetEntries());
                        /// for TTree
                        auto glgeom = connectionMap.GetGlobalGeomPositionPtr(ih, iCh);
                        auto glch = connectionMap.GetGlobalChannelPositionPtr(ih, iCh);
                        if (glgeom == nullptr || glch == nullptr) {
                            std::cerr << "Not in the map. Skipped: " << feb_channel << endl;
                            continue;
                        }
                        std::string pcbPosition = connectionMap.GetPcbPosition(hfeb_num);

                        if (pcbPosition.find("U-") != pcbPosition.npos) {
                            event_Upstream->Fill(glgeom->y_, glgeom->x_, hFEBCH[hfeb_num][iCh]->GetEntries());
                        } else if (pcbPosition.find("L-") != pcbPosition.npos ||
                                    pcbPosition.find("R-") != pcbPosition.npos) {
                            event_Side->Fill(glgeom->z_, glgeom->y_, hFEBCH[hfeb_num][iCh]->GetEntries());
                        } else if (pcbPosition.find("T-") != pcbPosition.npos) {
                            event_Top->Fill(glgeom->z_, glgeom->x_, hFEBCH[hfeb_num][iCh]->GetEntries());
                        }
                        if (Calibrate) {
                            cl.SFGD_Calibration(hFEBCH[hfeb_num][iCh], feb_channel);
                            TGraphErrors *gr = new TGraphErrors;
                            cl.Gain_Calculation(gr, feb_channel);
                            auto peaks = cl.Calibration_Par();

                            gain = cl.GetGain();
                            gain_error = cl.GetGainError();

                            if (gain != 0) hGain->Fill(gain);
                            /// Add entry into Gain distribution
                            GainMap->Fill(iCh, ih, cl.GetGain());
///
                            auto *l_median = cl.Calibration_Line_Median();
                            auto *l_mean = cl.Calibration_Line_Mean();
                            l_median->Draw();
                            l_mean->Draw();
                            auto *legend = cl.Calibration_Legend();
                            legend->Draw();
                            FEBdir->cd();
                            c1->Update();
                            c1->Write(feb_channel.c_str());
                            gr->Draw("APE");
                            c1->Update();
                            c1->Write((feb_channel + "_graph").c_str());
                            // DO NOT delete these lines to save histogram into ROOT file
                            FEBdir->cd();
                            hFEBCH[hfeb_num][iCh]->Write((feb_channel + "_hist").c_str());

                            unsigned int global_channel = (ih << 8) | iCh;
                            auto calib_pars = cl.Calibration_Par();
                            fout << ih << " " << iCh << " " << gain << " " << gain_error;
                            for (auto par: calib_pars)
                                fout << " " << par.GetPosition();
                            fout << std::endl;
                            delete gr;
                        }
                        delete hFEBCH[hfeb_num][iCh];
                        /// Add tree element
                        crate = crate_number;
                        feb = ih;
                        slot = slot_id;
                        channel = iCh;
                        mean = cl.GetMean();
                        median = cl.GetMedian();
                        x = glgeom->x_;
                        y = glgeom->y_;
                        z = glgeom->z_;
                        lgpposition_id = glgeom->lgpposition_id_;
                        lgpposition = glgeom->lgpposition_;
                        asic = glch->asic_;
                        pcbPosition = glgeom->pcbPosition_;
                        position = glch->position_;
                        tree->Fill();
                    } else {
                        ++empty_channels;
                        std::cout << "==== Warning::Missing channel: " << feb_channel << std::endl;
                    }
                }
            }
            FEBdir->cd();
            TF1 *fit = new TF1("fit", "[0]*x+[1]", 64, 95);
            allFEBs->Fit("fit", "qr+");
            allFEBs->Write();
            FEBdir->Close();
            delete fit;
            delete allFEBs;
        }
            Cratedir->Close();
        /// Find channels with std more than 3 sigma
        string connection;
        if(Calibrate) {
            TF1 *fit = new TF1("fit", "gaus");

            /// Adjust the fitting boundaries
            hGain->Fit("fit", "", "");
            double mean_gain = fit->GetParameter(1);
            double std_gain = fit->GetParameter(2);
            wfile->cd();
            auto *l_1 = new TLine(hGain->GetMean() - 3 * fit->GetParameter(2), 0,
                                  hGain->GetMean() - 3 * fit->GetParameter(2), 10000);
            auto *l_2 = new TLine(hGain->GetMean() + 3 * fit->GetParameter(2), 0,
                                  hGain->GetMean() + 3 * fit->GetParameter(2), 10000);
            l_1->SetLineStyle(2);
            l_2->SetLineStyle(2);
            c1->cd();
            hGain->Draw();
            l_1->Draw();
            l_2->Draw();
            c1->Write();


            int channels_hg = 0;
            int no_fit = 0;

            wfile->cd();
            auto gain_values = cl.GetGains();
            TH1F *hGain_dist = new TH1F("Gain_deviation", "Gain_deviation", 1000, -10, 10);
            for (auto ih: NFEB) {
                for (auto iCh = 0; iCh < SFGD_FEB_NCHANNELS; ++iCh) {
                    connection = "FEB_" + to_string(ih) + "_Channel_" + to_string(iCh);
                    if (gain_values.find(connection) != gain_values.end()) {
                        auto gain_value = gain_values.at(connection);
//                fout << connection<< " "<<gain_value.first<<" "<<gain_value.second<<"\n";
                        if (gain_value.first == 0) {
                            cout << "Problem in " << connection << " number of fitted peaks less than 4" << endl;
                            ++no_fit;
                        } else {
                            hGain_dist->Fill((gain_value.first - mean_gain) / mean_gain);
                            if ((gain_value.first < mean_gain - 3 * std_gain) ||
                                (gain_value.first > mean_gain + 3 * std_gain)) {
                                channels_hg += 1;
                                cout << "Problem in " << connection << ": " << gain_value.first << " out of 3 sigma"
                                     << endl;
                            }
                        }
                    }
                }
            }
            hGain_dist->Write();
            GainMap->GetYaxis()->SetTitle("FEB");
            GainMap->GetXaxis()->SetTitle("Channel");
            GainMap->Write();
            wfile->cd();
            tree->Write();
            std::cout << "Mean_gain: " << mean_gain << ", std_gain: " << std_gain << std::endl;
            std::cout << "Out of 3 sigma: " << channels_hg << ", form " << hGain->GetEntries() << std::endl;
            std::cout << "Number of gain = 0: " << no_fit << std::endl;
            std::cout << "Number of empty channels: " << empty_channels << std::endl;
            delete hGain_dist;
            delete l_1;
            delete l_2;
        }
        delete hGain;
        delete GainMap;
        delete tree;
        PrintCrates pc;
        pc.Write2DMapsCanvas(*wfile, *event_Side, *event_Top, *event_Upstream, "Entries");
        wfile->Close();
        delete wfile;
//        gROOT->GetListOfFiles()->Remove(wfile); /// It doesn't work on my computer, if it works on yours, please, use this instead of wfile->Close();
    }
    delete c1;
    return 0;
}
