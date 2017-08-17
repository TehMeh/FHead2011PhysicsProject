{
	gStyle->SetOptStat(0);

	TCanvas C;

	string outDir="./PlotsAAFinal/160AAF"; // ./PlotsAA/160AA or ./PlotsAA/AA
	string inDir="../TMVAFactory/AAoutputFinal/160AAF";
	string DataID="PbPb";

	string reader="../TMVAReader/F160";  //PPweightAA/PPweight"; // or "...reader/160" or "....reader/". This line is never commented.

	//string outDir="./PlotsPPFinal/F";
	//string inDir="../TMVAFactory/PPoutputFinal/F";
	//string DataID="PP";

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

	struct Variable{
		int Cute=3;
		Float_t Line=1;
		int Stylish=1;
		int on=1;
		int cycles=1;
		float range[3]={1, 0, 1};
		string unit="none";
	};
	
	struct Meth{
		int on=1;
	};	

	const int VarNum=10;
	int VarOnCount=0;
	//add variables only to the back!!!
	string VariableName[VarNum]={"HalfPtMoment", "DRSquareMoment", "SmallDRPT", "MassMoment", "WidthMoment", "ParticleCount", "PTSquare", "Hadron", "HadronEta", "MyMoment"};
	string Numbers[9]={"", "05", "1", "15", "2", "25", "3", "35", "4"};

	float axis[VarNum][3]={ {100, 0, 1}, {100, 0, 25}, {100, 0, 1}, {100, 0, 0.6}, {100, 0, 0.6}, {100, 0, 100 }, {100, 0, 1}, {10, 0, 5}, {10, 0, 5}, {120, 0, 40}};
	string AxisName[VarNum]={"sum[Pt^1.5]", "sum[dR^2]", "sum[Pt], dR<0.1", "sum[Pt*dR^2]", "sum[Pt*dR]", "Multiplicity", "sum[Pt^2]", "<=dR<", "<=dEta<", "sum[Pt^2*dR^(-1.5)]"};
	string FancyName[VarNum]={ "#sum_{i#epsilonjet}#(){#frac{p_{T}^{i}}{p_{T}^{jet}}}^{1.5}", "#sum_{i#epsilonjet}#(){#frac{dR^{i}}{dR^{jet}}}^{2}","#sum_{i#epsilonjet}#(){#frac{p_{T}^{i}}{p_{T}^{jet}}}, dR<0.1", "#sum_{i#epsilonjet}#(){#frac{p_{T}^{i}}{p_{T}^{jet}}}#(){#frac{dR^{i}}{dR^{jet}}}^{2}","#sum_{i#epsilonjet}#(){#frac{p_{T}^{i}}{p_{T}^{jet}}}#(){#frac{dR^{i}}{dR^{jet}}}","Multiplicity", "#sum_{i#epsilonjet}#(){#frac{p_{T}^{i}}{p_{T}^{jet}}}^{2}", "<=dR<", "<=dEta<", "#sum_{i#epsilonjet}#(){#frac{p_{T}^{i}}{p_{T}^{jet}}}^{2}#(){#frac{dR^{i}}{dR^{jet}}}^{-1.5}" };


	string rings[10]={"", "0", "0.01", "0.1", "0.15", "0.2", "0.25", "0.3", "0.35", "0.4"}; //used with Hadron and HadronEta axis naming

	int OnCount=0;
	const int MethodNum=2;		
	string Method[MethodNum]={"BDT_Fisher", "BDT_DAB"};
	string Bound="0";
	string comp[2]={"<", ">"};
	std::map<string,Meth> MethMap;
	Meth tmp;
		
	TFile *input;
	for (int i=0;i<MethodNum;i++){
		input=new TFile((inDir+CentralName[c]+"TMVAOutputAll.root").c_str());
		if(((TTree*)input->Get("TestTree"))->FindBranch(Method[i].c_str())){
			tmp.on=1;
			OnCount++;
		}
		else 
		tmp.on=0;
		MethMap[Method[i]]=tmp;
	}

	std::map<string,Variable> VarMap;
	Variable temp;
	for (int i=0;i<VarNum;i++){
		input=new TFile((inDir+CentralName[c]+"TMVAOutput"+VariableName[i]+".root").c_str());

		if (i==7 || i==8) temp.cycles=9;
		else temp.cycles=1;		

		if(!((TTree*)input->Get("TestTree"))) temp.on=0;
		else {
			temp.on=1;
			VarOnCount++;
		}
		temp.range[0]=axis[i][0];
		temp.range[1]=axis[i][1];
		temp.range[2]=axis[i][2];
		VarMap[VariableName[i]]=temp;
		input->Close();
	}
	input=0;

//end of preparations. Variable switching and such
//-------------------------------------------------------------------
//start info gathering and actual plotting;

	std::vector<TH1F> data, MCquark, MCgluon;
	TH1F *Hist=new TH1F("temp", "", 1,0,1);
	TFile *inputData; // "input" reused from before
	TTree *MCTree, *DataTree;	
	THStack *FatStack=new THStack("stacker", "");
	std::vector<THStack> stacks;

	TLegend Legend(0.55, 0.9, 0.9, 0.73, "", "NDC");
	Legend.SetTextFont(42);
	Legend.SetTextSize(0.045);	
	std::vector<TLegend> legends;
	
	for(int k=0; k<MethodNum; k++){
		for(int z=c; z<otherCatNum; z++){
			//Hist->SetBins(200,-1,1);
			Hist->SetName((CentralName[z]+"_"+Method[k]+"_data").c_str());
			//Hist->SetLineColor(kBlack);
			//Hist->SetLineWidth(3);
			data.push_back(*Hist);
			
			//Hist->SetLineWidth(2);
			Hist->SetName((CentralName[z]+Method[k]+"_MCgluon").c_str());
			//Hist->SetLineColor(kBlue);
			MCgluon.push_back(*Hist);
			
			Hist->SetName((CentralName[z]+Method[k]+"_MCquark").c_str());
			//Hist->SetLineColor(kRed);
			MCquark.push_back(*Hist);

			FatStack->SetName((CentralName[z]+Method[k]).c_str());
			stacks.push_back(*FatStack);

			legends.push_back(Legend);
		}//end of methood cycle, k
	}//end of centrality cycle, z
	C.Clear();
	Hist->SetName("empty");

	int indx;
	int OnCounter;
	C.Clear();
	int swop;
	OnCounter=-1;
	for(int k=0; k<MethodNum; k++){
		indx=k*(otherCatNum-c)-1;
		if (!MethMap[Method[k]].on) continue;
		OnCounter++;
		C.DivideSquare(otherCatNum-c);

		for(int z=c; z<otherCatNum; z++){
			if(otherCatNum==1)
				swop=1;
			else swop=z;
			C.cd(swop);
			indx++;
			inputData=new TFile((reader+DataID+CentralName[z]+"TMVA.root").c_str());	
			input=new TFile((inDir+CentralName[z]+"TMVAOutputAll.root").c_str());

			DataTree=(TTree*)inputData->Get((DataID+"Tree").c_str());
			MCTree=(TTree*)input->Get("TestTree");

			DataTree->Draw((Method[k]+">>"+CentralName[z]+"_"+Method[k]+"_data").c_str(), "", "");
			MCTree->Draw((Method[k]+">>"+CentralName[z]+Method[k]+"_MCgluon").c_str(), "classID == 1", "same");
			MCTree->Draw((Method[k]+">>"+CentralName[z]+Method[k]+"_MCquark").c_str(), "classID == 0", "same");
			data.at(indx)=*(TH1F*)gPad->GetPrimitive((CentralName[z]+"_"+Method[k]+"_data").c_str());
			MCquark.at(indx)=*(TH1F*)gPad->GetPrimitive((CentralName[z]+Method[k]+"_MCquark").c_str());
			MCgluon.at(indx)=*(TH1F*)gPad->GetPrimitive((CentralName[z]+Method[k]+"_MCgluon").c_str());	
			data.at(indx).Scale(1/data.at(indx).Integral());
			MCquark.at(indx).Scale(1/(MCquark.at(indx).Integral()+MCgluon.at(indx).Integral()));
			MCgluon.at(indx).Scale(1/(MCquark.at(indx).Integral()+MCgluon.at(indx).Integral()));
			legends.at(indx).AddEntry(&data.at(indx), (DataID+"_data").c_str(), "l");
			legends.at(indx).AddEntry(&MCquark.at(indx),(DataID+"_MC quark").c_str(), "l");
			legends.at(indx).AddEntry(&MCgluon.at(indx),(DataID+"_MC gluon").c_str(), "l");
			
	
			data.at(indx).SetLineColor(kBlack);
			data.at(indx).SetLineWidth(2);
			MCquark.at(indx).SetLineColor(kBlue);
			MCquark.at(indx).SetLineWidth(1);
			MCgluon.at(indx).SetLineColor(kRed);
			MCgluon.at(indx).SetLineWidth(1);

			stacks.at(indx).Add(&MCquark.at(indx));
			stacks.at(indx).Add(&MCgluon.at(indx));
			//the amount of quarks is much smaller so no need to compare quark maximum
			if (data.at(indx).GetMaximum()>=MCgluon.at(indx).GetMaximum()){
				data.at(indx).SetAxisRange(0, 1.1*data.at(indx).GetMaximum(), "Y");
			}
			else {
				data.at(indx).SetAxisRange(0, 1.1*MCgluon.at(indx).GetMaximum(), "Y");
			}
			Hist=(TH1F*)gPad->GetPrimitive((CentralName[z]+Method[k]+"_MCquark").c_str());
			data.at(indx).SetTitle(("Cut comparison for "+Method[k]+" method, cntr "+CentralVal[z]+";"+Method[k]+"_Cut_Value").c_str());
			data.at(indx).SetBins(Hist->GetNbinsX(),Hist->GetBinLowEdge(1),Hist->GetBinLowEdge(Hist->GetNbinsX())+Hist->GetBinWidth(Hist->GetNbinsX()));
			Hist->SetName("schwifty");
			data.at(indx).Draw();
			MCgluon.at(indx).Draw("same");
			MCquark.at(indx).Draw("same");
			legends.at(indx).Draw();
		}//end of centrality cycle, z
		if(OnCounter==0 && OnCount>1){
			cout<<"shtart pdf!"<<endl;
			C.SaveAs((outDir+"Compare.pdf(").c_str());
		}
		else if(OnCounter==OnCount-1 && OnCount>1){
			C.SaveAs((outDir+"Compare.pdf)").c_str());
			cout<<"Be proud of thy self!"<<endl;
		}
		else
			C.SaveAs((outDir+"Compare.pdf").c_str());
		C.Clear();
	}//end of method cycle, k

	data.clear();
	MCgluon.clear();
	MCquark.clear();
}					
