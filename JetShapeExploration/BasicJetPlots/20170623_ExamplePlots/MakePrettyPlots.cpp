#include <iostream>
#include <string>
#include <cstring>
using namespace std;

#include "TFile.h"
#include "TH1D.h"

#include "PlotHelper3.h"
#include "SetStyle.h"

int main()
{
   const int IDNum=4;
   const int VarNum=8;
   string IDName[IDNum+1]= {"Charged","Electron", "Muon", "Photon", "Hadron"};
   string PartonName[4]={"All", "Light", "Heavy", "Gluon"};
   string VarName[VarNum+1]={"Count", "PT", "E", "Eta", "Phi", "CountInJet", "DeltaR", "DeltaEta", "DeltaPhi"};
   string xVal[4]={"0", "1", "1.5", "2"},yVal[3]={"0", "1", "2"};
  
   // Set plot style
   SetThesisStyle();

   // Output pdf file
   PdfFileHelper PdfFile("PrettyPlots.pdf");
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
   for(int i=1;i<4;i++){
	for(int k=0;k<=IDNum;k++){
		PdfFile.AddHistogramFromFile(File, ("HJet"+IDName[k]+"PT"+PartonName[i]).c_str(), "COLZ", false);	
	}
   }
   // percentage in DR<0.1 plots
   for (int i=0;i<4;i++){
	PdfFile.AddHistogramFromFile(File, ("HJetPTPercent"+PartonName[i]).c_str(), "", false);
   }
   // Hadron distribution
   for (int i=1;i<4;i++){
	PdfFile.AddHistogramFromFile(File, ("HJetHadronCountDR"+PartonName[i]).c_str(), "", false);
   }	
   // Moments 
	
   for (int i=0;i<4;i++){
	for (int k=0;k<3;k++){
		for(int m=1;m<4;m++){
			PdfFile.AddHistogramFromFile(File, ("HMoment"+PartonName[m]+"_x="+xVal[i]+"_y="+yVal[k]).c_str(), "", false);
		}
	}
   }
   // Cleanup
   File.Close();

   // Add a time stamp and close the pdf file
   PdfFile.AddTimeStampPage();
   PdfFile.Close();

   return 0;
}



