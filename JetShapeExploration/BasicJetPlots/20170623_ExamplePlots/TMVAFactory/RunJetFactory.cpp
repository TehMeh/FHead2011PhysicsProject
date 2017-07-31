#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <map>
 
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"
#include "TDirectory.h"
 
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Types.h"

#include "PlotHelper3.h"

using namespace TMVA;

struct Variable{
	char type='F';
	int on=1;
	int cycles=1;
};

//string dataInput="../ScaledResult/AA6DijetCymbal.root";
//string outDir="./AAoutput/AA";
//string dataType="AA"
string dataInput="../ScaledResult/PP6Dijet.root";
string outDir="./PPoutput/";
string dataType="";

int main()
{	int VarNum=10;
	//add variables only to the back!!!
	string VariableName[VarNum]={"HalfPtMoment", "DRSquareMoment", "SmallDRPT", "MassMoment", "WidthMoment", "ParticleCount", "PTSquare", "Hadron", "HadronEta", "MyMoment"};
	string Numbers[9]={"", "05", "1", "15", "2", "25", "3", "35", "4"};

	std::map<string,Variable> VarMap;
	Variable temp;
	for (int i=0;i<VarNum;i++){
		if(i==8 || i==7)
		temp.cycles=9;
		else 
		temp.cycles=1;
		temp.on=1;
		VarMap[VariableName[i]]=temp;
	}
	//Varaible Switch
	VarMap["HalfPtMoment"].on=1;
	VarMap["DRSquareMoment"].on=1;
	VarMap["SmallDRPT"].on=1;
	VarMap["MassMoment"].on=1;
	VarMap["WidthMoment"].on=1;
	VarMap["ParticleCount"].on=1;
	VarMap["PTSquare"].on=1;
	VarMap["Hadron"].on=1; //rings. Actually 8 variables
	VarMap["HadronEta"].on=1; //rings. Actually 8 variables
	VarMap["MyMoment"].on=0; //gives RootFineder errors and a flat ROC. Really bad variable or smth wrong with the code...?
	//-----------------------
	TMVA::Tools::Instance();	
 	
	TFile *output=new TFile((outDir+"TMVAOutputAll.root").c_str(),"RECREATE");
			
	Factory *JetFactory=new Factory((dataType+"TMVAJets").c_str(), output, "Transformations=I:AnalysisType=Classification:Color:DrawProgressBar");

	TFile *input=new TFile(dataInput.c_str());

	TTree *quark=(TTree*)input->Get("LightQuarkTree");
	TTree *gluon=(TTree*)input->Get("GluonTree");

	Float_t weight=1;

	JetFactory->AddSignalTree(quark,weight);
	JetFactory->AddBackgroundTree(gluon, weight);

	//JetFactory->AddVariable("HadronDist", 'F'); //Uses std::vector. TMVA creates an event for each entry in the vector. Do not use. Replaced by "rings"
	int a;
	for (int  i=0; i<VarNum; i++){
		if (!VarMap[VariableName[i]].on) continue;
		if (VarMap[VariableName[i]].cycles>1) a=1;	
		else a=0;
		for (int k=a;k<VarMap[VariableName[i]].cycles;k++){
				JetFactory->AddVariable((VariableName[i]+Numbers[k]).c_str(),VarMap[VariableName[i]].type);
		}
	}			

	TCut TheCut="";

	JetFactory->PrepareTrainingAndTestTree(TheCut,"nTrain_Signal=0:nTest_Signal=0:nTrain_Background=0:nTest_Background=0");

	JetFactory->BookMethod(Types::kBDT, "BDT_DAB","!H:!V:NTrees=800:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
	//JetFactory->BookMethod(Types::kBDT, "BDT_Fisher", "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");

	JetFactory->TrainAllMethods();

	JetFactory->TestAllMethods();

	JetFactory->EvaluateAllMethods();
	
	output->Close();

	delete JetFactory;
	
	// single variable factories for separate ROC curves
	std::cout<<"Singles!!! "<<endl<<endl;

	for (int i=0;i<VarNum;i++){

		if (!VarMap[VariableName[i]].on){
			output=new TFile((outDir+"TMVAOutput"+VariableName[i]+".root").c_str(), "RECREATE");
			output->Close();
			continue;
		}
		output=new TFile((outDir+"TMVAOutput"+VariableName[i]+".root").c_str(), "RECREATE");

		std::cout<<VariableName[i]<<endl<<endl;
		Factory *SingleFactory=new Factory((dataType+"TMVAJets"+VariableName[i]).c_str(), output, "Transformations=I;D:Silent:AnalysisType=Classification:Color:DrawProgressBar");

		SingleFactory->AddSignalTree(quark,weight);
		SingleFactory->AddBackgroundTree(gluon,weight);
		
		if (VarMap[VariableName[i]].cycles>1) a=1;	
		else a=0;
		for (int k=a;k<VarMap[VariableName[i]].cycles;k++){
			SingleFactory->AddVariable((VariableName[i]+Numbers[k]).c_str(), VarMap[VariableName[i]].type);
		}	

		SingleFactory->PrepareTrainingAndTestTree(TheCut,"nTrain_Signal=0:nTest_Signal=0:nTrain_Background=0:nTest_Background=0");

		SingleFactory->BookMethod(Types::kBDT, "BDT_DAB","!H:!V:NTrees=800:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
		//SingleFactory->BookMethod(Types::kBDT, "BDT_Fisher", "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");


		SingleFactory->TrainAllMethods();
		
		SingleFactory->TestAllMethods();	

		SingleFactory->EvaluateAllMethods();
		
		output->Close();
		
		delete SingleFactory;

	}
	
	return 0;
}
