{	TCanvas C;

	string outDir="./PlotsAA/AA";
	string inDir="../TMVAFactory/AAoutput/AA";
	string DataID="AA";

//	string outDir="./PlotsPP/";
//	string inDir="../TMVAFactory/PPoutput/";
//	string DataID="PP";

	gStyle->SetOptStat(0);	

	struct Variable{
		int Cute;
		Float_t Line;
		int Stylish;
		int on;
	};
	
	struct Meth{
		int on;
	};	

	const int VarNum=11;
	//add variables only to the back!!!
	string VariableName[VarNum]={"All", "HalfPtMoment", "DRSquareMoment", "SmallDRPT", "MassMoment", "WidthMoment", "ParticleCount", "PTSquare", "Hadron", "HadronEta", "MyMoment"};

	int Colors[VarNum]={2, 3, 4, 46, 46, 40, 41, 29, 38, 8, 9};
	Float_t Width[2]={3, 2};
	int Style[2]={1, 2};

	int OnCount=0;
	const int MethodNum=2;		
	string Method[MethodNum]={"BDT_DAB", "BDT_Fisher"};

	std::map<string,Meth> MethMap;
	Meth tmp;
		
	TFile *input;
	for (int i=0;i<MethodNum;i++){
		input=new TFile((inDir+"TMVAOutput"+VariableName[0]+".root").c_str());
		if(((TTree*)input->Get("TestTree"))->FindBranch(Method[i].c_str())){
			tmp.on=1;
			OnCount++;
		}
		else 
		tmp.on=0;
		MethMap[Method[i]]=tmp;
	}
	//--------

	TH1F *Histo=new TH1F("Name", "", 100, 0, 1);
		

	TFile *combined;
	std::map<string,Variable> VarMap;
	Variable temp;
	for (int i=0;i<VarNum;i++){
		input=new TFile((inDir+"TMVAOutput"+VariableName[i]+".root").c_str());
		if(((TTree*)input->Get("TestTree"))) temp.on=1;
		else 
		temp.on=0;
		//input->Close();
		if (i==0){
			temp.Stylish=Style[i];
			temp.Line=Width[i];
		}
		else{
			temp.Stylish=Style[1];
			temp.Line=Width[1];
		}
		temp.Cute=Colors[i];
		VarMap[VariableName[i]]=temp;
	}
	

	combined = new TFile(("./"+DataID+"combined.root").c_str(),"recreate");
	combined->Close();
	for(int k=0;k<MethodNum;k++){
		if (!MethMap[Method[k]].on) continue;
		for (int i=0; i<VarNum;i++){
			input =new TFile((inDir+"TMVAOutput"+VariableName[i]+".root").c_str());
			combined= new TFile(("./"+DataID+"combined.root").c_str(), "UPDATE");
			Histo=(TH1F*)input->Get(("Method_BDT/"+Method[k]+"/MVA_"+Method[k]+"_trainingRejBvsS").c_str());
			Histo->SetName((VariableName[i]+"_MVA_"+Method[k]+"_trainingRejBvsS_file").c_str());
			Histo->Write();
			combined->Close();
			input->Close();
		}
	}

	std::vector<TH1F> curves;
	Histo= new TH1F("name", "", 100, 0, 1);
	for(int k=0; k<MethodNum; k++){
		for (int i=0; i<VarNum; i++){
			Histo->SetName((VariableName[i]+"_MVA_"+Method[k]+"_trainingRejBvsS").c_str());
			curves.push_back(*Histo);
		}
	}
	Histo->SetName("");

	//The start of actual ploting;

	
	std::vector<TLegend> legends;

	TLegend *Legend=new TLegend(0.1, 0.6, 0.4, 0.12, "", "NDC");
	Legend->SetTextFont(42);
   	Legend->SetTextSize(0.037);

	int counter = -1;
	int isdrawn = 0;
	
	
	input=new TFile(("./"+DataID+"combined.root").c_str());
	
	for (int k=0; k<MethodNum;k++){
		counter++;
		if (MethMap[Method[k]].on==0) continue;
		isdrawn=0;
		for (int i=0; i<VarNum; i++){
			if (VarMap[VariableName[i]].on==1){
				//input =new TFile(("../TMVAFactory/TMVAOutput"+VariableName[i]+".root").c_str());
				Histo=(TH1F*)input->Get((VariableName[i]+"_MVA_"+Method[k]+"_trainingRejBvsS_file").c_str());

				Histo->SetLineWidth(VarMap[VariableName[i]].Line);
				Histo->SetLineStyle(VarMap[VariableName[i]].Stylish);
				Histo->SetLineColor(VarMap[VariableName[i]].Cute);
				Histo->SetTitle("");
				Histo->SetBins(100, 0, 1);
					
				curves.at(k*VarNum+i)=*Histo;
				Histo->SetName("");

				if (isdrawn==1) curves.at(k*VarNum+i).Draw("same");
				else {
					curves.at(k*VarNum+i).SetTitle(("ROC for "+Method[k]+" method").c_str());
					curves.at(k*VarNum+i).Draw("");
					isdrawn=1;
				}
				//Legend->AddEntry((VariableName[i]+"_MVA_"+Method[k]+"_trainingRejBvsS").c_str(), VariableName[i].c_str(), "l");
				Legend->AddEntry(&curves.at(k*VarNum+i), VariableName[i].c_str(), "l");
			}
			
		}//end of variable cycle
		Legend->Draw();
		legends.push_back(*Legend);
		if(counter==0 && OnCount==1)
		C.SaveAs((outDir+"ROCPlots.pdf").c_str());
		else if (counter==0)
		C.SaveAs((outDir+"ROCPlots.pdf(").c_str());
		else if (counter==OnCount-1)
		C.SaveAs((outDir+"ROCPlots.pdf)").c_str());
		else
		C.SaveAs((outDir+"ROCPlots.pdf").c_str());	

		Legend->Clear();
	}//end of method cycle
	input->Delete("*;*");
	curves.clear();
	legends.clear();
	input->Close();
	C.Clear();
}
