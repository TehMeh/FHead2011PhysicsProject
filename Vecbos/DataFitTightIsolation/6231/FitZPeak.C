#include <string>
using namespace std;

#include "TFile.h"
#include "TTree.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TGraphErrors.h"

#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooLorentzian.h"
#include "RooAddPdf.h"
#include "RooPolynomial.h"
#include "RooExponential.h"
#include "RooCruijff.h"
#include "RooPlot.h"

#include "PlotHelper.h"

using namespace RooFit;

struct FitResult;
void FitZPeak(string RootFile = "Samples/DataAll.root", string OutputBase = "DataFitTightIsolation",
   string JetFlavor = "Calo", double Isolation = 0.05);
FitResult FitZPeak(string RootFile, PsFileHelper &PsFile, int JetCount, double Isolation, string JetFlavor);

struct FitResult
{
   double Mass[2];
   double AlphaL[2];
   double AlphaR[2];
   double SigmaL[2];
   double SigmaR[2];
};

void FitZPeak(string RootFile, string OutputBase, string JetFlavor, double Isolation)
{
   PsFileHelper PsFile(OutputBase + "_" + JetFlavor + ".ps");
   PsFile.AddTextPage("Fit Z peak floating everything");

   FitResult Result = FitZPeak(RootFile, PsFile, 1, Isolation, JetFlavor);

   cout << "Mass:" << Result.Mass[0] << " " << Result.Mass[1] << endl;
   cout << "AlphaL: " << Result.AlphaL[0] << " " << Result.AlphaL[1] << endl;
   cout << "AlphaR: " << Result.AlphaR[0] << " " << Result.AlphaR[1] << endl;
   cout << "SigmaL: " << Result.SigmaL[0] << " " << Result.SigmaL[1] << endl;
   cout << "SigmaR: " << Result.SigmaR[0] << " " << Result.SigmaR[1] << endl;

   PsFile.AddTimeStampPage();
   PsFile.Close();
}

FitResult FitZPeak(string RootFile, PsFileHelper &PsFile, int JetCount, double Isolation, string JetFlavor)
{
   RooRealVar ZMass("ZMass", "Invariant mass of dimuon pair", 60, 120, "GeV/c^2");
   RooRealVar NCaloJets(Form("N%sJet", JetFlavor.c_str()), "Count of jets", 0, 100000);
   RooRealVar Muon1CombinedIsolation("Muon1CombinedIsolation", "Muon1 Combined Isolation", 0, 100000);
   RooRealVar Muon1PT("Muon1PT", "Muon1 PT", 0, 100000);
   RooRealVar Muon2CombinedIsolation("Muon2CombinedIsolation", "Muon2 Combined Isolation", 0, 100000);
   RooRealVar Muon2PT("Muon2PT", "Muon2 PT", 0, 100000);
   RooArgSet TreeVarSet(ZMass, NCaloJets, Muon1CombinedIsolation, Muon1PT, Muon2CombinedIsolation, Muon2PT);

   TFile F(RootFile.c_str());
   TTree *ZTree = (TTree *)F.Get("outTree");
   RooDataSet dataset("ZCandidates", "ZCandidates", ZTree, TreeVarSet);

   cout << ".." << endl;

   TCut NumberOfJetCut = Form("N%sJet >= %d", JetFlavor.c_str(), JetCount);
   // TCut IsolationCut = Form("Muon1CombinedIsolation < Muon1PT * %f", Isolation);
   TCut IsolationCut = Form("Muon1CombinedIsolation < Muon1PT * %f && Muon2CombinedIsolation < Muon2PT * %f",
      Isolation, Isolation);
   RooDataSet *ReducedDataSet = (RooDataSet *)dataset.reduce(NumberOfJetCut && IsolationCut);

   RooRealVar PeakPosition("PeakPosition", "Z peak position", 91.000, 80, 100, "GeV/c^2");
   RooRealVar AlphaR("AlphaR", "AlphaR", 0.435, 0.01, 1000000);
   RooRealVar AlphaL("AlphaL", "AlphaL", 0.508, 0.01, 1000000);
   RooRealVar SigmaR("SigmaR", "SigmaR", 2.189, 0.01, 1000000);
   RooRealVar SigmaL("SigmaL", "SigmaL", 2.349, 0.01, 1000000);
   RooCruijff MainZPeak("MainZPeak", "MainZPeak", ZMass, PeakPosition, SigmaL, AlphaR, SigmaL, AlphaL);

   cout << "!" << endl;

   MainZPeak.fitTo(*ReducedDataSet);
   
   cout << "!!" << endl;

   RooPlot *Parameters = ZMass.frame(Bins(50), Name("MLL"), Title("Mass of lepton pair"));
   MainZPeak.paramOn(Parameters, Format("NELU", AutoPrecision(2)), Layout(0.1, 0.4, 0.9));
   PsFile.AddPlot(Parameters, "", false);

   RooPlot *FinalResult = ZMass.frame(Bins(50), Name("MLL"), Title("Mass of lepton pair"));
   ReducedDataSet->plotOn(FinalResult, MarkerSize(0.9));
   MainZPeak.plotOn(FinalResult, LineColor(kRed));
   PsFile.AddPlot(FinalResult, "", false);

   // TCanvas Canvas("Canvas", "Canvas", 0, 0, 1024, 768);
   // FinalResult->Draw();
   // Canvas.SaveAs("DataTightIsolationFit.png");
   // Canvas.SaveAs("DataTightIsolationFit.C");
   // Canvas.SaveAs("DataTightIsolationFit.eps");

   FitResult Result;

   Result.Mass[0] = PeakPosition.getVal();
   Result.Mass[1] = PeakPosition.getError();
   Result.AlphaL[0] = AlphaL.getVal();
   Result.AlphaL[1] = AlphaL.getError();
   Result.AlphaR[0] = AlphaR.getVal();
   Result.AlphaR[1] = AlphaR.getError();
   Result.SigmaL[0] = SigmaL.getVal();
   Result.SigmaL[1] = SigmaL.getError();
   Result.SigmaR[0] = SigmaL.getVal();
   Result.SigmaR[1] = SigmaL.getError();

   return Result;
}

