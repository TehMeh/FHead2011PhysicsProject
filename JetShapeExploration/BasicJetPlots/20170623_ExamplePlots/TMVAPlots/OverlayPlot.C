{	//ReportOverPlot.pdf
	//Map creation and such

	gStyle->SetOptStat(0);

	TCanvas C;

	string outDir="./PlotsAAFinal/160AAF"; // ./PlotsAApp/AApp  or ./PlotsAA/(160)AA
	string inDir="../TMVAFactory/AAoutputFinal/160AAF";
	string DataID="PbPb";
	string reader="../TMVAReader/F160"; //"../TMVAReader/PPweightAA/PPweight", "../TMVAReader/160"
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
	int UseWeight=0; // <<<<<<<<<<<<<<<<----------------------weight switch!!
	string weight[2]={"", "Wght"};
	Double_t val1[8], val2[8];


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

	float axis[VarNum][3]={ {100, 0, 1}, {100, 0, 35}, {100, 0, 1}, {100, 0, 0.6}, {100, 0, 0.6}, {100, 0, 100 }, {100, 0, 1}, {10, 0, 5}, {10, 0, 5}, {120, 0, 40}};
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
		

	// weights from fit
	
	if(UseWeight){
		ifstream iweight((outDir+"fitWeight.txt").c_str());
		for (int k=0; k<MethodNum; k++){
			for(int z=c; z<otherCatNum; z++){
				iweight>>val1[k*(otherCatNum-c)+(z-c)];
				iweight>>val2[k*(otherCatNum-c)+(z-c)];
				cout<<val1[k*(otherCatNum-c)+(z-c)]<<" weights"<<endl;
			}
		}	
		iweight.close();
	}
	else
	{
		for (int k=0; k<MethodNum; k++){
			for(int z=c; z<otherCatNum; z++){
				val1[k*(otherCatNum-c)+(z-c)]=1;
				val2[k*(otherCatNum-c)+(z-c)]=1;
			}
		}	
	}
	//------------------------------------------------------

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
	
	//MethMap["BDT_DAB"].on=0;
	//MethMap["BDT_Fisher"].on=0;
	//OnCount--;

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
	delete input;

	///*
	//VarMap["Hadron"].on=0;
	//VarOnCount--;
	//VarMap["HadronEta"].on=0;
	//VarOnCount--;
	//*/
	// end of map making and such
	cout<<"Maps and switching: OK!"<<endl;
	//------------------------------------------------------------

	TH1F *H1=new TH1F("yeah boii", "", 100, 0, 1);	
	H1->SetLineWidth(2);

	vector<TH1F> dataSample, dataLessCut, dataMoreCut; //classification on data
	vector<TH1F> MCSample, MCLessCut, MCMoreCut; // classification on MC
	vector<TH1F> allQ, allG; //number of Quarks or Gluons in the MC sample. Required for fraction
	
	vector<TH1F> smallerQ, smallerG, moreQ, moreG; // Gluons/Quarks that pass gluon(less)/quark(more) classification. Signal as signal and as bacground+ vice verca
	
	vector <TLegend> SampleLegend, LessLegend, MoreLegend;		

	TLegend *Legend=new TLegend(0.4, 0.9, 0.9, 0.73, "", "NDC");
	Legend->SetTextFont(42);
	Legend->SetTextSize(0.045);	

	// stuff for ratios
	//TLatex LabelText[MethodNum*((VarNum-2)+2*8)], PurText[MethodNum*((VarNum-2)+2*8)], FracText[MethodNum*((VarNum-2)+2*8)], EffText[MethodNum*((VarNum-2)+2*8)];
	TLatex LabelText, PurText, FracText, EffText;

	float totQ, totG, QclasQ, QclasG, GclasG, GclasQ; // total number of Q or G also signal as signal and bckg as bckg;
	float dataGpart, dataQpart, MCGpart, MCQpart;
	float dataQfrac, dataGfrac;
	float MCQfrac, MCGfrac, Qpur, Gpur, Qeff, Geff; //frac=classified/all ; pur=classified(signal)/classified; eff=classified(signal)/total(signal)
	float dataQfracerr, dataGfracerr; 
	float MCQfracerr, MCGfracerr, Gpurerr, Qpurerr, Qefferr, Gefferr; // corresponding errors
	float dataNorm, MCNorm; // size of the sample , norming factor
	float data, MC;
		
	int a=0; // helps with cycles and naming "rings"	
	int b=0; //for that cycle thing to keep track of indices (later)
	int indx, OnCounter=-1, VarOnCounter=-1; // because not all of the methods may be turned on
	int kount=0;
	int indx2; //for hist weighting
	//cycle for centralities/categories
	for (int z=c; z<otherCatNum; z++){

		TFile *dataFile=new TFile((reader+DataID+CentralName[z]+"TMVA.root").c_str());
		TFile *MCFile=new TFile((inDir+CentralName[z]+"TMVAOutputAll.root").c_str());
		TTree *DataTree=(TTree*)dataFile->Get((DataID+"Tree").c_str());
		TTree *MCTree=(TTree*)MCFile->Get("TestTree");	
		cout<<"Files: OK!"<<endl;
		for (int k=0; k<MethodNum; k++){

			for( int i=0; i<VarNum; i++){

				if (VarMap[VariableName[i]].cycles>1) a=1;
				else a=0;

				for (int m=a; m<VarMap[VariableName[i]].cycles;m++){

					H1->SetBins(VarMap[VariableName[i]].range[0], VarMap[VariableName[i]].range[1], VarMap[VariableName[i]].range[2]);
	
					SampleLegend.push_back(*Legend);
					LessLegend.push_back(*Legend);
					MoreLegend.push_back(*Legend);

					H1->SetTitle((";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_data").c_str());
					H1->SetFillStyle(0);
					H1->SetLineColor(kBlack);
					H1->SetLineStyle(1);
					dataSample.push_back(*H1);
	
					H1->SetTitle((";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_dataLessCut").c_str());
					H1->SetLineColor(kRed);
					dataLessCut.push_back(*H1);

					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_dataMoreCut").c_str());
					H1->SetLineColor(kBlue);
					dataMoreCut.push_back(*H1);
					//----------------------------------------------------------

					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_MC").c_str());
					H1->SetFillStyle(0);
					H1->SetLineColor(kBlack);
					H1->SetLineStyle(2);
					MCSample.push_back(*H1);

					H1->SetTitle((";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_MCLessCut").c_str());
					H1->SetLineColor(kRed);
					MCLessCut.push_back(*H1);

					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_MCMoreCut").c_str());
					H1->SetLineColor(kBlue);
					MCMoreCut.push_back(*H1);

					//---------------------------------------------------------
				
					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_MoreCut_quark").c_str());
					H1->SetLineColor(kBlue);
					H1->SetLineStyle(2);
					moreQ.push_back(*H1);

					H1->SetTitle((";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_MoreCut_gluon").c_str());
					H1->SetLineColor(kRed);
					moreG.push_back(*H1);

					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_LessCut_quark").c_str());
					H1->SetLineColor(kBlue);
					smallerQ.push_back(*H1);

					H1->SetTitle((";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_LessCut_gluon").c_str());
					H1->SetLineColor(kRed);
					smallerG.push_back(*H1);

					//---------------------------------------------------------					
					if (k==0) {
						H1->SetName((VariableName[i]+Numbers[m]+"_Quark").c_str());
						H1->SetLineColor(kBlue);
						allQ.push_back(*H1);				

						H1->SetName((VariableName[i]+Numbers[m]+"_Gluon").c_str());
						H1->SetLineColor(kRed);
						allG.push_back(*H1);
					}
				
					//---------------------------------------------------------
				}//end of rings cycle, m
			}//end of variable cycle, i
		}//end of methood cycle, k

		//If this is not included...the last histogram of the last vector gives zero integral. In this case smallerG is the last vector...strange indeed.
		H1->SetName("histogram");		
		cout<<"Push_Back: OK!"<<endl;
		// teh plotting!!!!

		C.Divide(2,2);

		OnCounter=-1;	
		for (int k=0;k<MethodNum;k++){
			indx2=k*(otherCatNum-c)+(z-c);

			if (!MethMap[Method[k]].on) continue;
			OnCounter++;
			VarOnCounter=-1;
			indx=k*((VarNum-2)+2*8)-1; //-1, 23, 47 etc.
			for(int i=0; i<VarNum; i++){
				if (VarMap[VariableName[i]].cycles>1) a=1;
				else a=0;

				b=0;
				if(i>0){
					if(VarMap[VariableName[i-1]].cycles>1) b=1;		
					if (!VarMap[VariableName[i-1]].on) indx+=b*7+1;		
				}
				if(!VarMap[VariableName[i]].on) continue;
			
				VarOnCounter++;

				for (int m=a; m<VarMap[VariableName[i]].cycles;m++){
					indx++;

					C.cd(3);			
				
					// histograms for purity before/after cut

					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_LessCut_gluon").c_str(), ("classID == 1 && "+Method[k]+comp[0]+Bound).c_str(), "");
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MoreCut_gluon").c_str(), ("classID == 1 && "+Method[k]+comp[1]+Bound).c_str(), "same");				

					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_LessCut_quark").c_str(), ("classID == 0 && "+Method[k]+comp[0]+Bound).c_str(), "same");
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MoreCut_quark").c_str(), ("classID == 0 && "+Method[k]+comp[1]+Bound).c_str(), "same");
			
					DataTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_data").c_str(), "", "");
					DataTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_dataLessCut").c_str(), (Method[k]+comp[0]+Bound).c_str(), "");
					DataTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_dataMoreCut").c_str(), (Method[k]+comp[1]+Bound).c_str(), "");

					/*
					//1 pad
					DataTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_data").c_str(), "", "");
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MC").c_str(), "", "");
					
					//2 pad
					DataTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_dataLessCut").c_str(), (Method[k]+comp[0]+Bound).c_str(), "");
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MCLessCut").c_str(), (Method[k]+comp[0]+Bound).c_str(), "same");

					//3 pad
					DataTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_dataMoreCut").c_str(), (Method[k]+comp[1]+Bound).c_str(), "");
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MCMoreCut").c_str(), (Method[k]+comp[1]+Bound).c_str(), "same");
					*/

					//fills pure Quark/Gluon histograms for classification efficiency (also for purity)
					if( OnCounter==0) {
						MCTree->Draw((VariableName[i]+Numbers[m]+">>"+VariableName[i]+Numbers[m]+"_Gluon").c_str(), "classID == 1", "");				

						MCTree->Draw((VariableName[i]+Numbers[m]+">>"+VariableName[i]+Numbers[m]+"_Quark").c_str(), "classID == 0", "same");
					}	
					C.cd(3)->Clear();
				
					// if the weights are used, then the histograms are pre-scaled and errors are not calculated, if not, then not pre-scaled(due to calculations of errors and such)
					if (UseWeight){
						moreG.at(indx).Scale(val2[indx2]/allG.at(indx%24).Integral());
						smallerG.at(indx).Scale(val2[indx2]/allG.at(indx%24).Integral());
						moreQ.at(indx).Scale(val1[indx2]/allQ.at(indx%24).Integral());
						smallerQ.at(indx).Scale(val1[indx2]/allQ.at(indx%24).Integral());
					}
					C.cd(1);
					
					
					dataNorm=1/dataSample.at(indx).Integral();
					dataSample.at(indx).Scale(dataNorm);				
					
					MCSample.at(indx)=moreG.at(indx)+moreQ.at(indx)+smallerQ.at(indx)+smallerG.at(indx);
					MCSample.at(indx).SetLineColor(kBlack);
					MCNorm=1/MCSample.at(indx).Integral();
					MCSample.at(indx).Scale(MCNorm);
				
					if (dataSample.at(indx).GetMaximum()>=MCSample.at(indx).GetMaximum())
						dataSample.at(indx).SetAxisRange(0, dataSample.at(indx).GetMaximum()+0.03, "Y");
					else 
						dataSample.at(indx).SetAxisRange(0, MCSample.at(indx).GetMaximum()+0.03, "Y");
			
					dataSample.at(indx).SetTitle((Method[k]+", samples, "+VariableName[i]+Numbers[m]+" , cntr "+CentralVal[z]+";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					dataSample.at(indx).Draw("");
					MCSample.at(indx).Draw("Same");	

					SampleLegend.at(indx).AddEntry(&MCSample.at(indx), (DataID+"_MC, no cuts").c_str(), "l");
					SampleLegend.at(indx).AddEntry(&dataSample.at(indx), (DataID+"_data, no cuts").c_str(), "l");
					SampleLegend.at(indx).Draw();

			
					C.cd(2)->Clear();
					dataLessCut.at(indx).Scale(dataNorm);
					MCLessCut.at(indx)=smallerQ.at(indx)+smallerG.at(indx);
					MCLessCut.at(indx).SetLineColor(kRed);
					MCLessCut.at(indx).Scale(MCNorm);
								
					dataLessCut.at(indx).SetTitle((Method[k]+comp[0]+Bound+", Gluon selection, "+VariableName[i]+Numbers[m]+" , cntr "+CentralVal[z]+";"+rings[m]+AxisName[i]+rings[m+a]).c_str());	
					if (dataLessCut.at(indx).GetMaximum()>=MCLessCut.at(indx).GetMaximum())
						dataLessCut.at(indx).SetAxisRange(0, dataLessCut.at(indx).GetMaximum()+0.01, "Y");
					else 
						dataLessCut.at(indx).SetAxisRange(0, MCLessCut.at(indx).GetMaximum()+0.01, "Y");

					dataLessCut.at(indx).Draw();
					MCLessCut.at(indx).Draw("same");

					LessLegend.at(indx).AddEntry(&MCLessCut.at(indx), (DataID+"_MC, "+Method[k]+comp[0]+Bound+" (G)").c_str(), "l");
					LessLegend.at(indx).AddEntry(&dataLessCut.at(indx), (DataID+"_data, "+Method[k]+comp[0]+Bound+" (G)").c_str(), "l");
					LessLegend.at(indx).Draw();

					C.cd(3);

					MCMoreCut.at(indx)=moreQ.at(indx)+moreG.at(indx);	
					MCMoreCut.at(indx).SetLineColor(kBlue);			
					MCMoreCut.at(indx).Scale(MCNorm);
					dataMoreCut.at(indx).Scale(dataNorm);

					if (dataMoreCut.at(indx).GetMaximum()>=MCMoreCut.at(indx).GetMaximum())
						dataMoreCut.at(indx).SetAxisRange(0, dataMoreCut.at(indx).GetMaximum()+0.01, "Y");
					else
						dataMoreCut.at(indx).SetAxisRange(0, MCMoreCut.at(indx).GetMaximum()+0.01, "Y");
			
					dataMoreCut.at(indx).SetTitle((Method[k]+comp[1]+Bound+", Quark selection, "+VariableName[i]+Numbers[m]+" , cntr "+CentralVal[z]+";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					dataMoreCut.at(indx).Draw();
					MCMoreCut.at(indx).Draw("same");

					MoreLegend.at(indx).AddEntry(&MCMoreCut.at(indx), (DataID+"_MC, "+Method[k]+comp[1]+Bound+" (Q)").c_str(), "l");
					MoreLegend.at(indx).AddEntry(&dataMoreCut.at(indx), (DataID+"_data, "+Method[k]+comp[1]+Bound+" (Q)").c_str(), "l");
					MoreLegend.at(indx).Draw();
					
					if(!UseWeight){
						C.cd(4);

						//Getting integrals
						data=dataSample.at(indx).Integral();
						MC=MCSample.at(indx).Integral();	
						dataQpart=dataMoreCut.at(indx).Integral();
						dataGpart=dataLessCut.at(indx).Integral();

						MCQpart=MCMoreCut.at(indx).Integral();
						MCGpart=MCLessCut.at(indx).Integral();
						if(OnCounter==0){
							allQ.at(indx%24).Scale(MCNorm);
							allG.at(indx%24).Scale(MCNorm);
						}
						totQ=allQ.at(indx%24).Integral();
						totG=allG.at(indx%24).Integral();


						smallerG.at(indx).Scale(MCNorm);
						smallerQ.at(indx).Scale(MCNorm);
						moreG.at(indx).Scale(MCNorm);
						moreQ.at(indx).Scale(MCNorm);

						GclasG=smallerG.at(indx).Integral();
						GclasQ=moreG.at(indx).Integral();
						QclasQ=moreQ.at(indx).Integral();
						QclasG=smallerQ.at(indx).Integral();			
				
						//calculating stuff
						dataQfrac=dataQpart/data;
						dataGfrac=dataGpart/data;
						MCQfrac=MCQpart/MC;
						MCGfrac=MCGpart/MC;

						Qpur=QclasQ/(MCQpart);
						Gpur=GclasG/(MCGpart);

						Qeff=QclasQ/totQ;
						Geff=GclasG/totG;

						//errors
						dataQfracerr=sqrt(dataQpart/pow(data,2))*sqrt(dataNorm);
						dataGfracerr=sqrt(dataGpart/pow(data,2))*sqrt(dataNorm);
						MCQfracerr=sqrt(MCQpart/pow(MC,2))*sqrt(MCNorm);
						MCGfracerr=sqrt(MCGpart/pow(MC,2))*sqrt(MCNorm);

						Qpurerr=sqrt(QclasQ*GclasQ/pow(QclasQ+GclasQ,3))*sqrt(MCNorm);
						Gpurerr=sqrt(QclasG*GclasG/pow(QclasG+GclasG,3))*sqrt(MCNorm);

						Gefferr=sqrt(GclasG/pow(totG,2)*(1+GclasG/totG))*sqrt(MCNorm); 

						Qefferr=sqrt(QclasQ/pow(totQ,2)*(1+QclasQ/totQ))*sqrt(MCNorm);
	
						//writing stuff out
						FracText.SetNDC();
						FracText.SetTextSize(0.05);
						FracText.DrawLatex(0.01, 0.85, Form("Fraction of data classified as Quarks: %1.3f #pm %1.3f",dataQfrac,dataQfracerr));
						FracText.DrawLatex(0.01, 0.79, Form("Fraction of MC classified as Quarks: %1.3f #pm %1.3f",MCQfrac, MCQfracerr));
					 	FracText.DrawLatex(0.01, 0.72, Form("Fraction of data classified as Gluons: %1.3f #pm %1.3f",dataGfrac,dataGfracerr));
						FracText.DrawLatex(0.01, 0.66, Form("Fraction of MC classified as Gluons: %1.3f #pm %1.3f",MCGfrac,MCGfracerr));

						PurText.SetNDC();
						PurText.SetTextSize(0.05);
						PurText.DrawLatex(0.01, 0.58, Form("Sample purity after Quark classification: %1.3f #pm %1.3f",Qpur,Qpurerr));
						PurText.DrawLatex(0.01, 0.52, Form("Sample purity after Gluon classification: %1.3f #pm %1.3f",Gpur,Gpurerr));

						EffText.SetNDC();
						EffText.SetTextSize(0.05);
						EffText.DrawLatex(0.01, 0.44, Form("Quark classification efficiency: %1.3f #pm %1.3f",Qeff,Qefferr));
						EffText.DrawLatex(0.01, 0.38, Form("Gluon classification efficiency: %1.3f #pm %1.3f",Geff,Gefferr));
					}//end of weight if
					//saving the pdf
					if (VarOnCounter==0 && OnCounter==0 && (VarOnCount>1 || OnCount>1) && m==a)
					C.SaveAs((outDir+CentralName[z]+weight[UseWeight]+"ReportOverPlot.pdf(").c_str());
					else if(OnCounter==OnCount-1 && VarOnCounter==VarOnCount-1 && VarMap[VariableName[i]].cycles==m+1)
					{
						C.SaveAs((outDir+CentralName[z]+weight[UseWeight]+"ReportOverPlot.pdf)").c_str());
						cout<<"Well done, you're so great!!!"<<endl;
					}
					else
					C.SaveAs((outDir+CentralName[z]+weight[UseWeight]+"ReportOverPlot.pdf").c_str());

					C.Update();
					C.cd(4)->Clear();
				}//end of rings cycle, m
			}//end of variable cycle, i
		}//end of methood cycle, k

		LessLegend.clear();
		SampleLegend.clear();
		MoreLegend.clear();
		dataSample.clear();
		dataLessCut.clear();
		dataMoreCut.clear();
		MCSample.clear();
		MCLessCut.clear();
		MCMoreCut.clear();
		allQ.clear();
		allG.clear();
		moreQ.clear();
		moreG.clear();
		smallerQ.clear();
		smallerG.clear();
		C.Clear();
	}//end of category cycle, z
}
