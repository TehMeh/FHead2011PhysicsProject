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
#include "TMVA/MethodCategory.h"

#include "PlotHelper3.h"

using namespace TMVA;

struct Variable{
	char type='F';
	int on=1;
	int cycles=1;
};

string dataInput="../MyScaledResultFinal/160AA6DijetCymbal.root"; //or "../MyScaledResult/160AA6DijetCymbal.root"
string outDir="./AAoutputFinal/160AAF";	//or ./AAoutput/160AA
string dataType="AA";	
string JobName="_160TMVAJetsFinal";//or _160TMVAJets.root
//string dataInput="../MyScaledResultFinal/PP6Dijet.root";
//string outDir="./PPoutputFinal/F";
//string dataType="";

int main()
{	int VarNum=10;
	//add variables only to the back!!!
	string VariableName[VarNum]={"HalfPtMoment", "DRSquareMoment", "SmallDRPT", "MassMoment", "WidthMoment", "ParticleCount", "PTSquare", "Hadron", "HadronEta", "MyMoment"};
	string Numbers[9]={"", "05", "1", "15", "2", "25", "3", "35", "4"};

	TCut centrality[5]={"","Centrality>=0 && Centrality<0.1", "Centrality>=0.1 && Centrality<0.3","Centrality>=0.3 && Centrality<0.5", "Centrality>=0.5 && Centrality<0.8"};
	int CatNum=5;
	int b=1;
	string CentralName[5]={"","010", "1030","3050", "5080"};
	if (dataType==""){
		CatNum=1;
		b=0;
	}

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
	VarMap["Hadron"].on=0; //rings. Actually 8 variables
	VarMap["HadronEta"].on=0; //rings. Actually 8 variables
	VarMap["MyMoment"].on=1; //gives RootFinder errors and a flat ROC. Really bad variable or smth wrong with the code...?
	//-----------------------
	TMVA::Tools::Instance();	

	TFile *input=new TFile(dataInput.c_str());

	TTree *quark=(TTree*)input->Get("LightQuarkTree");
	TTree *gluon=(TTree*)input->Get("GluonTree");

	Float_t weight=1;
	int a;
	TFile *output;
	

	for (int m=b; m<CatNum;m++){
		cout<<"ALL"<<endl<<endl;
		output=new TFile((outDir+CentralName[m]+"TMVAOutputAll.root").c_str(),"RECREATE");
			
		Factory *JetFactory=new Factory((dataType+CentralName[m]+JobName).c_str(), output, "Transformations=I:Silent:AnalysisType=Classification:Color:DrawProgressBar");

		JetFactory->AddSignalTree(quark,weight);
		JetFactory->AddBackgroundTree(gluon, weight);

		//JetFactory->AddVariable("HadronDist", 'F'); //Uses std::vector. TMVA creates an event for each entry in the vector. Do not use. Replaced by "rings"
		for (int  i=0; i<VarNum; i++){
			if (!VarMap[VariableName[i]].on) continue;
			if (VarMap[VariableName[i]].cycles>1) a=1;	
			else a=0;
			for (int k=a;k<VarMap[VariableName[i]].cycles;k++){
					JetFactory->AddVariable((VariableName[i]+Numbers[k]).c_str(),VarMap[VariableName[i]].type);
			}
		}			
		if (dataType=="AA")
		JetFactory->AddSpectator("Centrality", 'F');
		JetFactory->AddSpectator("pt", 'F');
		JetFactory->AddSpectator("dRN", 'F');
		JetFactory->AddSpectator("ID", 'F');
		JetFactory->AddSpectator("newdRN", 'F');
		/*
		JetFactory->AddSpectator("pubx", 'F');
		JetFactory->AddSpectator("puCount", 'F');
		JetFactory->AddSpectator("pu0", 'F');
		*/
		
		JetFactory->PrepareTrainingAndTestTree(centrality[m],"nTrain_Signal=0:nTest_Signal=0:nTrain_Background=0:nTest_Background=0");

		JetFactory->BookMethod(Types::kBDT, "BDT_DAB","!H:!V:NTrees=800:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
		JetFactory->BookMethod(Types::kBDT, "BDT_Fisher", "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");
	
		/*
		// categories do not provide separate ROC curves, which would be kind of great
		MethodCategory *myCateg=0;
		MethodBase *categ=JetFactory->BookMethod(Types::kCategory, "centrality","");
		myCateg=dynamic_cast<MethodCategory*>(categ);
		
		std:: string var="";
		for (int i=0; i<VarNum;i++){
			if(VarMap[VariableName[i]].on)
			var=var+VariableName[i]+":";
		}
		
		myCateg->AddMethod(centrality[0], var, Types::kBDT, "BDT_DAB_010","!H:!V:NTrees=800:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
		myCateg->AddMethod(centrality[1], var, Types::kBDT, "BDT_DAB_1030","!H:!V:NTrees=800:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
		myCateg->AddMethod(centrality[2], var, Types::kBDT,"BDT_DAB_3050","!H:!V:NTrees=800:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
		*/
			
		JetFactory->TrainAllMethods();

		JetFactory->TestAllMethods();

		JetFactory->EvaluateAllMethods();
	
		output->Close();

		delete JetFactory;

	}// single variable factories for separate ROC curves
 
	std::cout<<"Singles!!! "<<endl<<endl;

	for(int m=b; m<CatNum;m++){

		for (int i=0;i<VarNum;i++){

			if (!VarMap[VariableName[i]].on){
				output=new TFile((outDir+CentralName[m]+"TMVAOutput"+VariableName[i]+".root").c_str(), "RECREATE");
				output->Close();
				continue;
			}
			output=new TFile((outDir+CentralName[m]+"TMVAOutput"+VariableName[i]+".root").c_str(), "RECREATE");

			std::cout<<VariableName[i]<<endl<<endl;

			Factory *SingleFactory=new Factory((dataType+CentralName[m]+JobName+VariableName[i]).c_str(), output, "Transformations=I;D:Silent:AnalysisType=Classification:Color:DrawProgressBar");

			SingleFactory->AddSignalTree(quark,weight);
			SingleFactory->AddBackgroundTree(gluon,weight);
		
			if (VarMap[VariableName[i]].cycles>1) a=1;	
			else a=0;
			for (int k=a;k<VarMap[VariableName[i]].cycles;k++){
				SingleFactory->AddVariable((VariableName[i]+Numbers[k]).c_str(), VarMap[VariableName[i]].type);
			}	
			if(dataType=="AA")
			SingleFactory->AddSpectator("Centrality", 'F');
			SingleFactory->AddSpectator("pt", 'F');
			SingleFactory->AddSpectator("dRN", 'F');
			SingleFactory->AddSpectator("ID", 'F');
			SingleFactory->AddSpectator("newdRN", 'F');
			/*
			SingleFactory->AddSpectator("pubx", 'F');
			SingleFactory->AddSpectator("puCount", 'F');
			SingleFactory->AddSpectator("pu0", 'F');
			*/
			SingleFactory->PrepareTrainingAndTestTree(centrality[m],"nTrain_Signal=0:nTest_Signal=0:nTrain_Background=0:nTest_Background=0");

			SingleFactory->BookMethod(Types::kBDT, "BDT_DAB","!H:!V:NTrees=800:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");
			SingleFactory->BookMethod(Types::kBDT, "BDT_Fisher", "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");


			SingleFactory->TrainAllMethods();
		
			SingleFactory->TestAllMethods();	

			SingleFactory->EvaluateAllMethods();
		
			output->Close();
		
			delete SingleFactory;
		} //end of variable cycle, i
	}//end of centrality cycle, m
	
	return 0;
}
