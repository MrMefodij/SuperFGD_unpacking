//
// Created by Maria on 04.05.2024.
//
#include "PrintCrates.h"


PrintCrates::PrintCrates() {
    line = new TLine();
}
PrintCrates::~PrintCrates() {
    delete line;
}

void PrintCrates::draw_line(int x0, int y0, int x1, int y1, int style)
{
    line->PaintLine(x0,y0,x1,y1);
    line->SetLineStyle(style);
    line->Draw();
}

void PrintCrates::zigzag1(int z_part_0, int z_part_1, int offset, int style) {

    for (int i : {  4, 5, 6, 7, 8, 9,10,11,12,
                    13,14,15,16,17,18,19,20}) {
        draw_line(z_part_0*8 + offset, i*8,
                  z_part_1*8 + offset,i*8, style);
    }

    for (int i : {  0,1,2,3,5,7,9,11,
                    12,14,16,18,20,21,22,23 }){
        draw_line(z_part_0*8 + offset,i*8, z_part_0*8 + offset,
                  i*8+8, style);
    }

    for (int i : {4,6,8,10, 13,15,17,19}){
        draw_line(z_part_1*8 + offset, i*8, z_part_1*8 + offset,
                  i*8+8, style);
    }
}

void PrintCrates::zigzag2(int z_part_0, int z_part_1, int offset) {

    for (int i : {  5, 6, 7, 8, 9,10,11,12,13,
                    14,15,16,17,18,19}) {
        draw_line(z_part_0*8 + offset,i*8,
                  z_part_1*8 + offset, i*8);
    }

    for (int i : {  0,1,2,3,4,6,8,10,
                    13,15,17,19,20,21,22,23 }){
        draw_line(z_part_0*8 + offset,i*8,
                   z_part_0*8 + offset, i*8+8);
    }

    for (int i : {5,7,9,11,12,14,16,18}){
        draw_line(z_part_1*8 + offset, i*8,
                  z_part_1*8 + offset, i*8+8);
    }
}

void PrintCrates::zigzag3(int z_part_0, int z_part_1, int offset) {

    for (int i : {1,2,3,4,5,6,7} ) {
        draw_line(i*8 + offset, z_part_0*8,
                  i*8 + offset, z_part_1*8);
    }

    for (int i : {0,2,4,6} ){
        int x0 = i*8 + offset;
        int x1 = i*8+8 + offset;
        if (x0 < 0) x0 = 0;
        if (x1 > Z_SIZE) x1 = Z_SIZE;
        draw_line(x0, z_part_0*8,
                  x1, z_part_0*8);
    }

    for (int i : {1,3,5,7,8,9,10,11}){
        int x0 = i*8 + offset;
        int x1 = i*8+8 + offset;
        if (x0 < 0) x0 = 0;
        if (x1 > Z_SIZE) x1 = Z_SIZE;
        draw_line(x0, z_part_1*8,
                  x1, z_part_1*8);
    }
}

void PrintCrates::zigzag4(int z_part_0, int z_part_1, int offset) {

    for (int j : {0,1,2,3,4,5,6} ) {
        int i = 22 - j;
        draw_line(i*8 + offset, z_part_0*8,
                  i*8 + offset, z_part_1*8);
    }

    for (int j : {0,2,4,6} ){
        int i = 22 - j;
        int x0 = i*8 + offset;
        int x1 = i*8+8 + offset;
        if (x0 < 0) x0 = 0;
        if (x1 > Z_SIZE) x1 = Z_SIZE;
        draw_line(x0, z_part_0*8,
                  x1, z_part_0*8);
    }

    for (int j : {1,3,5,7,8,9,10}){
        int i = 22 - j;
        int x0 = i*8 + offset;
        int x1 = i*8+8 + offset;
        if (x0 < 0) x0 = 0;
        if (x1 > Z_SIZE) x1 = Z_SIZE;
        draw_line(x0, z_part_1*8,
                  x1, z_part_1*8);
    }
}

void PrintCrates::crate_boundary_Top_ZX() {
    zigzag1(9,10,-1);
    zigzag2(5,4,-1);
    zigzag2(19,18,-1);
    draw_line(14*8-1, 0, 14*8-1, X_SIZE);
    draw_line(0, 96,Z_SIZE,96);
}

void PrintCrates::crate_boundary_Up_YX() {
    zigzag1(2,3);
    draw_line(0, 96, Y_SIZE, 96);
}

void PrintCrates::crate_boundary_Side_ZY() {
    zigzag3(3,2);
    draw_line(95, 0, 95, Y_SIZE);
    zigzag4(6,5);
    draw_line(95, 24, Z_SIZE, 24);
}

TH2F PrintCrates::swap_histogram_axis(const TH2F& h1) {
    Int_t nbinx = h1.GetNbinsX();
    Int_t nbiny = h1.GetNbinsY();
    Double_t x1 = h1.GetXaxis()->GetBinLowEdge(1);
    Double_t x2 = h1.GetXaxis()->GetBinLowEdge(nbinx)+h1.GetXaxis()->GetBinWidth(nbinx);
    Double_t y1 = h1.GetYaxis()->GetBinLowEdge(1);
    Double_t y2 = h1.GetYaxis()->GetBinLowEdge(nbiny)+h1.GetYaxis()->GetBinWidth(nbiny);

    TH2F h2 = TH2F(h1.GetName(),h1.GetTitle(),nbiny,y1,y2,nbinx,x1,x2);
    h2.GetXaxis()->SetTitle(h1.GetYaxis()->GetTitle());
    h2.GetYaxis()->SetTitle(h1.GetXaxis()->GetTitle());
    h2.GetZaxis()->SetTitle(h1.GetZaxis()->GetTitle());

    Double_t c;
    for(int i=1; i<=nbinx; i++) {
        for(int j=1; j<=nbiny; j++) {
            c = h1.GetBinContent(i,j);
            h2.SetBinContent(j,i,c);
        }
    }
    return h2;
}

void PrintCrates::Write2DMapsCanvas(TFile& wfile,TH2F& hSide, TH2F& hTop, TH2F& hUpstream, std::string title, bool DrawPcbBorders, bool DrawCrateBorders){


    /// Normalize histograms
    auto maxSize = std::min(std::min(hUpstream.GetMaximum(),hTop.GetMaximum()),hSide.GetMaximum());
    auto minSize = std::min(std::min(hUpstream.GetMinimum(),hTop.GetMinimum()),hSide.GetMinimum());
    hSide.GetZaxis()->SetRangeUser(minSize,maxSize+1);
    hUpstream.GetZaxis()->SetRangeUser(minSize,maxSize+1);
    hTop.GetZaxis()->SetRangeUser(minSize,maxSize+1);
    // Define some plotting bounds
    double moveHorizontal = 0.01;
    double xPad = 0.25;
    double yPad = 0.255;
    double spaceright = 0.2;
    double spaceleft = 0.04;
    double spacetop = 0.06;
    double spacebottom = 0.05;

    double extraspace = 0.15;

    /// Create TCanvas
    TCanvas* c = new TCanvas("c","Canvas",1100,900);


    /// Make all the pads
    TPad* pUpstream = new TPad("pUpstream", "", spaceleft+extraspace, yPad, xPad+extraspace, 1.0-spacetop);
    TPad* pSide = new TPad("pSide", "", xPad+extraspace, spacebottom, 1.0-spaceright+extraspace, yPad);
    TPad* pTop = new TPad("pTop", "", xPad+extraspace, yPad, 1.0-spaceright+extraspace, 1.0-spacetop);
    TPad *pPallet = new TPad("pPallet", "", 0.0, spaceleft + 0.02, spaceleft + extraspace - 0.1,
                             1.0); // Hacky pad to draw a bigger z-axis


    /// Set the text alignment

    TPaveText *panal = new TPaveText(extraspace+0.01, spacebottom + 0.6 * (yPad - spacebottom) - 0.07, xPad + extraspace,yPad);
    TText *t1 = panal->AddText(("Side: "+ std::to_string((int)hSide.GetEntries())+" entries").c_str());
    TText *t2 = panal->AddText(("Top: "+ std::to_string((int)hTop.GetEntries())+" entries").c_str());
    TText *t3 = panal->AddText(("Upstream: "+ std::to_string((int)hUpstream.GetEntries())+" entries").c_str());
    panal->SetTextSize(0.025);
    panal->SetFillColor(0);



    /// More labels
    TText *LabelRight = new TText(0.15+extraspace, 0.035, "Right");
    TText *LabelUp = new TText(0.03+extraspace, 0.5, "Up");
    LabelUp->SetTextAngle(90);

    TText *LabelDown = new TText(0.81+extraspace, 0.6, "Down");
    LabelDown->SetTextAngle(270);

    double sizeLabels = 0.03;
    TText *LabelXtop = new TText(0.8+ moveHorizontal+extraspace, 0.8, "X (top)");
    LabelXtop->SetTextSize(sizeLabels);
    LabelXtop->SetTextAngle(270);

    TText *LabelXup = new TText(0.03+extraspace, 0.72, "X (up)");
    LabelXup->SetTextSize(sizeLabels);
    LabelXup->SetTextAngle(90);

    TText *LabelYside = new TText(0.8+ moveHorizontal+extraspace, 0.25, "Y (side)");
    LabelYside->SetTextSize(sizeLabels);
    LabelYside->SetTextAngle(270);

    TText *LabelYup = new TText(0.17+ moveHorizontal+extraspace, 0.96, "Y (up)");
    LabelYup->SetTextSize(sizeLabels);

    TText *LabelZside = new TText(0.65+ moveHorizontal+extraspace, 0.01, "Z (side)");
    LabelZside->SetTextSize(sizeLabels);

    TText *LabelZtop = new TText(0.65+ moveHorizontal+extraspace, 0.96, "Z (top)");
    LabelZtop->SetTextSize(sizeLabels);

    /// Set pad margins
    pUpstream->SetMargin(0.15, 0.005, 0.015, 0.05);
    pPallet->SetMargin(0.0, 0.5, 0.015, 0.1);
    pTop->SetMargin(0.015, 0.07, 0.015, 0.05);
    pSide->SetMargin(0.015, 0.07, 0.1, 0.);

    /// Lines for pcb borders
    TLine* lY;
    TLine* lX;
    TLine* lZ;

    /// Draw Upstream
    pUpstream->cd();
    hUpstream.SetStats(0);
    hUpstream.GetXaxis()->SetLabelFont(43); // The label text precision to 3
    hUpstream.GetXaxis()->SetLabelSize(15); // Set x-axis label size to 15 pixels
    hUpstream.GetYaxis()->SetLabelFont(43); // The label text precision to 3
    hUpstream.GetYaxis()->SetLabelSize(15); // Set x-axis label size to 20 pixels
    hUpstream.GetXaxis()->SetLabelOffset(0.007);
    hUpstream.SetTitleSize(0., "XY");
    hUpstream.Draw("colX+");
    /// Draw crate borders
    if(DrawCrateBorders)
        crate_boundary_Up_YX();

    /// Draw Upstream pcb borders
    if(DrawPcbBorders) {
        for (int i = 0; i < 23; ++i) {
            int lPos = 8 * (i + 1);
            // lUps[i] = new TLine(lPosX, 0, lPosX, 56);
            lY = new TLine( lPos, 0, lPos, 192);
            lY->SetLineStyle(3);
            lY->Draw();
            lX = new TLine(0, lPos, 56, lPos);
            lX->SetLineStyle(3);
            lX->Draw();
        }
    }

    /// Draw Side
    pSide->cd();
    hSide.SetStats(0);
    hSide.GetXaxis()->SetLabelFont(43); // The label text precision to 3
    hSide.GetXaxis()->SetLabelSize(15); // Set x-axis label size to 15 pixels
    hSide.GetYaxis()->SetLabelFont(43); // The label text precision to 3
    hSide.GetYaxis()->SetLabelSize(15); // Set x-axis label size to 20 pixels
    hSide.GetYaxis()->SetLabelOffset(0.01);
    hSide.SetTitleSize(0., "XY");
    hSide.Draw("colY+");

    /// Draw crate borders
    if(DrawCrateBorders)
        crate_boundary_Side_ZY();

    /// Draw Side pcb borders
    if(DrawPcbBorders) {
        for (int i = 0; i < 22; ++i) {
            int lPosY = 8 * (i + 1) - 1;
            int lPosZ = 8 * (i + 1);
            lY = new TLine( lPosY, 0, lPosY, 56);
            lY->SetLineStyle(3);
            lY->Draw();
            lZ = new TLine(0, lPosZ,  182, lPosZ);
            lZ->SetLineStyle(3);
            lZ->Draw();
        }
    }

    /// Draw Top
    pTop->cd();
    hTop.SetStats(0);
    hTop.GetXaxis()->SetLabelFont(43); // The label text precision to 3
    hTop.GetXaxis()->SetLabelSize(15); // Set x-axis label size to 15 pixels
    hTop.GetYaxis()->SetLabelFont(43); // The label text precision to 3
    hTop.GetYaxis()->SetLabelSize(15); // Set x-axis label size to 20 pixels
    hTop.GetXaxis()->SetLabelOffset(-0.01);
    hTop.SetTitleSize(0., "XY");
    hTop.Draw("colX+Y+");
    pTop->Update();
    /// Draw crate borders
    if(DrawCrateBorders)
        crate_boundary_Top_ZX();

    /// Draw Top pcb borders
    if(DrawPcbBorders) {
        for (int i = 0; i < 22; ++i) {
            int lPosZ = 8 * (i + 1) - 1;
            int lPosX = 8 * (i + 1);
//            if(i < 22) {
            lZ = new TLine(lPosZ,0,   lPosZ, 192);
            lZ->SetLineStyle(3);
            lZ->Draw();
//            }
            lX = new TLine(0, lPosX, 182, lPosX);
            lX->SetLineStyle(3);
            lX->Draw();
        }
    }

    /// Draw Pallet
    pPallet->cd();
    hSide.Draw("colZA");
    hSide.GetZaxis()->SetLabelFont(43); // The label text precision to 3
    hSide.GetZaxis()->SetLabelSize(15); // Set x-axis label size to 15 pixels
    hSide.GetZaxis()->SetLabelOffset(0.05);
    TBox * box = new TBox(-1,-1,182,195);
    box->SetFillColor(0);
    box->Draw("");

    /// Draw the pads
    c->cd();
    pPallet->Draw();
    pUpstream->Draw();
    pSide->Draw();
    pTop->Draw();

    ///  Draw the title and entries information
    panal->Draw();

    // Draw labels over the entire canvas
    LabelRight->Draw();
    LabelUp->Draw();
    LabelDown->Draw();
    LabelXtop->Draw();
    LabelXup->Draw();
    LabelYside->Draw();
    LabelYup->Draw();
    LabelZside->Draw();
    LabelZtop->Draw();


    wfile.cd();
    c->Write(title.c_str());
    delete t1;
    delete t2;
    delete t3;
    delete lY;
    delete lZ;
    delete lX;

    delete LabelXup;
    delete LabelXtop;
    delete LabelYside;
    delete LabelYup;
    delete LabelZtop;
    delete LabelZside;
    delete box;
    delete pSide;
    delete pUpstream;
    delete pTop;
    delete pPallet;
    c->Clear();
    c->Close();

}
