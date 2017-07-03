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
   string VarName[VarNum+1]={"Count", "PT", "E", "Eta", "Phi", "CountInJet", "DeltaR", "DeltaEta", "DeltaPhi"};
  
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
   for (int i=0;i<=IDNum;i++){
     PdfFile.AddHistogramFromFile(File, ("H"+IDName[i]+VarName[5]).c_str(), "", false);
   }
   // DR,DEta,DPhi distribution pages
   for (int k=VarNum-2;k<=VarNum;k++){
     for (int i=0;i<=IDNum;i++){
       for (int j=0;j<2;j++){
	 if(j==0)
	   PdfFile.AddHistogramFromFile(File, ("HJet"+IDName[i]+"PTvs"+VarName[k]).c_str(), "COLZ", true);
	 else
	   PdfFile.AddHistogramFromFile(File, ("HJet"+IDName[i]+"Countvs"+VarName[k]).c_str(), "", false);
       }
     }
   }
   PdfFile.AddHistogramFromFile(File, "HPTPercent", "", false);
   // Cleanup
   File.Close();

   // Add a time stamp and close the pdf file
   PdfFile.AddTimeStampPage();
   PdfFile.Close();

   return 0;
}



