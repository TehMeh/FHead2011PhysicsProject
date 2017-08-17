#include <iostream>
#include <string>
#include <cstring>
#include <cmath>

using namespace std;

#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

#include "ProgressBar.h"

#include "BasicUtilities.h"
#include "Messenger.h"

//Analysis prototypes for Data and MC
void DataAnalyzer(bool &IsPP, bool &IsPPHiReco, bool &IsPA, TFile *InputFile, TFile &OutputFile, HiEventTreeMessenger &MHiEvent, JetTreeMessenger &MJet, PFTreeMessenger &MPF, SkimTreeMessenger &MSkim, TriggerTreeMessenger &MHLT, double &PTHatMin, double &PTHatMax, GGTreeMessenger &MGG);
void MCAnalyzer(bool &IsPP, bool &IsPPHiReco, bool &IsPA, TFile *InputFile, TFile &OutputFile, HiEventTreeMessenger &MHiEvent, JetTreeMessenger &MJet, PFTreeMessenger &MPF, SkimTreeMessenger &MSkim, TriggerTreeMessenger &MHLT, double &PTHatMin, double &PTHatMax, GGTreeMessenger &MGG);			

//------------------------------//
//								//
// 0.1 for MC and 0.1 for data	// 
//								//
//------------------------------//
int main(int argc, char *argv[])
{
   // Checking if input is good
   if(argc != 6)
   {
      cerr << "Usage: " << argv[0] << " Input Output Tag PTHatMin PTHatMax" << endl;
      return -1;
   }

   // Getting inputs
   string Input = argv[1];
   string Output = argv[2];
   string Tag = argv[3];
   double PTHatMin = atof(argv[4]);
   double PTHatMax = atof(argv[5]);

   // Is it data or MC?  Is it pp or PbPb or pPb?  etc.
   bool IsData = IsDataFromTag(Tag);
   bool IsPP = IsPPFromTag(Tag);
   bool IsPPHiReco = IsPPHiRecoFromTag(Tag);
   bool IsPA = IsPAFromTag(Tag);

   // Prepare input
   TFile *InputFile = TFile::Open(Input.c_str());
   if(InputFile == NULL)
   {
      cerr << "Error!  Input file error!" << endl;
      return -1;
   }
	
	string tree1, tree2;
	
	if(IsPP){
		tree1="ak4PFJetAnalyzer/t";
		tree2="pfcandAnalyzer/pfTree";
	}
	else if (!IsPP){
		tree1="akCs4PFJetAnalyzer/t";
		tree2="pfcandAnalyzerCS/pfTree";
	}

   HiEventTreeMessenger MHiEvent(InputFile);
   JetTreeMessenger MJet(InputFile, tree1);
   PFTreeMessenger MPF(InputFile, tree2);
   SkimTreeMessenger MSkim(InputFile);
   TriggerTreeMessenger MHLT(InputFile);
   GGTreeMessenger MGG(InputFile);

   if(MHiEvent.Tree == NULL)
   {
      cerr << "Error!  HiEvent tree not found in the input file!" << endl;
      return -1;
   }

   	// Prepare output
   	TFile OutputFile(Output.c_str(), "RECREATE");
	if(IsData)
		DataAnalyzer(IsPP, IsPPHiReco, IsPA, InputFile, OutputFile, MHiEvent, MJet, MPF, MSkim, MHLT, PTHatMin, PTHatMax, MGG );
	else if (!IsData)
		MCAnalyzer(IsPP, IsPPHiReco, IsPA, InputFile, OutputFile, MHiEvent, MJet, MPF, MSkim, MHLT, PTHatMin, PTHatMax, MGG );
	
	// Cleanup
   	InputFile->Close();

   return 0;
}


void MCAnalyzer(bool &IsPP, bool &IsPPHiReco, bool &IsPA, TFile *InputFile, TFile &OutputFile, HiEventTreeMessenger &MHiEvent, JetTreeMessenger &MJet, PFTreeMessenger &MPF, SkimTreeMessenger &MSkim, TriggerTreeMessenger &MHLT, double &PTHatMin, double &PTHatMax, GGTreeMessenger &MGG )
{
   TH1D HN("HN", "Raw event count", 1, 0, 1); //important!
   TH1D HPTHat("HPTHat", ";PTHat;", 100, 0, 500);
   TH1D HPTHatSelected("HPTHatSelected", ";PTHat;", 100, 0, 500);
   TH1D HJetPT("HJetPT", ";Jet PT;", 100, 0, 500);
   TH1D HJetEta("HJetEta", "JetPT > 50;Jet Eta;", 100, -5, 5);
   TH1D HJetPhi("HJetPhi", "JetPT > 50;Jet Phi;", 100, -M_PI, M_PI);
   TH1D HJetEtaSmallPT("HJetEtaSmallPT", "JetPT < 50;Jet Eta;", 100, -5, 5);
   TH1D HJetPhiSmallPT("HJetPhiSmallPT", "JetPT < 50;Jet Phi;", 100, -M_PI, M_PI);
  
   TH2D HJetRawPTvsJetPT("HJetRawPTvsJetPT","RawPT;PT",100,0,500,100,0,500);
   TH1D HPFPT("HPFPT", ";PF PT", 100, 0, 60);
   TH1D HPFE("HPFE", ";PF E", 100, 0, 60);
   TH1D HPFEta("HPFEta" , ";PF Eta", 100, -5, 5);
   TH1D HPFPhi("HPFPhi", ";PF Phi", 100,-M_PI, M_PI);
   // Initiate necessary arrays and histograms 
   const int IDNum=4;
   const int VarNum=8;
   // Parameters for Count: Charged, Electron, Muon, Photon, Hadron
   const int Param[IDNum+1][3]= { {300, 0, 300}, {10, 0, 10}, {10, 0, 10}, {300, 0, 300}, { 60, 0 , 60} }; 
   // Bin Parameters for DeltaR, DelatEta,DeltaPhi distributions 
   const double JetParamX[3][3]={ {100, 0, 0.4}, {100, -0.4, 0.4}, {100, -0.4, 0.4} };
   string IDName[IDNum+1]= {"Charged","Electron", "Muon", "Photon", "Hadron"};
   string PartonName[5]={"All", "Light", "Charm", "Bottom", "Gluon"};
   string VarName[VarNum+1]={"Count", "PT", "E", "Eta", "Phi", "CountInJet", "DeltaR", "DeltaEta", "DeltaPhi"};
   TH1D* HPTE=new TH1D("PTE", ";", 100, 0, 500);
   TH1D* HCount=new TH1D("Count", ";", 1, 0 , 1);
   TH1D* HEta=new TH1D("Eta", ";", 100, -5, 5);
   TH1D* HPhi=new TH1D("Phi", ";", 100, -M_PI, M_PI);
   TH2D* HJetDistrib= new TH2D("Jet", "", 1, 0, 1, 1, 0 , 1 );
   std::vector<TH1D> vPT, vEta, vPhi, vEnergy, vCount, vCountInJet, vPTPercent, vHadCountDRParton;
   std::vector<std::vector<TH2D> > vPTDist, vPTParton;
   std::vector<std::vector<TH1D> > vCountDist, vMoments;
   std::vector<TH2D> Temp2D;
   std::vector<TH1D> Temp1D;
   int IDCount[IDNum+1]={ 0, 0, 0, 0, 0};
   int indx;
   Float_t DR,DEta,DPhi;
   Float_t PercentDR=0.1, PTRatio[5]={0, 0, 0, 0, 0};
   // moment parameters 

   int amX=4;
   int amY=4;
   Float_t  x[amX]={0, 1, 1.5, 2}, y[amY]={0, 1, 2, -1.5};
   string xVal[amX]={"0", "1", "1.5", "2"};
   string yVal[amY]={"0", "1", "2", "-1.5"};
   Float_t PTFrac,DRFrac;
   Float_t moment[4][amX][amY];
   for(int m=0;m<4;m++){
	   for (int i=0;i<amX;i++){
	    	for (int k=0; k<amY;k++){
			moment[m][i][k]=0;
			}  
	   }
   }	

   //Trees
   TTree *LightQuarkTree=new TTree("LightQuarkTree", "");
   TTree *GluonTree=new TTree("GluonTree", "");
   TTree *PUTree=new TTree("PUTree", "");
   //Tree branches
   std::vector<Float_t> vHadronDistTree;
   Float_t HalfPtMoment,DRSquareMoment, WidthMoment, MassMoment, SmallDRPT, ParticleCount, PTSquare, MyMoment;
   Float_t Hadron[8]={0, 0, 0, 0, 0, 0, 0, 0}; //hadron count per ring
   Float_t HadronEta[8]={0, 0, 0, 0, 0, 0, 0, 0}; //hadron count per eta "ring"
   std::vector<Float_t> puCount, pubx, dRN, vpt, newdRN;
   Float_t pu0;
   std:vector<Float_t> vID;
   Float_t newPhi, newEta, newdR, px, py, pz;
   

   TBranch *HadDistLightQ=LightQuarkTree->Branch("HadronDist", &vHadronDistTree);
   TBranch *HadDistGluon=GluonTree->Branch("HadronDist", &vHadronDistTree);
   TBranch *HalfPtLightQ=LightQuarkTree->Branch("HalfPtMoment", &HalfPtMoment, "HalfPtMoment/F");
   TBranch *HalfPtGluon=GluonTree->Branch("HalfPtMoment", &HalfPtMoment, "HalfPtMoment/F");
   TBranch *DRSquareMomentLightQ=LightQuarkTree->Branch("DRSquareMoment", &DRSquareMoment, "DRSquareMoment/F");
   TBranch *DRquareMomentGluon=GluonTree->Branch("DRSquareMoment", &DRSquareMoment, "DRSquareMoment/F");
   TBranch *WidthMomentLightQuark=LightQuarkTree->Branch("WidthMoment", &WidthMoment, "WidthMoment/F");
   TBranch *WidthMomentGluon=GluonTree->Branch("WidthMoment", &WidthMoment, "WidthMoment/F");
   TBranch *MassMomentLightQuark=LightQuarkTree->Branch("MassMomemnt", &MassMoment, "MassMoment/F");
   TBranch *MassMomentGluon=GluonTree->Branch("MassMomemnt", &MassMoment, "MassMoment/F");
   TBranch *SmallDRPTLightQuark=LightQuarkTree->Branch("SmallDRPT", &SmallDRPT, "SmallDRPT/F");
   TBranch *SmallDRPTGluon=GluonTree->Branch("SmallDRPT", &SmallDRPT, "SmallDRPT/F");
   TBranch *ParticleCountLightQuark=LightQuarkTree->Branch("ParticleCount", &ParticleCount, "ParticleCount/F");
   TBranch *ParticleCountGluon=GluonTree->Branch("ParticleCount", &ParticleCount, "ParticleCount/F");
   TBranch *PTSquareLightQuark=LightQuarkTree->Branch("PTSquare", &PTSquare, "PTSquare/F");
   TBranch *PTSquareGluon=GluonTree->Branch("PTSquare", &PTSquare, "PTSquare/F");
  
   TBranch *puCountGluon=GluonTree->Branch("puCount", &puCount);
   TBranch *puCountLightQuark=LightQuarkTree->Branch("puCount", &puCount);
   TBranch *pubxGluon=GluonTree->Branch("pubx", &pubx);
   TBranch *pubxLightQuark=LightQuarkTree->Branch("pubx", &pubx);
   TBranch *dRNLightQuark=LightQuarkTree->Branch("dRN", &dRN);
   TBranch *dRNGluon=GluonTree->Branch("dRN", &dRN);
   TBranch *ptGluon=GluonTree->Branch("pt", &vpt);
   TBranch *ptLightQuark=LightQuarkTree->Branch("pt", &vpt);
   TBranch *pu0Gluon=GluonTree->Branch("pu0", &pu0, "pu0/F");
   TBranch *pu0LightQuark=LightQuarkTree->Branch("pu0", &pu0, "pu0/F");
  	
   TBranch *Hadron05LightQuark=LightQuarkTree->Branch("Hadron05", &Hadron[0], "Hadron05/F");
   TBranch *Hadron05Gluon=GluonTree->Branch("Hadron05", &Hadron[0], "Hadron05/F");
   TBranch *Hadron1LightQuark=LightQuarkTree->Branch("Hadron1", &Hadron[1], "Hadron1/F");
   TBranch *Hadron1Gluon=GluonTree->Branch("Hadron1", &Hadron[1], "Hadron1/F");
   TBranch *Hadron15LightQuark=LightQuarkTree->Branch("Hadron15", &Hadron[2], "Hadron15/F");
   TBranch *Hadron15Gluon=GluonTree->Branch("Hadron15", &Hadron[2], "Hadron15/F");
   TBranch *Hadron2LightQuark=LightQuarkTree->Branch("Hadron2", &Hadron[3], "Hadron2/F");
   TBranch *Hadron2Gluon=GluonTree->Branch("Hadron2", &Hadron[3], "Hadron2/F");
   TBranch *Hadron25LightQuark=LightQuarkTree->Branch("Hadron25", &Hadron[4], "Hadron25/F");
   TBranch *Hadron25Gluon=GluonTree->Branch("Hadron25", &Hadron[4], "Hadron25/F");
   TBranch *Hadron3LightQuark=LightQuarkTree->Branch("Hadron3", &Hadron[5], "Hadron3/F");
   TBranch *Hadron3Gluon=GluonTree->Branch("Hadron3", &Hadron[5], "Hadron3/F");
   TBranch *Hadron35LightQuark=LightQuarkTree->Branch("Hadron35", &Hadron[6], "Hadron35/F");
   TBranch *Hadron35Gluon=GluonTree->Branch("Hadron35", &Hadron[6], "Hadron35/F");
   TBranch *Hadron4LightQuark=LightQuarkTree->Branch("Hadron4", &Hadron[7], "Hadron4/F");
   TBranch *Hadron4Gluon=GluonTree->Branch("Hadron4", &Hadron[7], "Hadron4/F");

   TBranch *HadronEta05LightQuark=LightQuarkTree->Branch("HadronEta05", &HadronEta[0], "HadronEta05/F");
   TBranch *HadronEta05Gluon=GluonTree->Branch("HadronEta05", &HadronEta[0], "HadronEta05/F");
   TBranch *HadronEta1LightQuark=LightQuarkTree->Branch("HadronEta1", &HadronEta[1], "HadronEta1/F");
   TBranch *HadronEta1Gluon=GluonTree->Branch("HadronEta1", &HadronEta[1], "HadronEta1/F");
   TBranch *HadronEta15LightQuark=LightQuarkTree->Branch("HadronEta15", &HadronEta[2], "HadronEta15/F");
   TBranch *HadronEta15Gluon=GluonTree->Branch("HadronEta15", &HadronEta[2], "HadronEta15/F");
   TBranch *HadronEta2LightQuark=LightQuarkTree->Branch("HadronEta2", &HadronEta[3], "HadronEta2/F");
   TBranch *HadronEta2Gluon=GluonTree->Branch("HadronEta2", &HadronEta[3], "HadronEta2/F");
   TBranch *HadronEta25LightQuark=LightQuarkTree->Branch("HadronEta25", &HadronEta[4], "HadronEta25/F");
   TBranch *HadronEta25Gluon=GluonTree->Branch("HadronEta25", &HadronEta[4], "HadronEta25/F");
   TBranch *HadronEta3LightQuark=LightQuarkTree->Branch("HadronEta3", &HadronEta[5], "HadronEta3/F");
   TBranch *HadronEta3Gluon=GluonTree->Branch("HadronEta3", &HadronEta[5], "HadronEta3/F");
   TBranch *HadronEta35LightQuark=LightQuarkTree->Branch("HadronEta35", &HadronEta[6], "HadronEta35/F");
   TBranch *HadronEta35Gluon=GluonTree->Branch("HadronEta35", &HadronEta[6], "HadronEta35/F");
   TBranch *HadronEta4LightQuark=LightQuarkTree->Branch("HadronEta4", &HadronEta[7], "HadronEta4/F");
   TBranch *HadronEta4Gluon=GluonTree->Branch("HadronEta4", &HadronEta[7], "HadronEta4/F");


   TBranch *MyMomentLightQuark=LightQuarkTree->Branch("MyMoment", &MyMoment, "MyMoment/F");
   TBranch *MyMomentGluon=GluonTree->Branch("MyMoment", &MyMoment, "MyMoment/F");

   TBranch *puCountPU=PUTree->Branch("puCount", &puCount);
   TBranch *pubxPU=PUTree->Branch("pubx", &pubx);
   TBranch *dRNPU=PUTree->Branch("dRN", &dRN);
   TBranch *ptPU=PUTree->Branch("pt", &vpt);
   TBranch *pu0PU=PUTree->Branch("pu0", &pu0, "pu0/F");

	TBranch *IDLightQuark=LightQuarkTree->Branch("ID", &vID);
	TBranch *IDGluon=GluonTree->Branch("ID", &vID);
	TBranch *IDPU=PUTree->Branch("ID", &vID);

	TBranch *newDRGluon=GluonTree->Branch("newdRN", &newdRN);
	TBranch *newDRLightQuark=LightQuarkTree->Branch("newdRN", &newdRN);
	TBranch *newDRPU=PUTree->Branch("newdRN", &newdRN);
	
	
	//----centrality tree
	Float_t Centrality;

	TBranch *dataCenterQ=LightQuarkTree->Branch("Centrality", &Centrality, "Centrality/F");
	TBranch *dataCenterG=GluonTree->Branch("Centrality", &Centrality, "Centrality/F");	

   // Making histogram vectors
   for (int i=0;i<=IDNum;i++){
     HCount->SetName(("H"+IDName[i]+VarName[0]).c_str());
     HCount->SetTitle((";"+VarName[0]).c_str());
     HCount->SetBins(Param[i][0], Param[i][1], Param[i][2]);
     vCount.push_back(*HCount);
     
     HPTE->SetName(("H"+IDName[i]+VarName[1]).c_str());
     HPTE->SetTitle((";"+VarName[1]).c_str());
     vPT.push_back(*HPTE);

     HPTE->SetName(("H"+IDName[i]+VarName[2]).c_str());
     HPTE->SetTitle((";"+VarName[2]).c_str());
     vEnergy.push_back(*HPTE);

     HEta->SetName(("H"+IDName[i]+VarName[3]).c_str());
     HEta->SetTitle((";"+VarName[3]).c_str());
     vEta.push_back(*HEta);
     
     HPhi->SetName(("H"+IDName[i]+VarName[4]).c_str());
     HPhi->SetTitle((";"+VarName[4]).c_str());
     vPhi.push_back(*HPhi);

     HCount->SetName(("HJet"+IDName[i]+VarName[5]).c_str());
     HCount->SetTitle((";"+VarName[5]).c_str());
     HCount->SetBins(Param[i][0], Param[i][1], Param[i][2]);
     vCountInJet.push_back(*HCount);
   }

   // create 1D & 2D histograms for distributions DR, DEta, DPhi
 
   for (int k=1; k<4;k++){
       for (int i=0; i<=IDNum;i++){
	 HJetDistrib->SetName(("HJet"+IDName[i]+"PTvs"+VarName[5+k]).c_str());
	 HJetDistrib->SetTitle(("PT;"+VarName[5+k]).c_str());
	 HJetDistrib->SetBins(JetParamX[k-1][0],JetParamX[k-1][1],JetParamX[k-1][2], 100, 0, 500 );
	 Temp2D.push_back(*HJetDistrib);
	 
	 HCount->SetName(("HJet"+IDName[i]+"Countvs"+VarName[5+k]).c_str());
	 HCount->SetTitle((";"+VarName[5+k]).c_str());
	 HCount->SetBins(JetParamX[k-1][0],JetParamX[k-1][1], JetParamX[k-1][2]);
	 Temp1D.push_back(*HCount);
       }
       vPTDist.push_back(Temp2D);
       vCountDist.push_back(Temp1D);
       Temp1D.clear();
       Temp2D.clear();
   }
   // quark and gluon separetd PT(dR) disrtibution
   HJetDistrib->SetBins(80, 0, 0.4, 50, 0 ,7);
   HCount->SetBins(100, 0, 0.4);
   for (int i=1;i<5;i++){
	HCount->SetName(("HJetHadronCountDR"+PartonName[i]).c_str());
	HCount->SetTitle(";DeltaR");
	vHadCountDRParton.push_back(*HCount); //Light, b, c g
	for (int k=0;k<=IDNum;k++){
		HJetDistrib->SetName(("HJet"+IDName[k]+"PT"+PartonName[i]).c_str());
		HJetDistrib->SetTitle("PT;DeltaR");
		Temp2D.push_back(*HJetDistrib);
	}
	vPTParton.push_back(Temp2D); //Light, b, c g
	Temp2D.clear();
   }
   // percentage in DR<0.1 vector creation
   HCount->SetBins(100, 0 , 1);
   for (int i=0;i<5;i++){
	HCount->SetName(("HJetPTPercent"+PartonName[i]).c_str());
	HCount->SetTitle(";PT ratio");
	vPTPercent.push_back(*HCount); //Light, b, c g
   }
   // Moment histogram vector creation
   for (int i=1; i<5;i++){
	for (int k=0; k<4;k++){
		for (int j=0;j<4;j++){
			if(k==0 && j==0)
			HCount->SetBins(100, 0, 100);
			else if (j==3)
			HCount->SetBins(305,-5,300);
			else if (k==0 && j>0)
			HCount->SetBins(100, 0, 25);
			else if((k==2 && j>0) || (k==1 && j==2))
			HCount->SetBins(100, 0 , 0.5);
			else if(k==1 && j==1)
			HCount->SetBins(100, 0, 0.6);
			else if (k==1 && j==0)
			HCount->SetBins(100, 0.85, 1 );
			else if(k==3 && j>0)
			HCount->SetBins(100, 0, 0.09);
			else 
			HCount->SetBins(100, 0, 1);
		
			HCount->SetName(("HMoment"+PartonName[i]+"_x="+xVal[k]+"_y="+yVal[j]).c_str());
			HCount->SetTitle(";");
			Temp1D.push_back(*HCount);
		}
	}
	vMoments.push_back(Temp1D);
	Temp1D.clear();
   }



   // Loop over events
   int EntryCount = MHiEvent.Tree->GetEntries() * 0.1;

   ProgressBar Bar(cout, EntryCount);
   Bar.SetStyle(-1);
   for(int iE = 0; iE < EntryCount; iE++)
   {  
      // Progress bar :D
      Bar.Update(iE);
      Bar.PrintWithMod(200);

      // Get event from trees
      MHiEvent.GetEntry(iE);
      MJet.GetEntry(iE);
      MPF.GetEntry(iE);
      MSkim.GetEntry(iE);
      MHLT.GetEntry(iE);
	  MGG.GetEntry(iE);

      int used[MPF.ID->size()];
      for (int u=0; u<MPF.ID->size();u++){
		used[u]=0;
      }

	  for(int pu=0; pu<MGG.NPUInfo; pu++){
		 if(MGG.PUBX->at(pu)==0)
			pu0=(Float_t)MGG.PUCount->at(pu);

		 puCount.push_back(MGG.PUCount->at(pu));
		 pubx.push_back(MGG.PUBX->at(pu));
	  }

      // Fill histograms
      HN.Fill(0);
      HPTHat.Fill(MJet.PTHat);
      for (int iJ=0; iJ<MJet.JetCount; iJ++){
		HJetRawPTvsJetPT.Fill(MJet.JetRawPT[iJ],MJet.JetPT[iJ]);
      }

      if(MJet.PTHat < PTHatMin || MJet.PTHat >= PTHatMax)
         continue;

	  Centrality=GetCentrality(MHiEvent.hiBin);

      HPTHatSelected.Fill(MJet.PTHat);

      for(int iJ = 0; iJ < MJet.JetCount; iJ++)
      {
		
	 
         HJetPT.Fill(MJet.JetPT[iJ]);
         if(MJet.JetPT[iJ] > 50)
         {
            HJetEta.Fill(MJet.JetEta[iJ]);
            HJetPhi.Fill(MJet.JetPhi[iJ]);
         }
         else
         {
            HJetEtaSmallPT.Fill(MJet.JetEta[iJ]);
            HJetPhiSmallPT.Fill(MJet.JetPhi[iJ]);
         }
		 //Jet selection and histogram filling
		 //if (MJet.JetPT[iJ]<=100 || abs(MJet.JetEta[iJ])>=1)
		 //	continue;
		 px=0;
		 py=0;
		 pz=0; //this is enought to get the angles, no need for py
		 //axis recalcualtion cycle
		 for (unsigned int i=0; i!=MPF.ID->size(); i++){
			 DR=GetDR(MPF.Eta->at(i), MPF.Phi->at(i), MJet.JetEta[iJ], MJet.JetPhi[iJ]);
			   if (MPF.ID->at(i)>0 && MPF.ID->at(i)<6 && MJet.JetPT[iJ]>100 && abs(MJet.JetEta[iJ])<1){
				   if(DR<0.4 && used[i]==0)
				 		dRN.push_back(DR);
				}
			 if (DR>=0.4 || used[i]==1)
			 	continue;
				px+=MPF.PT->at(i)*cos(MPF.Phi->at(i));
				py+=MPF.PT->at(i)*sin(MPF.Phi->at(i));
				pz+=MPF.PT->at(i)*sinh(MPF.Eta->at(i));	
		  }//end of axis recalculation
		newPhi=acos(px/sqrt(pow(px,2)+pow(py,2)));
		newEta=asinh(pz/sqrt(pow(px,2)+pow(py,2)));
		px=0;
		py=0;
		pz=0;
		for (unsigned int i=0; i!=MPF.ID->size(); i++){
			 newdR=GetDR(newEta, newPhi, MPF.Eta->at(i), MPF.Phi->at(i));
			 if (newdR<=0.4 && used[i]==0){
				px+=MPF.PT->at(i)*cos(MPF.Phi->at(i));
				py+=MPF.PT->at(i)*sin(MPF.Phi->at(i));
				pz+=MPF.PT->at(i)*sinh(MPF.Eta->at(i));
			}
		 }//end of jet pt recalculation
		if (sqrt(pow(px,2)+pow(py,2))<=100 || abs(newEta)>=1)
		   	continue;
		 for (unsigned int i=0; i!=MPF.ID->size(); i++){
		   //DR=GetDR(MPF.Eta->at(i), MPF.Phi->at(i), MJet.JetEta[iJ], MJet.JetPhi[iJ]);
		   newdR=GetDR(newEta, newPhi, MPF.Eta->at(i), MPF.Phi->at(i));
		   //if(newdR<0.4 && used[i]==0)

		   if (MPF.ID->at(i)>0 && MPF.ID->at(i)<6){
		 	   if (newdR>0.4 || used[i]==1)
					continue;
			   used[i]=1;
			   DEta=MJet.JetEta[iJ]-MPF.Eta->at(i);
			   DPhi=MJet.JetPhi[iJ]-MPF.Phi->at(i);
			   if (DPhi<-M_PI) DPhi=DPhi+2*M_PI;
			   else if (DPhi>M_PI) DPhi=DPhi-2*M_PI;

			   indx=MPF.ID->at(i)-1;

			   //PTFrac=MPF.PT->at(i)/MJet.JetPT[iJ];
			   PTFrac=MPF.PT->at(i)/sqrt(pow(px,2)+pow(py,2));
			   DRFrac=newdR/0.4;	
	   
			   vID.push_back(MPF.ID->at(i));
			   newdRN.push_back(newdR);
			   vpt.push_back(MPF.PT->at(i));

			   if (newdR<PercentDR){
				PTRatio[0]=PTRatio[0]+PTFrac;
			   }
			   // Parton separation
				//u,d,s
			   if (abs(MJet.RefPartonFlavor[iJ])==1 || abs(MJet.RefPartonFlavor[iJ])==2 || abs(MJet.RefPartonFlavor[iJ])==3){
				   		vPTParton.at(0).at(indx).Fill(DR,MPF.PT->at(i));
					if (indx==4){ //if hadron
			  	   		vHadCountDRParton.at(0).Fill(DR);
						vHadronDistTree.push_back(DR);
						for(int r=1; r<9; r++){ 
							if (DR>=0.05*(r-1) && DR<0.05*r)
								Hadron[r-1]++;
							if (DEta>=0.05*(r-1) && DEta<0.05*r)
								HadronEta[r-1]++;
						}   
					}
					if (newdR<PercentDR){//small dR
						PTRatio[1]=PTRatio[1]+PTFrac;
					}
					for (int j=0;j<amX;j++){ //moments
						for (int m=0;m<amY;m++){
							if (y[m]<0 && DRFrac==0)				
								continue;
							moment[0][j][m]+=pow(PTFrac,x[j])*pow(DRFrac,y[m]);
						}
					}
			   }
				//Charm
			   else if (abs(MJet.RefPartonFlavor[iJ])==4){
					vPTParton.at(1).at(indx).Fill(DR,MPF.PT->at(i));
					if (indx==4){
			  	   		vHadCountDRParton.at(1).Fill(DR);
					}
					if (newdR<PercentDR){
						PTRatio[2]=PTRatio[2]+PTFrac;
				   	}
					for (int j=0;j<amX;j++){
						for (int m=0;m<amY;m++){
							if (y[m]<0 && DRFrac==0)
							continue;
							moment[1][j][m]+=pow(PTFrac,x[j])*pow(DRFrac,y[m]);
						}
					}
			   }
				//bottom
			   else if (abs(MJet.RefPartonFlavor[iJ])==5){
					vPTParton.at(2).at(indx).Fill(DR,MPF.PT->at(i));
					if (indx==4){
			  	   		vHadCountDRParton.at(2).Fill(DR);
					}
					if (newdR<PercentDR){
						PTRatio[3]=PTRatio[3]+PTFrac;
				   	}
					for (int j=0;j<amX;j++){
						for (int m=0;m<amY;m++){
							if (y[m]<0 && DRFrac==0)
							continue;
							moment[2][j][m]+=pow(PTFrac,x[j])*pow(DRFrac,y[m]);
						}
					}
			   }
				//Gluon
			   else if (abs(MJet.RefPartonFlavor[iJ])==21){
					vPTParton.at(3).at(indx).Fill(DR,MPF.PT->at(i));
					if (indx==4){
			  	   		vHadCountDRParton.at(3).Fill(DR);
						vHadronDistTree.push_back(DR);
						for(int r=1;r<9;r++){
						if (DR>=0.05*(r-1) && DR<0.05*r)
							Hadron[r-1]++;
						if (DEta>=0.05*(r-1) && DEta<0.05*r)
							HadronEta[r-1]++;
						}
					}
					if (newdR<PercentDR){
						PTRatio[4]=PTRatio[4]+PTFrac;
					}
					for (int j=0;j<amX;j++){
						for (int m=0;m<amY;m++){
							if (y[m]<0 && DRFrac==0)	continue;
							moment[3][j][m]+=pow(PTFrac,x[j])*pow(DRFrac,y[m]);
						}
					}
			   }


			   IDCount[indx]++;
			   vPTDist.at(0).at(indx).Fill(DR,MPF.PT->at(i));
			   vPTDist.at(1).at(indx).Fill(DEta,MPF.PT->at(i));
			   vPTDist.at(2).at(indx).Fill(DPhi,MPF.PT->at(i));
				 
			   vCountDist.at(0).at(indx).Fill(DR);
			   vCountDist.at(1).at(indx).Fill(DEta);
			   vCountDist.at(2).at(indx).Fill(DPhi);
		   } //if ID
		 }// for cycle, particles in jet
	
		 if (MJet.RefPartonFlavor[iJ]==1 || MJet.RefPartonFlavor[iJ]==2 || MJet.RefPartonFlavor[iJ]==3 ){
			HalfPtMoment=moment[0][2][0]; //Pt^1.5
			DRSquareMoment=moment[0][0][2];	//dR^2
			WidthMoment=moment[0][1][1]; // Pt^1dR^1
			MassMoment=moment[0][1][2]; // Pt^1dR^2
			SmallDRPT=PTRatio[1]; //Pt in dR<0.1
			ParticleCount=moment[0][0][0]; // ^0
			PTSquare=moment[0][3][0];//Pt^2
			MyMoment=moment[0][3][3];//Pt^2dR^(-1.5)
			LightQuarkTree->Fill();
		 }
		 else if (MJet.RefPartonFlavor[iJ]==21){
			HalfPtMoment=moment[3][2][0];
			DRSquareMoment=moment[3][0][2];	
			WidthMoment=moment[3][1][1];
			MassMoment=moment[3][1][2];
			SmallDRPT=PTRatio[4];
			ParticleCount=moment[3][0][0];
			PTSquare=moment[3][3][0];
			MyMoment=moment[3][3][3];	
			GluonTree->Fill();
		 }
		if(MJet.RefPartonFlavor[iJ]==1 || MJet.RefPartonFlavor[iJ]==2 || MJet.RefPartonFlavor[iJ]==3 || MJet.RefPartonFlavor[iJ]==21)
			PUTree->Fill();

		vHadronDistTree.clear();
		dRN.clear();
		newdRN.clear();
		vpt.clear();
		vID.clear();
		for(int r=0;r<8;r++){
			Hadron[r]=0;
			HadronEta[r]=0;
		}
		for(int r=0;r<8;r++) Hadron[r]=0;
		 for(int k=0;k<5;k++){
			if (PTRatio[k]!=0)
		 	vPTPercent.at(k).Fill(PTRatio[k]);
		 	PTRatio[k]=0;
		 }
		 for (int k=0;k<=IDNum;k++){
		   	if (IDCount[k]!=0)
	 	   	vCountInJet.at(k).Fill(IDCount[k]);
		   	IDCount[k]=0;
		 	 }
		 
		 for (int k=0;k<4;k++){
			for (int m=0; m<amX;m++){
				for(int n=0;n<amY;n++){
					if (moment[k][m][n]!=0)
					vMoments.at(k).at(m*4+n).Fill(moment[k][m][n]);
					moment[k][m][n]=0;
				} 
			}
		 }	
     }// Jet cycle
     
      for(unsigned int i=0; i!=MPF.ID->size(); i++){
		HPFPT.Fill(MPF.PT->at(i));
		HPFE.Fill(MPF.E->at(i));
		HPFEta.Fill(MPF.Eta->at(i));
		HPFPhi.Fill(MPF.Phi->at(i));
		   
	
		if (MPF.ID->at(i)>0 && MPF.ID->at(i)<6){
		  indx=MPF.ID->at(i)-1;
		  IDCount[indx]++;
		  vPT.at(indx).Fill(MPF.PT->at(i));
		  vEnergy.at(indx).Fill(MPF.E->at(i));
		  vEta.at(indx).Fill(MPF.Eta->at(i));
		  vPhi.at(indx).Fill(MPF.Phi->at(i));
		}  

      }
      for (int k=0;k<=IDNum;k++){
	 	if(IDCount[k]!=0)
		vCount.at(k).Fill(IDCount[k]);
		IDCount[k]=0;
      }

	  pubx.clear();
	  puCount.clear();
   }//end of event cycle

   Bar.Update(EntryCount);
   Bar.Print();
   Bar.PrintLine();

   // Write stuff out
   HN.Write();
   HPTHat.Write();
   HPTHatSelected.Write();

   HJetPT.Write();
   HJetEta.Write();
   HJetPhi.Write();
   HJetEtaSmallPT.Write();
   HJetPhiSmallPT.Write();
   HPFPT.Write();
   HPFE.Write();
   HPFEta.Write();
   HPFPhi.Write();

   for (int k=0;k<=IDNum;k++){
     vCount.at(k).Write();
     vPT.at(k).Write();
     vEnergy.at(k).Write();
     vEta.at(k).Write();
     vPhi.at(k).Write();
     vCountInJet.at(k).Write();
     for (int j=0;j<3;j++){
       vPTDist.at(j).at(k).Write();
       vCountDist.at(j).at(k).Write();
     }
   }

  for (int i=0;i<4;i++){
	for (int k=0;k<=IDNum;k++){
		vPTParton.at(i).at(k).Write();
	}
   }

  for(int i=0;i<5;i++){
	vPTPercent.at(i).Write();
	if(i<4)
	vHadCountDRParton.at(i).Write();
  }

  for (int i=0;i<4;i++){
	for (int k=0;k<amX*amY;k++){
		vMoments.at(i).at(k).Write();
	}
  }

  //Tree writing
  LightQuarkTree->Write();
  GluonTree->Write();
  PUTree->Write();

}
//end of MCAnalayzer




void DataAnalyzer(bool &IsPP, bool &IsPPHiReco, bool &IsPA, TFile *InputFile, TFile &OutputFile, HiEventTreeMessenger &MHiEvent, JetTreeMessenger &MJet, PFTreeMessenger &MPF, SkimTreeMessenger &MSkim, TriggerTreeMessenger &MHLT, double &PTHatMin, double &PTHatMax, GGTreeMessenger &MGG )
{
	TH1D HN("HN", "Raw event count", 1, 0, 1); 
	
	TTree *DataTree=new TTree("Name", "");

   	if(IsPP)
	DataTree->SetName("PPTree");
	else if (!IsPP)
	DataTree->SetName("PbPbTree");


   	std::vector<Float_t> vHadronDistTree, dRN, vpt, newdRN;
   	Float_t HalfPtMoment,DRSquareMoment, WidthMoment, MassMoment, SmallDRPT, ParticleCount, PTSquare, MyMoment;
   	Float_t Hadron[8]={0, 0, 0, 0, 0, 0, 0, 0};
	Float_t HadronEta[8]={0, 0, 0, 0, 0, 0, 0, 0};
	std::vector<Float_t> vID;
	Float_t px,py,pz, newEta, newPhi, newdR;
	//Tree branches
   	TBranch *HadDistData=DataTree->Branch("HadronDist", &vHadronDistTree);
   	TBranch *HalfPtData=DataTree->Branch("HalfPtMoment", &HalfPtMoment, "HalfPtMoment/F");
   	TBranch *DRSquareMomentData=DataTree->Branch("DRSquareMoment", &DRSquareMoment, "DRSquareMoment/F");
   	TBranch *WidthMomentData=DataTree->Branch("WidthMoment", &WidthMoment, "WidthMoment/F");
   	TBranch *MassMomentData=DataTree->Branch("MassMoment", &MassMoment, "MassMoment/F");
   	TBranch *SmallDRPTData=DataTree->Branch("SmallDRPT", &SmallDRPT, "SmallDRPT/F");
   	TBranch *ParticleCountData=DataTree->Branch("ParticleCount", &ParticleCount, "ParticleCount/F");
   	TBranch *PTSquareData=DataTree->Branch("PTSquare", &PTSquare, "PTSquare/F");

   	TBranch *Hadron05Data=DataTree->Branch("Hadron05", &Hadron[0], "Hadron05/F");
   	TBranch *Hadron1Data=DataTree->Branch("Hadron1", &Hadron[1], "Hadron1/F");
   	TBranch *Hadron15Data=DataTree->Branch("Hadron15", &Hadron[2], "Hadron15/F");
   	TBranch *Hadron2Data=DataTree->Branch("Hadron2", &Hadron[3], "Hadron2/F");
   	TBranch *Hadron25Data=DataTree->Branch("Hadron25", &Hadron[4], "Hadron25/F");
   	TBranch *Hadron3Data=DataTree->Branch("Hadron3", &Hadron[5], "Hadron3/F");
   	TBranch *Hadron35Data=DataTree->Branch("Hadron35", &Hadron[6], "Hadron35/F");
   	TBranch *Hadron4Data=DataTree->Branch("Hadron4", &Hadron[7], "Hadron4/F");

	TBranch *HadronEta05Data=DataTree->Branch("HadronEta05", &HadronEta[0], "HadronEta05/F");
   	TBranch *HadronEta1Data=DataTree->Branch("HadronEta1", &HadronEta[1], "HadronEta1/F");
   	TBranch *HadronEta15Data=DataTree->Branch("HadronEta15", &HadronEta[2], "HadronEta15/F");
   	TBranch *HadronEta2Data=DataTree->Branch("HadronEta2", &HadronEta[3], "HadronEta2/F");
   	TBranch *HadronEta25Data=DataTree->Branch("HadronEta25", &HadronEta[4], "HadronEta25/F");
   	TBranch *HadronEta3Data=DataTree->Branch("HadronEta3", &HadronEta[5], "HadronEta3/F");
   	TBranch *HadronEta35Data=DataTree->Branch("HadronEta35", &HadronEta[6], "HadronEta35/F");
   	TBranch *HadronEta4Data=DataTree->Branch("HadronEta4", &HadronEta[7], "HadronEta4/F");

   	TBranch *MyMomentData=DataTree->Branch("MyMoment", &MyMoment, "MyMoment/F");
	
	TBranch *dRNData=DataTree->Branch("dRN", &dRN);
	TBranch *ptData=DataTree->Branch("pt", &vpt);
	TBranch *partIDData=DataTree->Branch("ID", &vID);
	TBranch *newDRData=DataTree->Branch("newdRN", &newdRN);
	//----centrality 
	Float_t Centrality;

	TBranch *dataCenter=DataTree->Branch("Centrality", &Centrality, "Centrality/F");

	Float_t DR,DEta, DPhi, PTFrac, DRFrac, PTRatio=0;
	int indx;
	const Float_t PercentDR=0.1;	

	int amX=4;
	int amY=4;
	Float_t  x[amX]={0, 1, 1.5, 2}, y[amY]={0, 1, 2, -1.5};	
	Float_t DataMoment[amX][amY];
	for (int i=0; i<amX;i++){
		for(int j=0; j<amY; j++){
			DataMoment[i][j]=0;
		}
	}
	
	int EntryCount = MHiEvent.Tree->GetEntries() * 0.1;

	ProgressBar Bar(cout, EntryCount);
	Bar.SetStyle(-1);
	for(int iE = 0; iE < EntryCount; iE++)
   	{  
      	// Progress bar :D
      	Bar.Update(iE);
      	Bar.PrintWithMod(200);

      	// Get event from trees
      	MHiEvent.GetEntry(iE);
      	MJet.GetEntry(iE);
      	MPF.GetEntry(iE);
      	MSkim.GetEntry(iE);
      	MHLT.GetEntry(iE);	
		MGG.GetEntry(iE);
      	int used[MPF.ID->size()];
      	for (int u=0; u<MPF.ID->size();u++){
			used[u]=0;
      	}
		//Triggers
		if (IsPP){
			if (MHLT.CheckTrigger("HLT_AK4PFJet80_Eta5p1_v1")!=1)
				continue;
		}
		else if (!IsPP){
			if (MHLT.CheckTrigger("HLT_HIPuAK4CaloJet100_Eta5p1_v1")!=1)
				continue; 
		}

      	HN.Fill(0);
		
		Centrality=GetCentrality(MHiEvent.hiBin);	
		
		for (int iJ=0;iJ<MJet.JetCount; iJ++)
		{	
			//Selection
			//if (MJet.JetPT[iJ]<=100 || abs(MJet.JetEta[iJ])>=1)
		   	//	continue;
			px=0;
			py=0;
			pz=0; //this is enough to get the angles, no need for py
			 //axis recalcualtion cycle
			 for (unsigned int i=0; i!=MPF.ID->size(); i++){
				 DR=GetDR(MPF.Eta->at(i), MPF.Phi->at(i), MJet.JetEta[iJ], MJet.JetPhi[iJ]);
				   if (MPF.ID->at(i)>0 && MPF.ID->at(i)<6 && MJet.JetPT[iJ]>100 && abs(MJet.JetEta[iJ])<1){
				   		if(DR<0.4 && used[i]==0)
				 			dRN.push_back(DR);
					}
				 if (DR>=0.4 || used[i]==1)
				 	continue;
			
				px+=MPF.PT->at(i)*cos(MPF.Phi->at(i));
				py+=MPF.PT->at(i)*sin(MPF.Phi->at(i));
				pz+=MPF.PT->at(i)*sinh(MPF.Eta->at(i));	
			 }//end of axis recalculation
			 newPhi=acos(px/sqrt(pow(px,2)+pow(py,2)));
			 newEta=asinh(pz/sqrt(pow(px,2)+pow(py,2)));
			px=0;
			py=0;
			pz=0;
			for (unsigned int i=0; i!=MPF.ID->size(); i++){
				 newdR=GetDR(newEta, newPhi, MPF.Eta->at(i), MPF.Phi->at(i));
				  if (newdR<=0.4 && used[i]==0){
					px+=MPF.PT->at(i)*cos(MPF.Phi->at(i));
					py+=MPF.PT->at(i)*sin(MPF.Phi->at(i));
					pz+=MPF.PT->at(i)*sinh(MPF.Eta->at(i));
				  }
			}//end of jet pt recalculation
			if (sqrt(pow(px,2)+pow(py,2))<=100 || abs(newEta)>=1)
		   		continue;
			for (unsigned int i=0; i!=MPF.ID->size(); i++){
		   		//DR=GetDR(MPF.Eta->at(i), MPF.Phi->at(i), MJet.JetEta[iJ], MJet.JetPhi[iJ]);
				newdR=GetDR(newEta, newPhi, MPF.Eta->at(i), MPF.Phi->at(i));
	
		  		if (MPF.ID->at(i)>0 && MPF.ID->at(i)<6){
		 			if (newdR>0.4 || used[i]==1)
						continue;
					DEta=MJet.JetEta[iJ]-MPF.Eta->at(i);
			   		DPhi=MJet.JetPhi[iJ]-MPF.Phi->at(i);
			   		if (DPhi<-M_PI) DPhi=DPhi+2*M_PI;
			  		else if (DPhi>M_PI) DPhi=DPhi-2*M_PI;
			   		used[i]=1;
			   		indx=MPF.ID->at(i)-1;
			   		//PTFrac=MPF.PT->at(i)/MJet.JetPT[iJ];
					PTFrac=MPF.PT->at(i)/sqrt(pow(px,2)+pow(py,2));
			   		DRFrac=newdR/0.4;
					vID.push_back(MPF.ID->at(i));
					//filling DataMoments
					for (int j=0;j<amX;j++){
						for (int m=0;m<amY;m++){
							if (y[m]<0 && DRFrac==0)			
								continue;
							DataMoment[j][m]+=pow(PTFrac,x[j])*pow(DRFrac,y[m]);
						}
					}//end of DataMoment filling
					if (newdR<PercentDR){
						PTRatio=PTRatio+PTFrac;
				   	}
					if (indx==4){
						vHadronDistTree.push_back(DR);
						for(int r=1;r<9;r++){
							if (DR>=0.05*(r-1) && DR<0.05*r)
								Hadron[r-1]++;
							if (DEta>=0.05*(r-1) && DEta<0.05*r)
								HadronEta[r-1]++;
						}
					}
					newdRN.push_back(newdR);
					vpt.push_back(MPF.PT->at(i));
				} //end of ID if 
			}//end of PF in jet cylcle

			//Tree Filling
			HalfPtMoment=DataMoment[2][0];
			DRSquareMoment=DataMoment[0][2];	
			WidthMoment=DataMoment[1][1];
			MassMoment=DataMoment[1][2];
			SmallDRPT=PTRatio;
			ParticleCount=DataMoment[0][0];
			PTSquare=DataMoment[3][0];
			MyMoment=DataMoment[3][3];	

			DataTree->Fill();

			// Cleaning/Resetting
			vHadronDistTree.clear();
			dRN.clear();	
			newdRN.clear();
			vpt.clear();
			vID.clear();
			PTRatio=0;
			for (int i=0; i<amX;i++){
				for(int j=0; j<amY; j++){
					DataMoment[i][j]=0;
				}
			}
			for (int r=0;r<8;r++){
				Hadron[r]=0;
				HadronEta[r]=0;
			}
		
		} // end if iJ cycle
	}//end of iE cycle
	HN.Write();
	DataTree->Write();

}
