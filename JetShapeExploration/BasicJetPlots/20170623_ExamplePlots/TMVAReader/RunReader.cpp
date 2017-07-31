#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
 
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TBranch.h"
 
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#include "TMVA/MethodCuts.h"
#include "TMVA/Types.h"

using namespace TMVA;

int main()
{
	Tools::Instance();

	TMVA::Reader *JetReader = new TMVA::Reader( "!Color:!Silent" );

	const int VarNum=10;
	//add variables only to the back!!!
	std::string VariableName[VarNum]={"HalfPtMoment", "DRSquareMoment", "SmallDRPT", "MassMoment", "WidthMoment", "ParticleCount", "PTSquare", "Hadron", "HadronEta", "MyMoment"};

	std::string Numbers[9]={"", "05", "1", "15", "2", "25", "3", "35", "4"};
	
	const int MethodNum=2;		
	std::string Method[MethodNum]={"BDT_DAB", "BDT_Fisher"};

	TFile *test=new TFile("../TMVAFactory/TMVAOutputAll.root");

	struct Variable{
		int on=1;
		int cycles=1;
	};

	struct Meth{
		int on=1;
	};
		
	// checking whats on and creating maps
	Variable tmpvar;
	Meth tmpmeth;	
	
	std::map<std::string,Variable>	VarMap;
	std::map<std::string,Meth>	MethMap;
	int a; //just for Hadron and HadronEta checking. Numbers[a].
	for (int i=0;i<VarNum;i++){
		a=0;
		if (i==7 || i==8){
			tmpvar.cycles=9;
			a=1;
		}
		if (((TTree*)(test->Get("TestTree")))->FindBranch((VariableName[i]+Numbers[a]).c_str())) tmpvar.on=1;

		else tmpvar.on=0;
		VarMap[VariableName[i]]=tmpvar;		
	}

	for (int i=0; i<MethodNum;i++){
		if (((TTree*)(test->Get("TestTree")))->FindBranch(Method[i].c_str())) tmpmeth.on=1;
		else tmpmeth.on=0;
		MethMap[Method[i]]=tmpmeth;
	}
	
	test->Close();

	//end of checking waht is on and what is not
	Float_t HalfPtMoment,DRSquareMoment, WidthMoment, MassMoment, SmallDRPT, ParticleCount, PTSquare, MyMoment;
	Float_t Hadron[8]={0, 0, 0, 0, 0, 0, 0, 0};
	Float_t HadronEta[8]={0, 0, 0, 0, 0, 0, 0, 0};

	//JetReader->AddVariable("HadronDist", &HadronDist); Do not use!
	if(VarMap["HalfPtMoment"].on)
	JetReader->AddVariable("HalfPtMoment", &HalfPtMoment);
	if(VarMap["DRSquareMoment"].on)
	JetReader->AddVariable("DRSquareMoment", &DRSquareMoment);
	if(VarMap["SmallDRPT"].on)
	JetReader->AddVariable("SmallDRPT", &SmallDRPT);
	if(VarMap["MassMoment"].on)
	JetReader->AddVariable("MassMoment", &MassMoment);
	if(VarMap["WidthMoment"].on)
	JetReader->AddVariable("WidthMoment", &WidthMoment);
	if(VarMap["ParticleCount"].on)
	JetReader->AddVariable("ParticleCount", &ParticleCount);
	if(VarMap["PTSquare"].on)
	JetReader->AddVariable("PTSquare", &PTSquare);

	if(VarMap["Hadron"].on){
		JetReader->AddVariable("Hadron05", &Hadron[0]);
		JetReader->AddVariable("Hadron1", &Hadron[1]);
		JetReader->AddVariable("Hadron15", &Hadron[2]);
		JetReader->AddVariable("Hadron2", &Hadron[3]);
		JetReader->AddVariable("Hadron25", &Hadron[4]);
		JetReader->AddVariable("Hadron3", &Hadron[5]);
		JetReader->AddVariable("Hadron35", &Hadron[6]);
		JetReader->AddVariable("Hadron4", &Hadron[7]);
	}

	if(VarMap["HadronEta"].on){ 
		JetReader->AddVariable("HadronEta05", &HadronEta[0]);
		JetReader->AddVariable("HadronEta1", &HadronEta[1]);
		JetReader->AddVariable("HadronEta15", &HadronEta[2]);
		JetReader->AddVariable("HadronEta2", &HadronEta[3]);
		JetReader->AddVariable("HadronEta25", &HadronEta[4]);
		JetReader->AddVariable("HadronEta3", &HadronEta[5]);
		JetReader->AddVariable("HadronEta35", &HadronEta[6]);
		JetReader->AddVariable("HadronEta4", &HadronEta[7]);
	}

	if(VarMap["MyMoment"].on)
	JetReader->AddVariable("MyMoment", &MyMoment);


	std::string dir = "../TMVAFactory/weights/";	
	

	TFile *input=new TFile("../ScaledResult/PPDataHighPTJet_NoCrossSection.root");
	TTree *MapleTree=(TTree*)input->Get("PPTree");

	//MapleTree->SetBranchAddress("HadronDist", &HadronDist); //Do not use!	

	if(VarMap["HalfPtMoment"].on)
	MapleTree->SetBranchAddress("HalfPtMoment", &HalfPtMoment);
	if(VarMap["DRSquareMoment"].on)
	MapleTree->SetBranchAddress("DRSquareMoment", &DRSquareMoment);
	if(VarMap["SmallDRPT"].on)
	MapleTree->SetBranchAddress("SmallDRPT", &SmallDRPT);
	if(VarMap["MassMoment"].on)
	MapleTree->SetBranchAddress("MassMoment", &MassMoment);
	if(VarMap["WidthMoment"].on)
	MapleTree->SetBranchAddress("WidthMoment", &WidthMoment);
	if(VarMap["ParticleCount"].on)
	MapleTree->SetBranchAddress("ParticleCount", &ParticleCount);
	if(VarMap["PTSquare"].on)
	MapleTree->SetBranchAddress("PTSquare", &PTSquare);
	
	if(VarMap["Hadron"].on){
		MapleTree->SetBranchAddress("Hadron05", &Hadron[0]);
		MapleTree->SetBranchAddress("Hadron1", &Hadron[1]);
		MapleTree->SetBranchAddress("Hadron15", &Hadron[2]);
		MapleTree->SetBranchAddress("Hadron2", &Hadron[3]);
		MapleTree->SetBranchAddress("Hadron25", &Hadron[4]);
		MapleTree->SetBranchAddress("Hadron3", &Hadron[5]);
		MapleTree->SetBranchAddress("Hadron35", &Hadron[6]);
		MapleTree->SetBranchAddress("Hadron4", &Hadron[7]);
	}
	
	if(VarMap["HadronEta"].on){
		MapleTree->SetBranchAddress("HadronEta05", &HadronEta[0]);
		MapleTree->SetBranchAddress("HadronEta1", &HadronEta[1]);
		MapleTree->SetBranchAddress("HadronEta15", &HadronEta[2]);
		MapleTree->SetBranchAddress("HadronEta2", &HadronEta[3]);
		MapleTree->SetBranchAddress("HadronEta25", &HadronEta[4]);
		MapleTree->SetBranchAddress("HadronEta3", &HadronEta[5]);
		MapleTree->SetBranchAddress("HadronEta35", &HadronEta[6]);
		MapleTree->SetBranchAddress("HadronEta4", &HadronEta[7]);
	}

	if(VarMap["MyMoment"].on)
	MapleTree->SetBranchAddress("MyMoment", &MyMoment);
	
	TFile *target=new TFile("TMVApp.root","RECREATE" );

	TH1F *BDT=new TH1F("BDT", "", 200, -1, 1);
	std::vector<TH1F> MethHist;
	
	for (int k=0;k<MethodNum;k++){
		if (MethMap[Method[k]].on){
			JetReader->BookMVA(Method[k].c_str(),(dir+"TMVAJets_"+Method[k]+".weights.xml").c_str());
			BDT->SetName(("Evaluation_"+Method[k]).c_str());
			MethHist.push_back(*BDT);
		}
	}

	//Cloning MapleTree form the input to outTree for the output
	TTree *OutTree=MapleTree->CloneTree();	

	//Method Evaluation variable
	Double_t MethodEval[MethodNum];
	
	// adding additional branches to the outTree
	TBranch *BranchArr[MethodNum];
	//std::vector<TBranch> outBranch;	
	
	for (int k=0;k<MethodNum;k++){
		if (MethMap[Method[k]].on){
			BranchArr[k]=OutTree->Branch(Method[k].c_str(), &MethodEval[k], (Method[k]+"/D").c_str());
		}
	}

	// Start of the Actual Evaluation of MVAs
	std::cout << "--- Processing: " << MapleTree->GetEntries() << " events" << std::endl;
	TStopwatch sw;
	sw.Start();

	int counter=-1;
	for (Long64_t ievt=0; ievt<MapleTree->GetEntries();ievt++)
	{
			if (ievt%500 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

			MapleTree->GetEntry(ievt);
			
			counter=-1;
			for (int k=0; k<MethodNum;k++){
				if(MethMap[Method[k]].on){
					counter++;
					MethHist.at(counter).Fill(JetReader->EvaluateMVA(Method[k].c_str()));
					MethodEval[k]=JetReader->EvaluateMVA(Method[k].c_str());
					BranchArr[k]->Fill();
				}
			}
	}
	sw.Stop();
	std::cout << "--- End of event loop: "; sw.Print();
	
	OutTree->Write();	
	counter=-1;
	for (int i=0;i<MethodNum;i++){
		if(MethMap[Method[i]].on){
			counter++;
			MethHist.at(counter).Write();
		}
	}		
	
	target->Close();
	input->Close();

	MethHist.clear();

	delete JetReader;

	return 0;
}
