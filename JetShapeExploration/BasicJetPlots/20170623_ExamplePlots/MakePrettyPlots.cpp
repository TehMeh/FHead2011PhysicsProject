#include <iostream>
#include <string>
#include <cstring>
using namespace std;

#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"

#include "PlotHelper3.h"
#include "SetStyle.h"

void AddCanvasPlots(TCanvas &Canvas, TLegend &Legend, TFile &File, string Name1, Color_t &Color, string Title, string legend, int draw)
{ 
   TH1D *H1 = (TH1D *)File.Get(Name1.c_str());

   H1->SetTitle(Title.c_str());
   H1->SetLineColor(Color);
   H1->SetStats(kFALSE);
   H1->Scale(1/(H1->GetBinContent(H1->GetMaximumBin())));
   H1->SetLineWidth(2);

   if (draw==0) H1->Draw("");
   else H1->Draw("same");

   Legend.AddEntry(H1, legend.c_str(), "lp");

}

void AddMultiplePlots(PdfFileHelper &PdfFile, TCanvas &Canvas, TLegend &Legend){
	
	Legend.Draw();
	PdfFile.AddCanvas(Canvas);
	Canvas.Clear();
	Legend.Clear();
}


int main()
{
   const int IDNum=4;
   const int VarNum=8;
   string IDName[IDNum+1]= {"Charged","Electron", "Muon", "Photon", "Hadron"};
   string PartonName[5]={"All", "Light", "Charm", "Bottom", "Gluon"};
   string VarName[VarNum+1]={"Count", "PT", "E", "Eta", "Phi", "CountInJet", "DeltaR", "DeltaEta", "DeltaPhi"};
   string xVal[4]={"0", "1", "1.5", "2"},yVal[4]={"0", "1", "2", "-1.5"};
   Color_t PrettyColor[4]={ kBlue, kBlack, kGreen, kRed };
   TCanvas Canvas;
   TLegend Legend(0.8, 0.65, 0.93, 0.45, "");
   Legend.SetTextFont(42);
   Legend.SetTextSize(0.035);
   TLatex text;
   text.SetNDC();
   text.SetTextSize(0.04);
   // Set plot style
   SetThesisStyle();

   // Output pdf file
   PdfFileHelper PdfFile("PrettyPlotsData.pdf");
   PdfFile.AddTextPage("Some first plots!");

   // Input file
   TFile File("ScaledResult/PP6Dijet.root");
 
   // Add a bunch of histograms from the file
   PdfFile.AddTextPage("PTHat Plots");
   PdfFile.AddHistogramFromFile(File, "HPTHat", "", true);
   PdfFile.AddHistogramFromFile(File, "HPTHatSelected", "", true);
   
   PdfFile.AddTextPage("Jet Plots");
   PdfFile.AddHistogramFromFile(File, "HJetPT", "", true);
   PdfFile.AddHistogramFromFile(File, "HJetEta", "", false);
   PdfFile.AddHistogramFromFile(File, "HJetPhi", "", false);
   PdfFile.AddHistogramFromFile(File, "HJetEtaSmallPT", "", false);
   PdfFile.AddHistogramFromFile(File, "HJetPhiSmallPT", "", false);
   PdfFile.AddHistogramFromFile(File, "HJetRawPTvsJetPT", "COLZ",false);
   PdfFile.AddHistogramFromFile(File, "HPFPT", "", true);
   PdfFile.AddHistogramFromFile(File, "HPFE", "", true);
   PdfFile.AddHistogramFromFile(File, "HPFEta", "", false);
   PdfFile.AddHistogramFromFile(File, "HPFPhi", "", false);
   
   for (int i=0;i<=VarNum-4;i++){
     for (int k=0;k<=IDNum;k++){
       if (i==1 || i==2){
	 PdfFile.AddHistogramFromFile(File, ("H"+IDName[k]+VarName[i]).c_str(), "", true);
       }
       else
	 PdfFile.AddHistogramFromFile(File, ("H"+IDName[k]+VarName[i]).c_str(), "", false); 
     }
   }
   PdfFile.AddTextPage("Plots from central jets");
   for (int i=0;i<=IDNum;i++){
     PdfFile.AddHistogramFromFile(File, ("HJet"+IDName[i]+VarName[5]).c_str(), "", false);
   }
   PdfFile.AddTextPage("DR,  DEta, DPhi distributions");
   // DR,DEta,DPhi distribution pages
   for (int k=VarNum-2;k<=VarNum;k++){
     for (int i=0;i<=IDNum;i++){
       for (int j=0;j<2;j++){
	   if (j==0)
	   PdfFile.AddHistogramFromFile(File, ("HJet"+IDName[i]+VarName[j]+"vs"+VarName[k]).c_str(), "", false);
	   else 
	   PdfFile.AddHistogramFromFile(File, ("HJet"+IDName[i]+VarName[j]+"vs"+VarName[k]).c_str(), "COLZ", false);
       }
     }
   }
 
   PdfFile.AddTextPage("Quark and gluon separated plots");
   for(int i=1;i<5;i++){
	for(int k=0;k<=IDNum;k++){
		PdfFile.AddHistogramFromFile(File, ("HJet"+IDName[k]+"PT"+PartonName[i]).c_str(), "COLZ", false);	
	}
   }
	
   // percentage in DR<0.1 plots
   for (int i=0;i<5;i++){
	PdfFile.AddHistogramFromFile(File, ("HJetPTPercent"+PartonName[i]).c_str(), "", false);
   }
   // Hadron distribution
   for (int i=1;i<5;i++){
	PdfFile.AddHistogramFromFile(File, ("HJetHadronCountDR"+PartonName[i]).c_str(), "", false);
	if (i==1 || i==4)
	AddCanvasPlots(Canvas, Legend, File, "HJetHadronCountDR"+PartonName[i], PrettyColor[i-1], "Hadron DR<0.4 distribution", PartonName[i], i-1);
   }
	AddMultiplePlots(PdfFile, Canvas, Legend);
   
  
   // Moments
   for (int i=0;i<4;i++){
	for (int k=0;k<4;k++){
		for(int m=1;m<5;m++){
			PdfFile.AddHistogramFromFile(File, ("HMoment"+PartonName[m]+"_x="+xVal[i]+"_y="+yVal[k]).c_str(), "", false);
			if (m==1 || m==4)
			AddCanvasPlots(Canvas, Legend, File,"HMoment"+PartonName[m]+"_x="+xVal[i]+"_y="+yVal[k], PrettyColor[m-1], "Moment: x="+xVal[i]+" y="+yVal[k], PartonName[m], m-1);
		}
		text.DrawLatex(0.67, 0.8, ("#sum_{i#epsilonjet}#(){#frac{p_{T}^{i}}{p_{T}^{jet}}}^{"+xVal[i]+"}#(){#frac{dR^{i}}{dR^{jet}}}^{"+yVal[k]+"}").c_str());
		AddMultiplePlots(PdfFile, Canvas, Legend);
	}
   }
 
   // Cleanup
   File.Close();

   // Add a time stamp and close the pdf file
   PdfFile.AddTimeStampPage();
   PdfFile.Close();

   return 0;
}



