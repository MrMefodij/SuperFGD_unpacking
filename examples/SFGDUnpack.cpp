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

#include <iostream>
#include <vector>
#include <thread>
#include <TThread.h>
#include "MDargumentHandler.h"
#include "OutputHandler.h"
#include "Pthread_Unpacking.h"

using namespace std;


int main( int argc, char **argv ) {
    string stringBuf;

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

    vector<string> vFileNames  = argh.GetDataFiles(stringBuf,".bin");
    std::sort(vFileNames.begin(), vFileNames.end());
    if ( vFileNames.size() == 0 ) {
        cerr << "Can not open directory " << stringBuf << endl;
        return 1;
    }

    if ( argh.GetValue("output_directory", stringBuf) != MDARGUMENT_STATUS_OK ) return -1;
    string outputdir=stringBuf;
    OutputHandler outh;
    //outh.SetVerbose(1);
    std::thread th[UNPACKING_PTHREADS];
    pthread_mutex_t  std_out_lock;
    pthread_mutex_init(&std_out_lock, NULL);
    TThread::Initialize();
    std::pair<bool,int> pthread_file[UNPACKING_PTHREADS];

    for (int i = 0; i < vFileNames.size();) {
//        std::cout << vFileNames.size()/UNPACKING_PTHREADS << std::endl;
        for (int j = 0; j < UNPACKING_PTHREADS; ++j) {
            if (i < vFileNames.size()){
                string outputFilePrefix = outh.GetOutputFilePrefix(vFileNames.at(i),outputdir);
                th[j] = std::thread(UnpackFile, vFileNames.at(i), outputFilePrefix, &std_out_lock);
                pthread_file[j] = {true,i};
                i++;
            } else {
                pthread_file[j] = {false,0};
                i++;
            }
        }
        for (int j = 0; j < UNPACKING_PTHREADS; ++j) {
            if (pthread_file[j].first) {
                th[j].join();
                std::cout << vFileNames.at(pthread_file[j].second) << " was unpacked." <<std::endl;
                pthread_file[j] = {false,0};
            }
        }
    }
    return 0;
}
