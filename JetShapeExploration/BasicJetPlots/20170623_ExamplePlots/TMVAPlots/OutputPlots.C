{
	//QuickPlot.pdf , Fasterplot.pdf
    gStyle->SetOptStat(0);

	string outDir="./PlotsAA/AA";
	string inDir="../TMVAFactory/AAoutput/AA"; 
	string DataID="AA"; //not dependant on centrality

//	string outDir="./PlotsPP/";
//	string inDir="../TMVAFactory/PPoutput/";
//	string DataID="PP";

    TCanvas C;

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

	int OnCount=0;
	const int MethodNum=2;		
	string Method[MethodNum]={"BDT_DAB", "BDT_Fisher"};
	string Bound="0"; //used as comp[j]+Bound
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
	float axis[VarNum][3]={ {100, 0, 1}, {100, 0, 25}, {100, 0, 1}, {100, 0, 0.6}, {100, 0, 0.6}, {100, 0, 100 }, {100, 0, 1}, {10, 0, 5}, {10, 0, 5}, {160, 0, 80}};
	string AxisName[VarNum]={"sum[Pt^1.5]", "sum[dR^2]", "sum[Pt], dR<0.1", "sum[Pt*dR^2]", "sum[Pt*dR]", "Multiplicity", "sum[Pt^2]", "<=dR<", "<=dEta<", "sum[Pt^2*dR^(-1.5)]"};
	string rings[10]={"", "0", "0.05", "0.1", "0.15", "0.2", "0.25", "0.3", "0.35", "0.4"}; //used with Hadron and HadronEta axis naming

	std::map<string,Variable> VarMap;
	Variable temp;
	for (int i=0;i<VarNum;i++){
		input=new TFile((inDir+CentralName[c]+"TMVAOutput"+VariableName[i]+".root").c_str());
		if(!((TTree*)input->Get("TestTree"))) temp.on=0;
		else {
			temp.on=1;	
			VarOnCount++;
		}
		temp.range[0]=axis[i][0];
		temp.range[1]=axis[i][1];
		temp.range[2]=axis[i][2];
		if (i==7 || i==8) temp.cycles=9;
		else temp.cycles=1;
		VarMap[VariableName[i]]=temp;
		input->Close();
	}
//----------------------------------------------------------------------------------------------

	
    TFile *File;//((inDir+"TMVAOutputAll.root").c_str());
    TTree *Tree=0;//(TTree *)File.Get("TestTree");

    TH1F *HROC=0;//=(TH1F*)File.Get("Method_BDT/BDT_DAB/MVA_BDT_DAB_trainingRejBvsS");
    TH2D *CorrS=0;//(TH2D*)File.Get("CorrelationMatrixS");
    TH2D *CorrB=0;//(TH2D*)File.Get("CorrelationMatrixB");

    std::vector<TH1F> ROC;
    std::vector<TH1D> Signal, SignalFill, Background, BackgroundFill;
    std::vector<TLegend> PrettyLegend;	

    TText theText[MethodNum*2*((VarNum-2)+8*2)];
 	for (int i=0;i<MethodNum*2*((VarNum-2)+8*2);i++){
    		theText[i].SetNDC();
    		theText[i].SetTextSize(0.03);
	}

     TLegend legendary(0.29, 0.9, 0.9, 0.65, "", "NDC");
	legendary.SetTextFont(42);
	legendary.SetTextSize(0.037);
	legendary.SetHeader("");

	TH1D H1("H1", "yay", 100, 0, 1);

	TLegend Legend(0.1, 0.7, 0.35, 0.25, "", "NDC");
	Legend.SetTextFont(42);
	Legend.SetTextSize(0.033);


	int a=0; // helps with cycles and naming rings

	for(int z=c; z<otherCatNum;z++){
	
		File=new TFile((inDir+CentralName[z]+"TMVAOutputAll.root").c_str());
		Tree=(TTree*)File->Get("TestTree");
		CorrS=(TH2D*)File->Get("CorrelationMatrixS");
		CorrB=(TH2D*)File->Get("CorrelationMatrixB");
	
		for (int i=0; i<MethodNum;i++){
			if (!MethMap[Method[i]].on) continue; 
			HROC=(TH1F*)File->Get(("Method_BDT/"+Method[i]+"/MVA_"+Method[i]+"_trainingRejBvsS").c_str());
			HROC->SetName(Method[i].c_str());
			HROC->SetLineColor(2+i);
			HROC->SetLineWidth(2);
			ROC.push_back(*HROC);
		}
	    	for (int i=0; i<ROC.size();i++){
			if(i==0){
				ROC.at(i).SetTitle(("ROC centrality of "+CentralVal[z]).c_str());
				ROC.at(i).Draw("");
			}
			else {
				ROC.at(i).SetTitle("");
				ROC.at(i).Draw("same");
			}
	    		Legend.AddEntry(&ROC.at(i),ROC.at(i).GetName(), "l");
		}
			
		Legend.Draw();
	 
		if(z==c){
			C.SaveAs((outDir+"FasterPlot.pdf(").c_str());
			Legend.Clear();
			C.Clear();
		}
		else {
			C.SaveAs((outDir+"FasterPlot.pdf").c_str());
			Legend.Clear();
			C.Clear();
		}
	    CorrS->GetXaxis()->SetLabelSize(0.03);
	    CorrS->GetYaxis()->SetLabelSize(0.03);
		CorrS->SetTitle(("Correlation matrix (S) for centrality of "+CentralVal[z]).c_str());
	    CorrS->Draw("COLZ text");

	    C.SaveAs((outDir+"FasterPlot.pdf").c_str());

	    C.Clear();
	  
		CorrB->GetXaxis()->SetLabelSize(0.03);
		CorrB->GetYaxis()->SetLabelSize(0.03);
		CorrB->SetTitle(("Correlation matrix (B) for centrality of "+CentralVal[z]).c_str());
		CorrB->Draw("COLZ text");
		if(z==otherCatNum-1)
			C.SaveAs((outDir+"FasterPlot.pdf)").c_str());
		else
			C.SaveAs((outDir+"FasterPlot.pdf").c_str());
		C.Clear();
		ROC.clear();
	}
	// end of ROC(k) curves
	//---------------------------------------
	double S, B, Ratio, CutS, CutB, CutRatio;
	a=0; //reusing a from before
	int b=0;
	int indx; //crazy stuff with that index....
	int MethCounter=-1, VarCounter=-1;

	C.Clear();
	C.Divide(2,1);

	for(int z=c; z<otherCatNum; z++){
		File=new TFile((inDir+CentralName[z]+"TMVAOutputAll.root").c_str());
		Tree=(TTree*)File->Get("TestTree");

		for (int k=0; k<MethodNum; k++){

			for(int i=0; i<VarNum; i++){

				if (VarMap[VariableName[i]].cycles>1) a=1;
				else a=0;

				for (int m=a; m<VarMap[VariableName[i]].cycles;m++){

					H1.SetTitle((";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					H1.SetBins(VarMap[VariableName[i]].range[0], VarMap[VariableName[i]].range[1], VarMap[VariableName[i]].range[2]);
	
					for(int j=0; j<2;j++){
						PrettyLegend.push_back(legendary);
						H1.SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+Form("_Signal%i",j)).c_str());
						H1.SetFillStyle(0);
						H1.SetLineColor(kBlue);
						Signal.push_back(H1);

						H1.SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+Form("_SignalFill%i",j)).c_str());
						H1.SetFillColor(kBlue);
						H1.SetFillStyle(3290);
						SignalFill.push_back(H1);

						H1.SetTitle((Method[k]+" application for "+VariableName[i]+Numbers[m]+", centr "+CentralVal[z]+";"+rings[m]+AxisName[i]+rings[m+a]).c_str());//because this is the largest histogram it will be the first drawn histogram and provide the title
						H1.SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+Form("_Background%i",j)).c_str());
						H1.SetFillStyle(0);
						H1.SetLineColor(kRed);
						Background.push_back(H1);
	
						H1.SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+Form("_BackgroundFill%i",j)).c_str());
						H1.SetFillColor(kRed);
						H1.SetFillStyle(3200);
						BackgroundFill.push_back(H1);		
									
					}//end of <> cycle, j
				}//end of rings cycle, m
			}//end of variable cycle, i
		}//end of methood cycle, k
		//that last vector, last histogram integral thing
		H1.SetName("stranger things");
		//end of vector creation
		C.Divide(2,1);
		MethCounter=-1;
		//actual plotting and drawing
		for (int k=0; k<MethodNum;k++){
			VarCounter=-1;

			indx=k*2*((VarNum-2)+8*2)-1;

			if (!MethMap[Method[k]].on) continue;
			MethCounter++;

			for(int i=0; i<VarNum;i++){


				if (VarMap[VariableName[i]].cycles>1) a=1;
				else a=0;
				b=0;
				if (i>0){
					if (VarMap[VariableName[i-1]].cycles>1) b=1; //checks what was the number of cycles the last time;
					if (!VarMap[VariableName[i-1]].on) indx+=2*(b*7+1);			
				}	

				if (!VarMap[VariableName[i]].on) continue;
				VarCounter++;

				for (int m=a; m<VarMap[VariableName[i]].cycles; m++){

					for (int j=0;j<2;j++){
						indx++;
		
						C.cd(j+1);
						Tree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+Form("_Background%i",j)).c_str(), "classID == 1", "");				
						Tree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+Form("_BackgroundFill%i",j)).c_str(), ("classID == 1 && "+Method[k]+comp[j]+Bound).c_str(), "same");
						Tree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+Form("_Signal%i",j)).c_str(), "classID == 0", "same");
						Tree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+Form("_SignalFill%i",j)).c_str(), ("classID == 0 && "+Method[k]+comp[j]+Bound).c_str(), "same");
						PrettyLegend.at(indx).Clear();
						PrettyLegend.at(indx).AddEntry(&Background.at(indx), "ID = Gluon (1)", "l");
						PrettyLegend.at(indx).AddEntry(&BackgroundFill.at(indx), ("ID = Gluon (1), "+Method[k]+comp[j]+Bound).c_str(), "f");
						PrettyLegend.at(indx).AddEntry(&Signal.at(indx), "ID = Quark (0), ", "l");
						PrettyLegend.at(indx).AddEntry(&SignalFill.at(indx), ("ID = Quark (0), "+Method[k]+comp[j]+Bound).c_str(), "f");	
						PrettyLegend.at(indx).Draw();
						if (j==1){
							//Quarks as signal
							S=Signal.at(indx).Integral();
							B=Background.at(indx).Integral();
							Ratio=S/(S+B);	
							CutS=SignalFill.at(indx).Integral();
							CutB=BackgroundFill.at(indx).Integral();
							CutRatio=CutS/(CutS+CutB);		
						}
						else if (j==0){
							//Gluons as signal <==> background=>signal
							S=Background.at(indx).Integral();
							B=Signal.at(indx).Integral();
							Ratio=S/(S+B);	
							CutS=BackgroundFill.at(indx).Integral();
							CutB=SignalFill.at(indx).Integral();
							CutRatio=CutS/(CutS+CutB);
						}
						theText[indx].SetText(0.25, 0.91, Form("purity before cut= %1.3f , purity after cut= %1.3f", Ratio, CutRatio));
						theText[indx].Draw();
						//C.Update();
					}//end of <> cycle, j;
					if (VarCounter==0 && MethCounter==0 && (VarOnCount>1 || OnCount>1) && m==a) C.SaveAs((outDir+CentralName[z]+"QuickPlot.pdf(").c_str());
					else if(MethCounter==OnCount-1 && VarCounter==VarOnCount-1 && VarMap[VariableName[i]].cycles==m+1) C.SaveAs((outDir+CentralName[z]+"QuickPlot.pdf)").c_str());
					else C.SaveAs((outDir+CentralName[z]+"QuickPlot.pdf").c_str());
				}//end of rings cycle, m
			}// end of variable cycle, i
		}//end of method cycle, k
		PrettyLegend.clear();
		Background.clear();
		BackgroundFill.clear();
		Signal.clear();
		SignalFill.clear();
		C.Clear();
	}
}
