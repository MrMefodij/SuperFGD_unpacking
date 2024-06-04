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

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <iostream>
#include <string.h>
#include <exception>
#include <fstream>

#include "TFile.h"
#include "TH1D.h"

#include "MDdataWordSFGD.h"
#include "MDargumentHandler.h"

using namespace std;

#define   FIFO_PATH "/tmp/ufe_fifo"
#define   BUF_SIZE  2048

int main( int argc, char **argv ) {

  int ufe_fifo;
  while ( (ufe_fifo = open(FIFO_PATH, O_RDONLY)) == -1) {
    usleep(1000);
  }

  uint32_t buffer_32[BUF_SIZE/4];
  int actual = read(ufe_fifo, &buffer_32, sizeof(uint32_t));
  if (actual != sizeof(uint32_t) || buffer_32[0] != 0xabcdef) {
    cerr << "Cannot make a handshake.\n";
    return 1;
  }

  TFile rfile("histos.root", "recreate");
  TH1I  h_lgah("h_lg_amp_hit_ch", "hit channels", 100, 0, 100);
  TH1I  h_hgah("h_lh_amp_hit_ch", "hit channels", 100, 0, 100);
  TH1I  h_lga("h_lg_ampl", "hit ampl.", 300, 200, 500);
  TH1I  h_hga("h_hg_ampl", "hit ampl.", 500, 000, 500);

  while (1) {
    actual = read(ufe_fifo, &buffer_32, BUF_SIZE);

    if (actual != 0) {
      if (actual == 4 && buffer_32[0] == 0xfedcba)
        break;

      uint32_t* buf_ptr = buffer_32;
      for (int i=0; i<BUF_SIZE/4; ++i) {
        MDdataWordSFGD dw(buf_ptr++);
        if ( dw.GetDataType() == MDdataWordSFGD::GateHeader ||
             dw.GetDataType() == MDdataWordSFGD::GateTrailer ||
             dw.GetDataType() == MDdataWordSFGD::GTSHeader ||
             dw.GetDataType() == MDdataWordSFGD::GTSTrailer1 ) {
          cout << dw << endl;
        }

        if ( dw.GetDataType() == MDdataWordSFGD::ChargeMeas ) {
          if (dw.GetAmplitudeId() == 3) {
            h_hga.Fill(dw.GetAmplitude());
            h_hgah.Fill(dw.GetChannelId());
          } else if (dw.GetAmplitudeId() == 2) {
            h_lga.Fill(dw.GetAmplitude());
            h_lgah.Fill(dw.GetChannelId());
          }
        }
      }
    }
  }

  close(ufe_fifo);

  h_lgah.Write();
  h_hgah.Write();
  h_lga.Write();
  h_hga.Write();
  rfile.Close();

  return 0;
}


