void draw_line(int x0, int y0, int x1, int y1, int style = 1)
{
  TLine* line = new TLine(x0, y0, x1, y1);
  line->SetLineStyle(style);
  line->Draw();
}

void gridXY() {
  for (int i = 1; i < 24; i++)
    draw_line(i*8,0,i*8,56);
  for (int i = 1; i < 7; i++)
    draw_line(0,i*8,192,i*8);
}

void gridYZ() {
  for (int i = 1; i < 23; i++)
    draw_line(i*8-1,0,i*8-1,56);
  for (int i = 1; i < 7; i++)
    draw_line(0,i*8,182,i*8);
}

void gridXZ() {
  for (int i = 1; i < 23; i++)
    draw_line(i*8,0,i*8,182);
  for (int i = 1; i < 24; i++)
    draw_line(0,i*8-1,192,i*8-1);
}

void zigzag1(int z_part_0, int z_part_1, int offset = 0, int style = 1) {

  for (int i : {  4, 5, 6, 7, 8, 9,10,11,12,
         	 13,14,15,16,17,18,19,20}) {
    draw_line(i*8, z_part_0*8 + offset,
	      i*8, z_part_1*8 + offset, style);
    }
  
  for (int i : {  0,1,2,3,5,7,9,11,
		  12,14,16,18,20,21,22,23 }){
    draw_line(i*8, z_part_0*8 + offset,
	      i*8+8, z_part_0*8 + offset, style);
    }
  
  for (int i : {4,6,8,10, 13,15,17,19}){
    draw_line(i*8, z_part_1*8 + offset,
	      i*8+8, z_part_1*8 + offset, style);
  }
}

void zigzag2(int z_part_0, int z_part_1, int offset = 0) {

  for (int i : {  5, 6, 7, 8, 9,10,11,12,13,
         	 14,15,16,17,18,19}) {
    draw_line(i*8, z_part_0*8 + offset,
	      i*8, z_part_1*8 + offset);
    }
  
  for (int i : {  0,1,2,3,4,6,8,10,
		  13,15,17,19,20,21,22,23 }){
    draw_line(i*8, z_part_0*8 + offset,
	      i*8+8, z_part_0*8 + offset);
    }
  
  for (int i : {5,7,9,11,12,14,16,18}){
    draw_line(i*8, z_part_1*8 + offset,
	      i*8+8, z_part_1*8 + offset);
  }
}

void zigzag3(int z_part_0, int z_part_1, int offset = -1) {

  for (int i : {1,2,3,4,5,6,7} ) {
    draw_line(i*8 + offset, z_part_0*8,
	      i*8 + offset, z_part_1*8);
  }
  
  for (int i : {0,2,4,6} ){
    int x0 = i*8 + offset;
    int x1 = i*8+8 + offset;
    if (x0 < 0) x0 = 0;
    if (x1 > 182) x1 = 182;
    draw_line(x0, z_part_0*8,
	      x1, z_part_0*8);
  }
  
  for (int i : {1,3,5,7,8,9,10,11}){
    int x0 = i*8 + offset;
    int x1 = i*8+8 + offset;
    if (x0 < 0) x0 = 0;
    if (x1 > 182) x1 = 182;
    draw_line(x0, z_part_1*8,
	      x1, z_part_1*8);
  }
}

void zigzag4(int z_part_0, int z_part_1, int offset = -1) {

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
    if (x1 > 182) x1 = 182;
    draw_line(x0, z_part_0*8,
	      x1, z_part_0*8);
  }
  
  for (int j : {1,3,5,7,8,9,10}){
    int i = 22 - j;
    int x0 = i*8 + offset;
    int x1 = i*8+8 + offset;
    if (x0 < 0) x0 = 0;
    if (x1 > 182) x1 = 182;
    draw_line(x0, z_part_1*8,
	      x1, z_part_1*8);
  }
}

void zigzag3_swapped(int z_part_0, int z_part_1, int offset = -1) {

  for (int i : {1,2,3,4,5,6,7} ) {
    draw_line(z_part_0*8, i*8 + offset, 
	      z_part_1*8, i*8 + offset);
  }
  
  for (int i : {0,2,4,6} ){
    int x0 = i*8 + offset;
    int x1 = i*8+8 + offset;
    if (x0 < 0) x0 = 0;
    if (x1 > 182) x1 = 182;
    draw_line(z_part_0*8, x0,
	      z_part_0*8, x1);
  }
  
  for (int i : {1,3,5,7,8,9,10,11}){
    int x0 = i*8 + offset;
    int x1 = i*8+8 + offset;
    if (x0 < 0) x0 = 0;
    if (x1 > 182) x1 = 182;
    draw_line(z_part_1*8, x0,
	      z_part_1*8, x1);
  }
}

void zigzag4_swapped(int z_part_0, int z_part_1, int offset = -1) {

  for (int j : {0,1,2,3,4,5,6} ) {
    int i = 22 - j;
    draw_line(z_part_0*8, i*8 + offset, 
	      z_part_1*8, i*8 + offset);
  }
  
  for (int j : {0,2,4,6} ){
    int i = 22 - j;
    int x0 = i*8 + offset;
    int x1 = i*8+8 + offset;
    if (x0 < 0) x0 = 0;
    if (x1 > 182) x1 = 182;
    draw_line(z_part_0*8, x0, 
	      z_part_0*8, x1);
  }
  
  for (int j : {1,3,5,7,8,9,10}){
    int i = 22 - j;
    int x0 = i*8 + offset;
    int x1 = i*8+8 + offset;
    if (x0 < 0) x0 = 0;
    if (x1 > 182) x1 = 182;
    draw_line(z_part_1*8, x0, 
	      z_part_1*8, x1);
  }
}

void crate_boundary_XZ() {
  zigzag1(9,10,-1);
  zigzag2(5,4,-1);
  zigzag2(19,18,-1);
  zigzag1(6,7,-1,2);
  draw_line(0, 14*8-1, 192, 14*8-1);
  draw_line(96,0,96,182);
}

void slash_crate_0_4_8_XZ() {
  draw_line(0, 9*8-1, 96, 14*8-1); // crate 04
  draw_line(0, 0, 96, 6*8 -1);     // crate 00
  draw_line(96, 0, 192, 6*8 -1);   // crate 08
}

void slash_crate_1_XY() {
  draw_line(0, 2*8, 96, 7*8); // crate 01
}

void crate_boundary_XY() {
  zigzag1(2,3);
  draw_line(96, 0, 96, 56);
}

void crate_boundary_YZ() {
  zigzag3(3,2);
  draw_line(95, 0, 95, 56);
  zigzag4(6,5);
  draw_line(95, 24, 182, 24);
}

void crate_boundary_YZ_swapped() {
  zigzag3_swapped(3,2);
  draw_line(0, 95, 56, 95);
  zigzag4_swapped(6,5);
  draw_line(24, 95, 24, 182);
}

TH2F *swap_histogram_axis(TH2F *h1) {
   Int_t nbinx = h1->GetNbinsX();
   Int_t nbiny = h1->GetNbinsY();
   Double_t x1 = h1->GetXaxis()->GetBinLowEdge(1);
   Double_t x2 = h1->GetXaxis()->GetBinLowEdge(nbinx)+h1->GetXaxis()->GetBinWidth(nbinx);
   Double_t y1 = h1->GetYaxis()->GetBinLowEdge(1);
   Double_t y2 = h1->GetYaxis()->GetBinLowEdge(nbiny)+h1->GetYaxis()->GetBinWidth(nbiny);

   TH2F *h2 = new TH2F(h1->GetName(),h1->GetTitle(),nbiny,y1,y2,nbinx,x1,x2);
   h2->GetXaxis()->SetTitle(h1->GetYaxis()->GetTitle());
   h2->GetYaxis()->SetTitle(h1->GetXaxis()->GetTitle());
   h2->GetZaxis()->SetTitle(h1->GetZaxis()->GetTitle());

   Double_t c;
   for(int i=1; i<=nbinx; i++) {
      for(int j=1; j<=nbiny; j++) {
         c = h1->GetBinContent(i,j);
         h2->SetBinContent(j,i,c);
      }
   }
   return h2;
}

