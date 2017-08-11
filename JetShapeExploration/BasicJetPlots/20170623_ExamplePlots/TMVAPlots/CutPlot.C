{	//*SignifPlots.pdf
	TCanvas C;

	string outDir="./PlotsAA/160AA";
	string inDir="../TMVAFactory/AAoutput/160AA";
	string DataID="AA";

	//string outDir="./PlotsPP/";
	//string inDir="../TMVAFactory/PPoutput/";
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

	std::map<string,Variable> VarMap;
	Variable temp;

	int VarOnCount=0;
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
	cout<<VarOnCount<<endl;
	//turning everything, except "All", off
	for(int i=1; i<VarNum; i++){
		if(VarMap[VariableName[i]].on){
			VarMap[VariableName[i]].on=0;
			VarOnCount--;
		}
	}

	cout<<VarOnCount<<endl;
	TH1F* Histo=new TH1F("blah","", 100, 0, 69);

	cout<<"Variable switch complete!"<<endl;
//-----------------------------------------------------------------
	std::vector<TH1F> curves;
	std::vector<TH1F> effS, effB, signif; 
	Histo= new TH1F("name", "", 100, 0, 1);
	
	for (int z=0; z<otherCatNum; z++){
		for(int k=0; k<MethodNum; k++){
			for (int i=0; i<VarNum; i++){
				Histo->SetName((CentralName[z]+VariableName[i]+"_MVA_"+Method[k]+"_trainingRejBvsS").c_str());
				curves.push_back(*Histo);

				Histo->SetName((CentralName[z]+"effB_"+Method[k]+VariableName[i]).c_str());
				effB.push_back(*Histo);

				Histo->SetName((CentralName[z]+"effS_"+Method[k]+VariableName[i]).c_str());
				effS.push_back(*Histo);

				Histo->SetName((CentralName[z]+"signif_"+Method[k]+VariableName[i]).c_str());
				signif.push_back(*Histo);
			}
		}
	}
	//just in case
	Histo->SetName("boi");

	//start of significance and efficiency pdf creation
	C.Divide(2,2);

	std::vector<TLegend> sigLegend, rocLegend;
	Legend=new TLegend(0.45, 0.9, 0.9, 0.75, "", "NDC");
	Legend->SetTextFont(42);
   	Legend->SetTextSize(0.031);
	for(int z=0; z<otherCatNum; z++){
		for (int i=0; i<VarNum; i++){
			for(int k=0; k<MethodNum; k++){
				sigLegend.push_back(*Legend);
				rocLegend.push_back(*Legend);
			}
		}
	}

	TText texty, textify;
	texty.SetNDC();
	texty.SetTextSize(0.05);
	textify.SetNDC();
	textify.SetTextSize(0.05);
	counter=-1;
	Float_t SaveMe;
	int VarCounter=-1;
	int indx;
	for (int z=c; z<otherCatNum; z++){

		indx=(z-c)*VarNum*MethodNum-1;
		for (int i=0; i<VarNum;i++){

			if(i>1){
				if(!VarMap[VariableName[i-1]].on)
					indx+=MethodNum;
			}
			if(!VarMap[VariableName[i]].on) continue;
			VarCounter++;
			
			input=new TFile((inDir+CentralName[z]+"TMVAOutput"+VariableName[i]+".root").c_str());
			for(int k=0; k<MethodNum;k++){
				indx++;
				if(!MethMap[Method[k]].on) continue;
				counter++;
			
				C.cd(1);
				Histo=(TH1F*)input->Get(("Method_BDT/"+Method[k]+"/MVA_"+Method[k]+"_trainingRejBvsS").c_str());
				Histo->SetName((CentralName[z]+VariableName[i]+"_MVA_"+Method[k]+"_trainingRejBvsS").c_str());
				Histo->SetLineWidth(2);
				Histo->SetLineColor(kBlue);
				Histo->SetTitle(("Centrality "+CentralVal[z]+"_"+Method[k]+"_"+VariableName[i]+" ROC").c_str());
				Histo->Draw();
				curves.at(indx)=*Histo;
				Histo->SetName("boya");

				rocLegend.at(indx).AddEntry(&curves.at(indx),("ROC of "+VariableName[i]).c_str(), "l");
				rocLegend.at(indx).Draw();
			
				C.cd(2);

				Histo=(TH1F*)input->Get(("Method_BDT/"+Method[k]+"/MVA_"+Method[k]+"_trainingEffS").c_str());
				Histo->SetName(("MVA_"+Method[k]+"_"+VariableName[i]+"_trainingEffS").c_str());
				Histo->SetLineColor(kBlue);
				Histo->SetLineWidth(2);
				Histo->SetTitle(("Centrality "+CentralVal[z]+"_"+Method[k]+"_"+VariableName[i]+" Sig and Bckg eff. ;"+Method[k]+" Cut value").c_str());
				effS.at(indx)=*Histo;			
	
				Histo=(TH1F*)input->Get(("Method_BDT/"+Method[k]+"/MVA_"+Method[k]+"_trainingEffB").c_str());
				Histo->SetName(("MVA_"+Method[k]+"_"+VariableName[i]+"_trainingEffB").c_str());
				Histo->SetLineColor(kRed);
				Histo->SetLineWidth(2);
				Histo->SetTitle("");
				effB.at(indx)=*Histo;
				Histo->SetName("depression");	
	
				effS.at(indx).Draw();	
				effB.at(indx).Draw("Same");		
				sigLegend.at(indx).AddEntry(&effS.at(indx), (VariableName[i]+" Signal eff.").c_str(), "l");	
				sigLegend.at(indx).AddEntry(&effB.at(indx), (VariableName[i]+" Background eff.").c_str(), "l");
				sigLegend.at(indx).Draw();


				C.cd(4);

				signif.at(indx).SetName(("MVA_"+Method[k]+"_"+VariableName[i]+"_Significance").c_str());
				signif.at(indx).SetBins(effS.at(indx).GetNbinsX(),effS.at(indx).GetBinLowEdge(1), effS.at(indx).GetBinLowEdge(effS.at(indx).GetNbinsX())+effS.at(indx).GetBinWidth(effS.at(indx).GetNbinsX()));// Nbins, lowedge[1], lowedge[last]+width[last]
			
				//the actual calculation of the signifcance
				for (int z=1; z<effS.at(indx).GetNbinsX()+1; z++){
					// SetContent(num, GetContent)
					if (effB.at(indx).GetBinContent(z)==0) // if no bckg
						signif.at(indx).SetBinContent(z,sqrt(effS.at(indx).GetBinContent(z))); // sqrt(S)
					else 
						signif.at(indx).SetBinContent(z,effS.at(indx).GetBinContent(z)/sqrt(effB.at(indx).GetBinContent(z)+effS.at(indx).GetBinContent(z))); // S/sqrt(S+B). Significance sometimes the definition is S/sqrt(S+B)
				}		

				signif.at(indx).SetTitle(("Centrality "+CentralVal[z]+"_"+Method[k]+" significance ; "+Method[k]+" Cut value").c_str());
				signif.at(indx).SetLineColor(kGreen);
				signif.at(indx).Draw("");

				Legend->AddEntry(&signif.at(indx), "Significance [S/sqrt(B+S)]", "l");
				//Legend->Draw();	
					
				C.cd(3);
				C.cd(3)->Clear();
				texty.SetText(0.1,0.7,Form("Maximum significance is %1.3f at the cut of %1.3f", signif.at(indx).GetMaximum(), signif.at(indx).GetBinCenter(signif.at(indx).GetMaximumBin())));
				texty.Draw();
				textify.SetText(0.1, 0.6,Form("Sig. eff. : %1.3f, Bckg rej: %1.3f",effS.at(indx).GetBinContent(signif.at(indx).GetMaximumBin()) ,1-effB.at(indx).GetBinContent(signif.at(indx).GetMaximumBin())));
				textify.Draw();

				C.cd(1);
				Histo->Reset();
				Histo->SetName("teh best histogram, that has ever lived");
				Histo->SetBins(effS.at(indx).GetNbinsX(),0,1);
				Histo->SetMarkerStyle(kFullCircle);
				Histo->SetMarkerSize(0.5);
				Histo->SetMarkerColor(kRed);
				//effB acts as a weight...noice
				for (int z=1; z<effS.at(indx).GetNbinsX()+1; z++){
					//skip if belongs to the same bin
					if(z>1)
						if(Histo->FindBin(effS.at(indx).GetBinContent(z))==Histo->FindBin(effS.at(indx).GetBinContent(z-1)))
							continue;
					//SetBinContent(FindBin(S), 1-B) 
					Histo->SetBinContent(Histo->FindBin(effS.at(indx).GetBinContent(z)),1-effB.at(indx).GetBinContent(z));
				}
				//Histo->Fill(effS.at(indx).GetBinContent(signif.at(indx).GetMaximumBin()), 1-effB.at(indx).GetBinContent(signif.at(indx).GetMaximumBin()));

				Histo->Draw("same hist p");
				if(counter==0 && (OnCount>1 || VarOnCount>1 || otherCatNum>1))
					C.SaveAs((outDir+"SignifPlots.pdf(").c_str());
				else if(counter==VarOnCount*OnCount*(otherCatNum-c)-1 && (OnCount>1 || VarOnCount>1 || otherCatNum>1) ){
					C.SaveAs((outDir+"SignifPlots.pdf)").c_str());
					cout<<" we the best music "<<endl;				
				}
				else 
					C.SaveAs((outDir+"SignifPlots.pdf").c_str());

				Legend->Clear();
				C.Update();				

			}// end of method cycle, k

			input->Close();
		}//end of Variable cycle, i
	}//end of centrality cycle, z

	curves.clear();
	sigLegend.clear();
	rocLegend.clear();
	effB.clear();
	effS.clear();
	signif.clear();
}
