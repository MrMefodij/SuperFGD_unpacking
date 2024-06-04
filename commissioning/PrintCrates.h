//
// Created by Maria on 28.03.2024 kolupanova@inr.ru
//

#ifndef UNPACKING_PRINTCRATES_H
#define UNPACKING_PRINTCRATES_H

#include <TLine.h>
#include <TH2D.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TStyle.h>
#include "SFGD_defines.h"
#include <TText.h>
#include <TBox.h>
#include <TLatex.h>
#include <TPaveText.h>
#include <TFile.h>

class PrintCrates{
public:
    PrintCrates();
    ~PrintCrates();
    /// Regroup the three views on a single canvas and write it in the output wfile
    void Write2DMapsCanvas(TFile& wfile,TH2F& hSide, TH2F& hTop, TH2F& hUpstream, std::string title, bool DrawPcbBorders = true, bool DrawCrateBorders = true);
private:
    TLine *line;
    void draw_line(int x0, int y0, int x1, int y1, int style = 1);
    void zigzag1(int z_part_0, int z_part_1, int offset = 0, int style = 1);
    void zigzag2(int z_part_0, int z_part_1, int offset = 0);
    void zigzag3(int z_part_0, int z_part_1, int offset = -1);
    void zigzag4(int z_part_0, int z_part_1, int offset = -1);
    void crate_boundary_Top_ZX();
    void crate_boundary_Up_YX();
    void crate_boundary_Side_ZY();
    TH2F swap_histogram_axis(const TH2F& h1);
};

#endif //UNPACKING_PRINTCRATES_H
