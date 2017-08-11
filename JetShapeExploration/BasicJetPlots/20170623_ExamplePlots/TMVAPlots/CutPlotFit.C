{	
	gStyle->SetOptStat(0);

	TCanvas C;

	//string outDir="./PlotsAA/AA";
	//string inDir="../TMVAFactory/AAoutput/AA";
	//string DataID="PbPb";
	string reader="../TMVAReader/"; //DO NOT COMMENT THIS
	string outDir="./PlotsPP/";
	string inDir="../TMVAFactory/PPoutput/";
	string DataID="PP";

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
	
	cout<<"S3"<<endl;

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
	string rings[10]={"", "0", "0.01", "0.1", "0.15", "0.2", "0.25", "0.3", "0.35", "0.4"}; //used with Hadron and HadronEta axis naming

	int OnCount=0;
	const int MethodNum=2;		
	string Method[MethodNum]={"BDT_Fisher", "BDT_DAB"};
	string Bound="0";
	string comp[2]={"<", ">"};
	std::map<string,Meth> MethMap;
	Meth tmp;

	cout<<"Start of map switching!"<<endl;
	
	TFile *input;
	input=new TFile((inDir+CentralName[c]+"TMVAOutputAll.root").c_str());
	for (int k=0; k<MethodNum; k++){
		if(input->Get("TestTree")){
			if(((TTree*)input->Get("TestTree"))->FindBranch(Method[k].c_str())){
				tmp.on=1;
				OnCount++;
			}
		}
		else 
		tmp.on=0;
		MethMap[Method[k]]=tmp;
	}
	
	input->Close();
	cout<<"Start of variable switching!"<<endl;

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
	
	cout<<"Variable Switch done"<<endl;
//end of preparations. Variable switching and such
//-------------------------------------------------------------------
//start info gathering and actual plotting;
	const int CatNum2=(const int)(otherCatNum-c)*MethodNum;
	TFractionFitter *fracFit[8];
	TObjArray *tmpArr[8]; //stupid thing won't let make an array with CatNum2. Gotta be careful, when adding more methods or more centralities...other than that it should be fine
	Double_t val1[8], val2[8], err1[8],err2[8];
	std::vector<TH1F> data, MCquark, MCgluon, fitRes;
	TH1F *Hist=new TH1F("temp", "", 1,0,1);
	TFile *inputData; // "input" reused from before
	TTree *MCTree, *DataTree;	
	THStack *FatStack=new THStack("stacker", "");
	std::vector<THStack> stacks;
	std::vector<TObjArray> Array; 
	
	for(int j=0; j<CatNum2;j++)
		tmpArr[j]=new TObjArray(2);
	
	TLegend Legend(0.55, 0.9, 0.9, 0.71, "", "NDC");
	Legend.SetTextFont(42);
	Legend.SetTextSize(0.045);	
	std::vector<TLegend> legends;
	
	for(int k=0; k<MethodNum; k++){
		for(int z=c; z<otherCatNum; z++){
			Hist->SetName((CentralName[z]+"_"+Method[k]+"_data").c_str());
			data.push_back(*Hist);
			
			//Hist->SetLineWidth(2);
			Hist->SetName((CentralName[z]+Method[k]+"_MCgluon").c_str());
			MCgluon.push_back(*Hist);
			
			Hist->SetName((CentralName[z]+Method[k]+"_MCquark").c_str());
			MCquark.push_back(*Hist);

			Hist->SetName((CentralName[z]+Method[k]+"_fit").c_str());
			fitRes.push_back(*Hist);

			FatStack->SetName((CentralName[z]+Method[k]).c_str());
			stacks.push_back(*FatStack);

			legends.push_back(Legend);
		}//end of methood cycle, k
	}//end of centrality cycle, z
	C.Clear();
	Hist->SetName("empty");
	
	cout<<"Vectors pushed back"<<endl;
	//get histograms
	int indx;
	int OnCounter;
	C.Clear();
	int swop;

	OnCounter=-1;
	for(int k=0; k<MethodNum; k++){
		indx=k*(otherCatNum-c)-1;

		if (!MethMap[Method[k]].on) continue;
		OnCounter++;

		C.DivideSquare(otherCatNum-c); //1-0 or 5-1
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
			//stupid thing above won't fill vectors for some reason
			data.at(indx).SetBins(Hist->GetNbinsX(),Hist->GetBinLowEdge(1),Hist->GetBinLowEdge(Hist->GetNbinsX())+Hist->GetBinWidth(Hist->GetNbinsX()));
			data.at(indx)=*(TH1F*)gPad->GetPrimitive((CentralName[z]+"_"+Method[k]+"_data").c_str());
			MCquark.at(indx)=*(TH1F*)gPad->GetPrimitive((CentralName[z]+Method[k]+"_MCquark").c_str());
			MCgluon.at(indx)=*(TH1F*)gPad->GetPrimitive((CentralName[z]+Method[k]+"_MCgluon").c_str());	

			data.at(indx).Scale(1/data.at(indx).Integral());
			MCquark.at(indx).Scale(1/(MCquark.at(indx).Integral()+MCgluon.at(indx).Integral()));
			MCgluon.at(indx).Scale(1/(MCquark.at(indx).Integral()+MCgluon.at(indx).Integral()));

	
		}//end of centrality, z
	}//end of method cycle, k

	cout<<"Got Histograms"<<endl;
	for(int k=0; k<MethodNum; k++){
		indx=k*(otherCatNum-c)-1;
		for(int z=c; z<otherCatNum; z++){
			indx++;
			//area normalization
			MCquark.at(indx).Scale(1/MCquark.at(indx).Integral());
			MCgluon.at(indx).Scale(1/MCgluon.at(indx).Integral());
			tmpArr[indx]->Add(&MCquark.at(indx));
			tmpArr[indx]->Add(&MCgluon.at(indx));
			//Array.push_back(*tmpArr);
			//tmpArr->Remove(&MCquark.at(indx));
			//tmpArr->Remove(&MCgluon.at(indx));	
		}//end of methood cycle, k
	}//end of centrality cycle, z
	cout<<"Array pushed back "<<indx<<endl;

	indx=-1;
	for(int k=0; k<MethodNum; k++){
		for(int z=c; z<otherCatNum; z++){
			indx++;
			if(data.at(indx).GetEntries()!=0){
				fracFit[indx]=new TFractionFitter(&data.at(indx), tmpArr[indx]);
				fracFit[indx]->Constrain(0,0,10);
				fracFit[indx]->Constrain(1,0,10);			
			}
			else fracFit[indx]=0;
		}//end of methood cycle, k
	}//end of centrality cycle, z
	cout<<"Fitter array filled"<<endl;
	
	//Double_t val1,err1,val2,err2;
	TText text[8];
	//text.SetTextSize(0.04);
	//text.SetNDC();
	int fitStat;
	C.Clear();

	OnCounter=-1;
	for(int k=0; k<MethodNum; k++){
		indx=k*(otherCatNum-c)-1;
		if (!MethMap[Method[k]].on) continue;
		OnCounter++;
		C.DivideSquare(otherCatNum-c);
		for(int z=c; z<otherCatNum; z++){
			indx++;
			if(otherCatNum==1)
				swop=1;
			else swop=z;
			C.cd(swop);
			fitStat=fracFit[indx]->Fit();
			cout<<"FitStatus "<<fitStat<<endl;
				fitRes.at(indx)=*((TH1F*)fracFit[indx]->GetPlot());

				legends.at(indx).AddEntry(&data.at(indx), (DataID+"_data").c_str(), "l");
				legends.at(indx).AddEntry(&fitRes.at(indx), "Fit result(+err)", "f");
				legends.at(indx).AddEntry(&MCquark.at(indx), "quarks*c_1", "l");	
				legends.at(indx).AddEntry(&MCgluon.at(indx), "gluons*c_2", "l");
			
				fracFit[indx]->GetResult(0,val1[indx],err1[indx]);
				fracFit[indx]->GetResult(1,val2[indx],err2[indx]);

				MCgluon.at(indx).SetLineColor(kRed);
				MCgluon.at(indx).Scale(val2[indx]);
				MCgluon.at(indx).SetFillStyle(0);
				stacks.at(indx).Add(&MCgluon.at(indx));
				
				MCquark.at(indx).SetLineColor(kBlue);
				MCquark.at(indx).Scale(val1[indx]);
				MCquark.at(indx).SetFillStyle(0);
				stacks.at(indx).Add(&MCquark.at(indx));

				//drawing

				data.at(indx).SetLineColor(kBlack);
				data.at(indx).SetLineWidth(1);
				data.at(indx).SetTitle(("Cut comparison for "+Method[k]+" method, cntr "+CentralVal[z]+";"+Method[k]+"_Cut_Value").c_str());
				data.at(indx).SetAxisRange(0,1.2*data.at(indx).GetMaximum(), "Y");
				data.at(indx).Draw();

				stacks.at(indx).Draw("same");

				fitRes.at(indx).Scale(1/fitRes.at(indx).Integral());
				fitRes.at(indx).SetLineWidth(2);
				fitRes.at(indx).SetLineColor(8);
				fitRes.at(indx).SetFillColor(8); //somewhat green
				fitRes.at(indx).SetFillStyle(3200);
				fitRes.at(indx).Draw("same");

				data.at(indx).SetLineWidth(2);
				data.at(indx).Draw("same"); //to have it on top


				legends.at(indx).Draw();
				C.Update();

				text[indx].SetNDC();
				text[indx].SetTextSize(0.04);
				text[indx].SetText(0.15,0.91,Form("(quark)c_1=%1.3f+-%1.3f , (gluon)c_2=%1.3f+-%1.3f",val1[indx],err1[indx],val2[indx],err2[indx]));
				text[indx].Draw();
			/*
			else{
				text.SetTextSize(0.07);
				text.SetText(0.4,0.5,"DOESN'T FIT");
				text.Draw();
			}
			*/
		}//end of centrality cycle, z
		
		if(OnCounter==0 && OnCount>1){
			cout<<"shtart pdf!"<<endl;
			C.SaveAs((outDir+"FitCompare.pdf(").c_str());
		}
		else if(OnCounter==OnCount-1 && OnCount>1){
			C.SaveAs((outDir+"FitCompare.pdf)").c_str());
			cout<<"Here, have a croissant!"<<endl;
		}
		else
			C.SaveAs((outDir+"FitCompare.pdf").c_str());
		C.Clear();

	}//end of method cycle, k
	
	
	ofstream weightfit((outDir+"fitWeight.txt").c_str()); 
	
	for(int k=0; k<MethodNum; k++){
		indx=k*(otherCatNum-c)-1;
		for(int z=c; z<otherCatNum; z++){
			indx++;
			if( !MethMap[Method[k]].on){
				val1[indx]=1;
				val2[indx]=1;
			}
			weightfit<<val1[indx]<<endl<<val2[indx]<<endl;
		}
	}
	weightfit.close();	

	legends.clear();
	data.clear();
	MCgluon.clear();
	MCquark.clear();
	fitRes.clear();
	Array.clear();
}
