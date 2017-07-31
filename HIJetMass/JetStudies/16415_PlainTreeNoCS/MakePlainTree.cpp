#include <iostream>
using namespace std;

#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/contrib/ConstituentSubtractor.hh"
using namespace fastjet;

#include "TTree.h"
#include "TH1D.h"
#include "TFile.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TGraph.h"

#include "ProgressBar.h"
#include "PlotHelper3.h"
#include "SetStyle.h"

#include "Messenger.h"
#include "SDJetHelper.h"
#include "BasicUtilities.h"
#include "CATree.h"

int main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
   bool UseRhoM = false;

   SetThesisStyle();

   ClusterSequence::set_fastjet_banner_stream(NULL);

   string InputFileName = "HiForestAOD_999.root";
   string OutputFileName = "Output_HiForestAOD_999.root";
   string Tag = "AA6Dijet100";
   double PTHatMin = -1000000;
   double PTHatMax = 1000000;

   if(argc < 6)
   {
      cerr << "Usage: " << argv[0] << " Input Output Tag PTHatMin PTHatMax" << endl;
      return -1;
   }

   InputFileName = argv[1];
   OutputFileName = argv[2];
   Tag = argv[3];
   PTHatMin = atof(argv[4]);
   PTHatMax = atof(argv[5]);

   bool IsPP = IsPPFromTag(Tag);
   bool IsData = IsDataFromTag(Tag);
   bool IsPPHiReco = IsPPHiRecoFromTag(Tag);

   TFile *InputFile = TFile::Open(InputFileName.c_str());

   string JetTreeName = "akCs4PFJetAnalyzer/t";
   string SoftDropJetTreeName = "akCsSoftDrop4PFJetAnalyzer/t";
   string PFTreeName = "pfcandAnalyzer/pfTree";

   if(IsPP == true)
   {
      JetTreeName = "ak4PFJetAnalyzer/t";
      SoftDropJetTreeName = "akSoftDrop4PFJetAnalyzer/t";
      PFTreeName = "pfcandAnalyzer/pfTree";
   }

   if(IsPPHiReco == true)
   {
      JetTreeName = "akPu4PFJetAnalyzer/t";
      SoftDropJetTreeName = "akPu4CaloJetAnalyzer/t";
      PFTreeName = "pfcandAnalyzer/pfTree";
   }

   HiEventTreeMessenger MHiEvent(InputFile);
   JetTreeMessenger MJet(InputFile, JetTreeName);
   JetTreeMessenger MSDJet(InputFile, SoftDropJetTreeName);
   PFTreeMessenger MPF(InputFile, PFTreeName);
   RhoTreeMessenger MRho(InputFile);
   TriggerTreeMessenger MHLT(InputFile);
   SkimTreeMessenger MSkim(InputFile);

   if(MHiEvent.Tree == NULL)
      return -1;

   TFile OutputFile(OutputFileName.c_str(), "RECREATE");

   TH1D HN("HN", ";;", 1, 0, 1);

   TTree OutputTree("T", "T");

   int TreeRun, TreeEvent, TreeLumi;
   OutputTree.Branch("Run", &TreeRun, "Run/I");
   OutputTree.Branch("Event", &TreeEvent, "Event/I");
   OutputTree.Branch("Lumi", &TreeLumi, "Lumi/I");

   double TreeHF, TreeHFPlus, TreeHFMinus, TreeHFPlusEta4, TreeHFMinusEta4;
   OutputTree.Branch("HF", &TreeHF, "HF/D");
   OutputTree.Branch("HFPlus", &TreeHFPlus, "HFPlus/D");
   OutputTree.Branch("HFMinus", &TreeHFMinus, "HFMinus/D");
   OutputTree.Branch("HFPlusEta4", &TreeHFPlusEta4, "HFPlusEta4/D");
   OutputTree.Branch("HFMinusEta4", &TreeHFMinusEta4, "HFMinusEta4/D");

   int TreePartonFlavor, TreePartonFlavorForB;
   OutputTree.Branch("Flavor", &TreePartonFlavor, "Flavor/I");
   OutputTree.Branch("FlavorB", &TreePartonFlavorForB, "FlavorB/I");

   double TreeJetCSPT, TreeJetCSRawPT, TreeJetCSEta, TreeJetCSPhi;
   OutputTree.Branch("JetCSPT", &TreeJetCSPT, "JetCSPT/D");
   OutputTree.Branch("JetCSRawPT", &TreeJetCSRawPT, "JetCSRawPT/D");
   OutputTree.Branch("JetCSEta", &TreeJetCSEta, "JetCSEta/D");
   OutputTree.Branch("JetCSPhi", &TreeJetCSPhi, "JetCSPhi/D");
   
   double TreeJetPT, TreeJetRawPT, TreeJetEta, TreeJetPhi;
   OutputTree.Branch("JetPT", &TreeJetPT, "JetPT/D");
   OutputTree.Branch("JetRawPT", &TreeJetRawPT, "JetRawPT/D");
   OutputTree.Branch("JetEta", &TreeJetEta, "JetEta/D");
   OutputTree.Branch("JetPhi", &TreeJetPhi, "JetPhi/D");
   
   double TreeMatchEta, TreeMatchPhi;
   OutputTree.Branch("MatchEta", &TreeMatchEta, "MatchEta/D");
   OutputTree.Branch("MatchPhi", &TreeMatchPhi, "MatchPhi/D");
   
   double TreeSubJet1PT, TreeSubJet1Eta, TreeSubJet1Phi;
   double TreeSubJet2PT, TreeSubJet2Eta, TreeSubJet2Phi;
   OutputTree.Branch("SubJet1PT", &TreeSubJet1PT, "SubJet1PT/D");
   OutputTree.Branch("SubJet1Eta", &TreeSubJet1Eta, "SubJet1Eta/D");
   OutputTree.Branch("SubJet1Phi", &TreeSubJet1Phi, "SubJet1Phi/D");
   OutputTree.Branch("SubJet2PT", &TreeSubJet2PT, "SubJet2PT/D");
   OutputTree.Branch("SubJet2Eta", &TreeSubJet2Eta, "SubJet2Eta/D");
   OutputTree.Branch("SubJet2Phi", &TreeSubJet2Phi, "SubJet2Phi/D");

   double TreeSubJetDR, TreeSDMass;
   OutputTree.Branch("SubJetDR", &TreeSubJetDR, "SubJetDR/D");
   OutputTree.Branch("SDMass", &TreeSDMass, "SDMass/D");

   double TreeCentrality, TreePTHat, TreeRho;
   OutputTree.Branch("Centrality", &TreeCentrality, "Centrality/D");
   OutputTree.Branch("PTHat", &TreePTHat, "PTHat/D");
   OutputTree.Branch("Rho", &TreeRho, "Rho/D");

   double TreeMatchDR, TreeMatchPT;
   OutputTree.Branch("MatchDR", &TreeMatchDR, "MatchDR/D");
   OutputTree.Branch("MatchPT", &TreeMatchPT, "MatchPT/D");

   int TreeDepth0, TreeDepth7;
   OutputTree.Branch("Depth0", &TreeDepth0, "Depth0/I");
   OutputTree.Branch("Depth7", &TreeDepth7, "Depth7/I");

   double TreeSubJetDR0, TreeSDMass0;
   double TreeSubJetDR7, TreeSDMass7;
   OutputTree.Branch("SubJetDR0", &TreeSubJetDR0, "SubJetDR0/D");
   OutputTree.Branch("SDMass0", &TreeSDMass0, "SDMass0/D");
   OutputTree.Branch("SubJetDR7", &TreeSubJetDR7, "SubJetDR7/D");
   OutputTree.Branch("SDMass7", &TreeSDMass7, "SDMass7/D");
   
   double TreeSubJet1E0, TreeSubJet1PT0, TreeSubJet1Eta0, TreeSubJet1Phi0;
   double TreeSubJet2E0, TreeSubJet2PT0, TreeSubJet2Eta0, TreeSubJet2Phi0;
   OutputTree.Branch("SubJet1E0", &TreeSubJet1E0, "SubJet1E0/D");
   OutputTree.Branch("SubJet1PT0", &TreeSubJet1PT0, "SubJet1PT0/D");
   OutputTree.Branch("SubJet1Eta0", &TreeSubJet1Eta0, "SubJet1Eta0/D");
   OutputTree.Branch("SubJet1Phi0", &TreeSubJet1Phi0, "SubJet1Phi0/D");
   OutputTree.Branch("SubJet2E0", &TreeSubJet2E0, "SubJet2E0/D");
   OutputTree.Branch("SubJet2PT0", &TreeSubJet2PT0, "SubJet2PT0/D");
   OutputTree.Branch("SubJet2Eta0", &TreeSubJet2Eta0, "SubJet2Eta0/D");
   OutputTree.Branch("SubJet2Phi0", &TreeSubJet2Phi0, "SubJet2Phi0/D");
   
   double TreeSubJet1E7, TreeSubJet1PT7, TreeSubJet1Eta7, TreeSubJet1Phi7;
   double TreeSubJet2E7, TreeSubJet2PT7, TreeSubJet2Eta7, TreeSubJet2Phi7;
   OutputTree.Branch("SubJet1E7", &TreeSubJet1E7, "SubJet1E7/D");
   OutputTree.Branch("SubJet1PT7", &TreeSubJet1PT7, "SubJet1PT7/D");
   OutputTree.Branch("SubJet1Eta7", &TreeSubJet1Eta7, "SubJet1Eta7/D");
   OutputTree.Branch("SubJet1Phi7", &TreeSubJet1Phi7, "SubJet1Phi7/D");
   OutputTree.Branch("SubJet2E7", &TreeSubJet2E7, "SubJet2E7/D");
   OutputTree.Branch("SubJet2PT7", &TreeSubJet2PT7, "SubJet2PT7/D");
   OutputTree.Branch("SubJet2Eta7", &TreeSubJet2Eta7, "SubJet2Eta7/D");
   OutputTree.Branch("SubJet2Phi7", &TreeSubJet2Phi7, "SubJet2Phi7/D");

   bool TreePassPbPb40, TreePassPbPb60, TreePassPbPb80, TreePassPbPb100;
   OutputTree.Branch("PassPbPb40", &TreePassPbPb40, "PassPbPb40/O");
   OutputTree.Branch("PassPbPb60", &TreePassPbPb60, "PassPbPb60/O");
   OutputTree.Branch("PassPbPb80", &TreePassPbPb80, "PassPbPb80/O");
   OutputTree.Branch("PassPbPb100", &TreePassPbPb100, "PassPbPb100/O");

   double TreeCHF, TreeNHF, TreeCEF, TreeNEF, TreeMUF;
   OutputTree.Branch("CHF", &TreeCHF, "CHF/D");
   OutputTree.Branch("NHF", &TreeNHF, "NHF/D");
   OutputTree.Branch("CEF", &TreeCEF, "CEF/D");
   OutputTree.Branch("NEF", &TreeNEF, "NEF/D");
   OutputTree.Branch("MUF", &TreeMUF, "MUF/D");
   
   double TreeCHM, TreeNHM, TreeCEM, TreeNEM, TreeMUM;
   OutputTree.Branch("CHM", &TreeCHM, "CHM/D");
   OutputTree.Branch("NHM", &TreeNHM, "NHM/D");
   OutputTree.Branch("CEM", &TreeCEM, "CEM/D");
   OutputTree.Branch("NEM", &TreeNEM, "NEM/D");
   OutputTree.Branch("MUM", &TreeMUM, "MUM/D");

   int EntryCount = MHiEvent.Tree->GetEntries() * 1.00;
   ProgressBar Bar(cout, EntryCount);
   Bar.SetStyle(-1);

   PdfFileHelper PdfFile("SanityCheck.pdf");
   PdfFile.AddTextPage("Some visualizations :D");

   for(int iE = 0; iE < EntryCount; iE++)
   {
      if(iE < 200 || (iE % (EntryCount / 300)) == 0)
      {
         Bar.Update(iE);
         Bar.Print();
      }

      MHiEvent.GetEntry(iE);
      MJet.GetEntry(iE);
      MSDJet.GetEntry(iE);
      MPF.GetEntry(iE);
      MRho.GetEntry(iE);
      MHLT.GetEntry(iE);
      MSkim.GetEntry(iE);
         
      HN.Fill(0);

      TreeRun = MHiEvent.Run;
      TreeEvent = MHiEvent.Event;
      TreeLumi = MHiEvent.Lumi;

      TreeHF = MHiEvent.hiHF;
      TreeHFPlus = MHiEvent.hiHFplus;
      TreeHFMinus = MHiEvent.hiHFminus;
      TreeHFPlusEta4 = MHiEvent.hiHFplusEta4;
      TreeHFMinusEta4 = MHiEvent.hiHFminusEta4;
         
      TreePassPbPb40 = MHLT.CheckTrigger("HLT_HIPuAK4CaloJet40_Eta5p1_v1");
      TreePassPbPb60 = MHLT.CheckTrigger("HLT_HIPuAK4CaloJet60_Eta5p1_v1");
      TreePassPbPb80 = MHLT.CheckTrigger("HLT_HIPuAK4CaloJet80_Eta5p1_v1");
      TreePassPbPb100 = MHLT.CheckTrigger("HLT_HIPuAK4CaloJet100_Eta5p1_v1");

      if(IsData == true && IsPP == true && MHLT.CheckTrigger("HLT_AK4PFJet80_Eta5p1_v1") == false)
         continue;
      if(IsData == true && IsPP == false)
      {
         if(MHLT.CheckTrigger("HLT_HIPuAK4CaloJet40_Eta5p1_v1") == false
            && MHLT.CheckTrigger("HLT_HIPuAK4CaloJet60_Eta5p1_v1") == false
            && MHLT.CheckTrigger("HLT_HIPuAK4CaloJet80_Eta5p1_v1") == false
            && MHLT.CheckTrigger("HLT_HIPuAK4CaloJet100_Eta5p1_v1") == false)
         continue;
      }
      if(IsData == true && MSkim.PassBasicFilterLoose() == false)
         continue;

      // SDJetHelper HSDJet(MSDJet);

      TreeCentrality = GetCentrality(MHiEvent.hiBin);
      TreePTHat = MSDJet.PTHat;
      TreeRho = GetRho(MRho.EtaMax, MRho.Rho, 0, true);

      if(MSDJet.PTHat <= PTHatMin || MSDJet.PTHat > PTHatMax)
         continue;

      vector<PseudoJet> Particles;
      for(int iPF = 0; iPF < MPF.ID->size(); iPF++)
      {
         double Mass = 0;
         if((*MPF.ID)[iPF] == 1)
            Mass = 0.13957018;   // Pion...
         if(UseRhoM == false)
            Mass = 0;
         FourVector P;
         P.SetPtEtaPhiMass((*MPF.PT)[iPF], (*MPF.Eta)[iPF], (*MPF.Phi)[iPF], Mass);
         Particles.push_back(PseudoJet(P[1], P[2], P[3], P[0]));
      }
      JetDefinition Definition(antikt_algorithm, 0.4);
      ClusterSequence Sequence(Particles, Definition);
      vector<PseudoJet> Jets = Sequence.inclusive_jets();

      for(int iJ = 0; iJ < Jets.size(); iJ++)
      {
         int BestSDJet = 0;
         double BestSDDR = -1;
         for(int i = 0; i < (int)Jets.size(); i++)
         {
            double DR = GetDR(MSDJet.JetEta[i], MSDJet.JetPhi[i], Jets[iJ].eta(), Jets[iJ].phi());
            if(BestSDDR < 0 || DR < BestSDDR)
            {
               BestSDJet = i;
               BestSDDR = DR;
            }
         }

         if(BestSDDR >= 0)
         {
            TreeJetPT = MSDJet.JetPT[BestSDJet];
            TreeJetRawPT = MSDJet.JetRawPT[BestSDJet];
            TreeJetEta = MSDJet.JetEta[BestSDJet];
            TreeJetPhi = MSDJet.JetPhi[BestSDJet];

            // TreeSubJet1PT = HSDJet.RecoSubJet1[BestSDJet].GetPT();
            // TreeSubJet1Eta = HSDJet.RecoSubJet1[BestSDJet].GetEta();
            // TreeSubJet1Phi = HSDJet.RecoSubJet1[BestSDJet].GetPhi();
            // TreeSubJet2PT = HSDJet.RecoSubJet2[BestSDJet].GetPT();
            // TreeSubJet2Eta = HSDJet.RecoSubJet2[BestSDJet].GetEta();
            // TreeSubJet2Phi = HSDJet.RecoSubJet2[BestSDJet].GetPhi();

            // TreeSubJetDR = HSDJet.RecoSubJetDR[BestSDJet];
            TreeSDMass = MSDJet.JetM[BestSDJet];

            TreePartonFlavor = MSDJet.RefPartonFlavor[BestSDJet];
            TreePartonFlavorForB = MSDJet.RefPartonFlavorForB[BestSDJet];
         }

         int BestCSJet = 0;
         double BestCSDR = -1;
         for(int i = 0; i < MJet.JetCount; i++)
         {
            double DR = GetDR(Jets[iJ].eta(), Jets[iJ].phi(), MJet.JetEta[i], MJet.JetPhi[i]);
            if(BestCSDR < 0 || DR < BestCSDR)
            {
               BestCSJet = i;
               BestCSDR = DR;
            }
         }

         if(BestCSDR >= 0)
         {
            TreeCHF = MJet.JetPFCHF[BestCSJet];
            TreeNHF = MJet.JetPFNHF[BestCSJet];
            TreeCEF = MJet.JetPFCEF[BestCSJet];
            TreeNEF = MJet.JetPFNEF[BestCSJet];
            TreeMUF = MJet.JetPFMUF[BestCSJet];
            TreeCHM = MJet.JetPFCHM[BestCSJet];
            TreeNHM = MJet.JetPFNHM[BestCSJet];
            TreeCEM = MJet.JetPFCEM[BestCSJet];
            TreeNEM = MJet.JetPFNEM[BestCSJet];
            TreeMUM = MJet.JetPFMUM[BestCSJet];

            TreeJetCSPT = MJet.JetPT[BestCSJet];
            TreeJetCSRawPT = MJet.JetRawPT[BestCSJet];
            TreeJetCSEta = MJet.JetEta[BestCSJet];
            TreeJetCSPhi = MJet.JetPhi[BestCSJet];
         }

         TreeMatchDR = BestCSDR;
         TreeMatchPT = Jets[iJ].perp();
         TreeMatchEta = Jets[iJ].eta();
         TreeMatchPhi = Jets[iJ].phi();

         if(TreeMatchPT < 80)   // no need to store them!
            continue;
         if(abs(TreeMatchEta) > 1.5)
            continue;

         vector<PseudoJet> Constituents = Jets[iJ].constituents();
         vector<Node *> Nodes;
         for(int i = 0; i < (int)Constituents.size(); i++)
         {
            FourVector P;
            P[0] = Constituents[i].e();
            P[1] = Constituents[i].px();
            P[2] = Constituents[i].py();
            P[3] = Constituents[i].pz();
            Nodes.push_back(new Node(P));
         }

         BuildCATree(Nodes);

         Node *Groomed0 = FindSDNode(Nodes[0], 0.1, 0.0, 0.4);
         Node *Groomed7 = FindSDNode(Nodes[0], 0.5, 1.5, 0.4);

         TreeDepth0 = NodeDistance(Groomed0, Nodes[0]);
         TreeDepth7 = NodeDistance(Groomed7, Nodes[0]);

         if(Groomed0->N > 1)
         {
            TreeSubJetDR0 = GetDR(Groomed0->Child1->P, Groomed0->Child2->P);
            TreeSDMass0 = Groomed0->P.GetMass();

            TreeSubJet1E0   = Groomed0->Child1->P[0];
            TreeSubJet1PT0  = Groomed0->Child1->P.GetPT();
            TreeSubJet1Eta0 = Groomed0->Child1->P.GetEta();
            TreeSubJet1Phi0 = Groomed0->Child1->P.GetPhi();
            TreeSubJet2E0   = Groomed0->Child2->P[0];
            TreeSubJet2PT0  = Groomed0->Child2->P.GetPT();
            TreeSubJet2Eta0 = Groomed0->Child2->P.GetEta();
            TreeSubJet2Phi0 = Groomed0->Child2->P.GetPhi();
         }
         else
            TreeSubJetDR0 = -1;

         if(Groomed7->N > 1)
         {
            TreeSubJetDR7 = GetDR(Groomed7->Child1->P, Groomed7->Child2->P);
            TreeSDMass7 = Groomed7->P.GetMass();

            TreeSubJet1E7   = Groomed7->Child1->P[0];
            TreeSubJet1PT7  = Groomed7->Child1->P.GetPT();
            TreeSubJet1Eta7 = Groomed7->Child1->P.GetEta();
            TreeSubJet1Phi7 = Groomed7->Child1->P.GetPhi();
            TreeSubJet2E7   = Groomed7->Child1->P[0];
            TreeSubJet2PT7  = Groomed7->Child2->P.GetPT();
            TreeSubJet2Eta7 = Groomed7->Child2->P.GetEta();
            TreeSubJet2Phi7 = Groomed7->Child2->P.GetPhi();
         }
         else
            TreeSubJetDR7 = -1;

         if(Nodes.size() > 0 && Nodes[0] != NULL)
            delete Nodes[0];

         OutputTree.Fill();
      }
   }

   PdfFile.AddTimeStampPage();
   PdfFile.Close();

   Bar.Update(EntryCount);
   Bar.Print();
   Bar.PrintLine();

   OutputTree.Write();
   HN.Write();

   OutputFile.Close();

   InputFile->Close();

   return 0;
}











