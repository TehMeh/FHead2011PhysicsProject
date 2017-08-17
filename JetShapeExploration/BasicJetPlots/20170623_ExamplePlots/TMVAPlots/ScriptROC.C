{	//ROCPlot.pdf
	//This script is different in a sense that it does not create separate plots for each centrality	
	TCanvas C;

	string outDir="./PlotsAAFinal/160AAF";
	string inDir="../TMVAFactory/AAoutputFinal/160AAF";
	string DataID="AA";

	//string outDir="./PlotsPPFinal/F";
	//string inDir="../TMVAFactory/PPoutputFinal/F";
	//string DataID="PP";

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


	const int CatNum=5;
	int c=1; //will be used when cycling over centralities
	int otherCatNum=CatNum;
	string CentralName[CatNum]={"", "010", "1030", "3050", "5080"};	
	string CentralVal[CatNum]={"--", "<0.1", ">0.1 && <0.3", ">0.3 && <0.5", ">0.5 && <0.8"};
	// cycle switch for PP data	
	if(DataID=="PP"){
		c=0;
		otherCatNum=1;
	}

	const int VarNum=11;
	//add variables only to the back!!!
	string VariableName[VarNum]={"All", "HalfPtMoment", "DRSquareMoment", "SmallDRPT", "MassMoment", "WidthMoment", "ParticleCount", "PTSquare", "Hadron", "HadronEta", "MyMoment"};

	int Colors[VarNum]={2, 3, 4, 49, 46, 40, 41, 29, 38, 8, 9};
	Float_t Width[2]={3, 2};
	int Style[5]={1, 2, 3, 4 , 5};

	int OnCount=0;
	const int MethodNum=2;		
	string Method[MethodNum]={"BDT_DAB", "BDT_Fisher"};

	std::map<string,Meth> MethMap;
	Meth tmp;
		
	TFile *input;
	for (int i=0;i<MethodNum;i++){
		input=new TFile((inDir+CentralName[c]+"TMVAOutput"+VariableName[0]+".root").c_str());
		if(((TTree*)input->Get("TestTree"))->FindBranch(Method[i].c_str())){ //if a required branch is found in the tree, the method will be turned on
			tmp.on=1;
			OnCount++;
		}
		else 
		tmp.on=0;
		MethMap[Method[i]]=tmp;
	}
	//--------
	cout<<"Method switch complete!"<<endl;

	TH1F *Histo=new TH1F("Name", "", 100, 0, 1);
		
	std::map<string,Variable> VarMap;
	Variable temp;
	int VarOnCount=0;
	// no need to use cycles. This only extracts ROCs
	for (int i=0;i<VarNum;i++){
		input=new TFile((inDir+CentralName[c]+"TMVAOutput"+VariableName[i]+".root").c_str());
		if(((TTree*)input->Get("TestTree"))){
			temp.on=1;
			VarOnCount++;
		}
		else 
		temp.on=0;
		//input->Close();
		if (i==0){
			temp.Stylish=Style[i];
			temp.Line=Width[i];
		}
		else{
			temp.Stylish=Style[i%4+1];
			temp.Line=Width[1];
		}
		temp.Cute=Colors[i];
		VarMap[VariableName[i]]=temp;
	}
	
	cout<<"Variable switch complete!"<<endl;


	std::vector<TH1F> curves;
	std::vector<TH1F> effS, effB, signif; 
	Histo= new TH1F("name", "", 100, 0, 1);
	for (int z=c; z<otherCatNum; z++){
		for(int k=0; k<MethodNum; k++){
			for (int i=0; i<VarNum; i++){
				Histo->SetName((CentralName[z]+VariableName[i]+"_MVA_"+Method[k]+"_trainingRejBvsS").c_str());
				curves.push_back(*Histo);
			}
		}
	}
	Histo->SetName("the histo");

	//The start of actual ploting!;

	std::vector<TLegend> legends;

	TLegend *Legend=new TLegend(0.1, 0.6, 0.4, 0.12, "", "NDC");
	Legend->SetTextFont(42);
   	Legend->SetTextSize(0.037);

	int counter = -1;
	int isdrawn = 0;
	int indx;	
	
	for (int z=c; z<otherCatNum; z++){
		indx=(z-c)*MethodNum*VarNum-1;
		for (int k=0; k<MethodNum;k++){
	
			counter++;
			if (MethMap[Method[k]].on==0) continue;
			isdrawn=0;
			for (int i=0; i<VarNum; i++){
				indx++;
				if (VarMap[VariableName[i]].on==1){
					input =new TFile((inDir+CentralName[z]+"TMVAOutput"+VariableName[i]+".root").c_str());
					Histo=(TH1F*)input->Get(("Method_BDT/"+Method[k]+"/MVA_"+Method[k]+"_trainingRejBvsS").c_str());

					Histo->SetName( (CentralName[z]+"_"+VariableName[i]+"_"+Method[k]+"_trainingRejBvsS_file").c_str() );
			
					Histo->SetLineWidth(VarMap[VariableName[i]].Line);
					Histo->SetLineStyle(VarMap[VariableName[i]].Stylish);
					Histo->SetLineColor(VarMap[VariableName[i]].Cute);
					Histo->SetTitle("");
					Histo->SetBins(100, 0, 1);
					
					curves.at(indx)=*Histo;
					Histo->SetName("");

					if (isdrawn==1) curves.at(indx).Draw("same");
					else {
						curves.at(indx).SetTitle(("ROC for "+Method[k]+" method and centrality "+CentralVal[z]).c_str());
						curves.at(indx).Draw("");
						isdrawn=1;
					}
					Legend->AddEntry(&curves.at(indx), VariableName[i].c_str(), "l");
				}
			}//end of variable cycle, i
			Legend->Draw();
			legends.push_back(*Legend);
			if(c==0 && OnCount==1)
			C.SaveAs((outDir+"ROCPlots.pdf").c_str());
			else if (indx==VarNum-1)
			C.SaveAs((outDir+"ROCPlots.pdf(").c_str());
			else if (indx==(otherCatNum-c)*VarNum*MethodNum-1)
			C.SaveAs((outDir+"ROCPlots.pdf)").c_str());
			else
			C.SaveAs((outDir+"ROCPlots.pdf").c_str());	

			Legend->Clear();
		}//end of method cycle,k
	}//end of category cycle,z
	legends.clear();
	input->Close();
	C.Clear();
	curves.clear();
}
