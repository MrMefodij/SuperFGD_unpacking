/* This file is part of SuperFGD software package. This software
 * package is designed for internal use for the SuperFGD detector
 * collaboration and is tailored for this use primarily.
 *
 * Unpacking is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Unpacking is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SuperFGD Unpacking.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//
// Created by amefodev on 13.06.2023. mrmefodij@gmail.com
//

#include <TFile.h>
#include <TTree.h>
#include <TDirectory.h>
#include <TF1.h>
#include <TGraph.h>
#include <TAxis.h>
#include <string>
#include <fstream>
#include <thread>
#include <set>
#include <mutex>
#include <chrono>

#include "MDargumentHandler.h"
#include "ToaEventDummy.h"
#include "SFGD_defines.h"
#include "MDfragmentSFGD.h"
#include "MDdataFile.h"


/// if you need DAC10bt for each ASIC: correction_factor = 32
/// for each channel: correction_factor = 1
#define correction_factor 32
#define SFGD_Elems SFGD_FEBS_NUM * SFGD_FEB_NCHANNELS / correction_factor

std::mutex mtx;

namespace ThresholdStudy {
    void doTimeThresholdStudy(std::string fileName, int fileNum, std::vector<unsigned int> &DAC10b_t,
                              std::set<unsigned int> &available_FEBs,
                              std::vector<std::vector<double>> &results_points) {
        {
            std::lock_guard<std::mutex> guard(mtx);
            std::cout << "Reading " << fileName << "...." << std::endl;
        }
        size_t pos_1 = fileName.find("_DAC10b_");
        size_t pos_2 = fileName.find(".bin");

        DAC10b_t[fileNum] = std::stoi(fileName.substr(pos_1 + 8, pos_2 - pos_1 - 8));
        unsigned int events_count[SFGD_Elems]{0};
        unsigned int all_events{0};
        std::ifstream ifs((fileName).c_str());
        if (ifs.fail()) {
            std::cerr << "Can not open file " << fileName << std::endl;
        }
        char *eventBuffer;
        unsigned int ocbEventNumber{0};
        bool firstEventNumber = true;
        MDdateFile dfile(fileName);
        if (dfile.open(false)) { // There is a valid files to unpack
            dfile.init();
            do { // Loop over all spills
                eventBuffer = dfile.GetNextEvent(false);
                if (dfile.GetOcbEventNumber() != ocbEventNumber && !firstEventNumber) {
                    ++all_events;
                } else {
                    firstEventNumber = false;
                }
                ocbEventNumber = dfile.GetOcbEventNumber();

                try {
                    MDfragmentSFGD spill;
                    spill.SetDataPtr(eventBuffer);
                    int nTr = spill.GetNumOfTriggers();
                    auto BordID = spill.GetBoardId();
                    if (!(BordID == 0 && spill.GetGateType() == 0 && spill.GetGateNumber() == 0)) {
                        std::lock_guard<std::mutex> guard(mtx);
                        available_FEBs.insert(BordID);
                    }
                    for (int i = 0; i < nTr; ++i) {
                        MDpartEventSFGD *trEv = spill.GetTriggerEventPtr(i);
                        for (int ich = 0; ich < SFGD_FEB_NCHANNELS; ++ich) {
                            unsigned int nlHits = trEv->GetNLeadingEdgeHits(ich);
                            for (unsigned int ih = 0; ih < nlHits; ++ih) {
                                ++events_count[int((BordID * SFGD_FEB_NCHANNELS + ich) / correction_factor)];
                            }
                        }
                    }

                } catch (MDexception &lExc) {
                    std::cerr << lExc.GetDescription() << std::endl
                              << "Unpacking exception\n"
                              << "Spill skipped!\n\n";
                } catch (std::exception &lExc) {
                    std::cerr << lExc.what() << std::endl
                              << "Standard exception\n"
                              << "Spill skipped!\n\n";
                } catch (...) {
                    std::cerr << "Unknown exception occurred...\n"
                              << "Spill skipped!\n\n";
                }
            } while (eventBuffer);
        }
        delete eventBuffer;
        dfile.close();
        {
            std::lock_guard<std::mutex> guard(mtx);
            std::cout << "Finished " << fileName << "...." << std::endl;
        }
        for (auto ih: available_FEBs) {
            for (auto ch = 0; ch < SFGD_FEB_NCHANNELS; ch += correction_factor) {
                unsigned int AsicNumber = (int) ((ih * SFGD_FEB_NCHANNELS + ch) / correction_factor);
                if (events_count[AsicNumber] != 0) {
                    std::lock_guard<std::mutex> guard(mtx);
                    results_points[AsicNumber][fileNum] = (double) events_count[AsicNumber] / all_events;
                }
            }
        }
    }
}


int main( int argc, char **argv ) {
    std::string stringBuf;
    clock_t tS = clock();
    // The following shows how to use the MDargumentHandler class
    // to deal with the main arguments
    // Define the arguments
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

    std::vector<std::string> vFileNames  = argh.GetDataFiles(stringBuf,".bin");
    std::sort(vFileNames.begin(), vFileNames.end());

    if ( vFileNames.empty() ) {
        std::cerr << "Can not open directory " << stringBuf << std::endl;
        return 1;
    }

    unsigned int cratesCount = vFileNames.size();
    std::string rootFileOutput = stringBuf + "DAC10bt.root";
    std::cout <<"Output: "<< rootFileOutput << std::endl;

    std::vector<unsigned int> DAC10b_t(cratesCount);

    std::vector<std::vector<double>> results_points(SFGD_Elems, std::vector<double>(cratesCount));
    std::set<unsigned int> available_FEBs;
    std::thread th[vFileNames.size()];
    for(int i = 0; i < vFileNames.size(); ++i){
        th[i] = std::thread(ThresholdStudy::doTimeThresholdStudy, vFileNames[i], i, std::ref(DAC10b_t), std::ref(available_FEBs),std::ref(results_points));
    }
    for(int i = 0; i < vFileNames.size(); ++i){
        th[i].join();
    }

    TFile wfile(rootFileOutput.c_str(), "recreate");
    wfile.cd();


    std::cout << "Threshold drawing ..."<<std::endl;
    for (auto ih : available_FEBs) {
        TDirectory *FEBdir = wfile.mkdir(("FEB " + std::to_string(ih)).c_str());
        FEBdir->cd();
        for (auto ch = 0; ch < SFGD_FEB_NCHANNELS; ch += correction_factor) {
            unsigned int AsicNumber = (int) ((ih * SFGD_FEB_NCHANNELS + ch) / correction_factor);
            std::vector<unsigned int> thresholds;
            std::string s = "FEB " + std::to_string(ih) + " ASIC " + std::to_string(ch / correction_factor);
            TGraph *example = new TGraph();
            example->GetYaxis()->SetTitle("#channel_hits/ #gates");
            example->GetXaxis()->SetTitle("DAC10b_t");
            example->SetTitle(s.c_str());
            for (auto j = 0; j < vFileNames.size(); ++j) {
                example->AddPoint(DAC10b_t[j], results_points[AsicNumber][j]);
            }
            if (example->GetN() > 0) {
                example->Write();
                delete example;
            }
        }
        FEBdir->Close();
        delete FEBdir;
    }

    wfile.Close();
    printf("Time taken: %.2fs\n", (double)(clock() - tS) / CLOCKS_PER_SEC);
    return 0;
}
