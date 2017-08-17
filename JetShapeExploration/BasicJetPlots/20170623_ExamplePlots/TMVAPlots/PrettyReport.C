
{	//not sure since when, but this thing does not produce PPDataReaderPlot.pdf;
	// uses stacking!
	//Reportplot.pdf
   TCanvas C;

   gStyle->SetOptStat(0);

	//string outDir="./PlotsPPFinal/F";
	//string inDir="../TMVAFactory/PPoutputFinal/F";
	//string DataName="PP";

	string reader="../TMVAReader/F160";

	string outDir="./PlotsAAFinal/160AAF";
	string inDir="../TMVAFactory/AAoutputFinal/160AAF";
	string DataName="PbPb";

	const int CatNum=5;
	int c=1; //will be used when cycling over centralities
	int otherCatNum=CatNum; //will be used when cycling oer centralities
	std::string CentralName[CatNum]={"", "010", "1030", "3050", "5080"};
	string CentralVal[CatNum]={"--", "<0.1", ">0.1 && <0.3", ">0.3 && <0.5", ">0.5 && <0.8"};
	// cycle switch for PP data	
	if(DataName=="PP"){
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
		if (i==7 || i==8) temp.cycles=9;
		else temp.cycles=1;
		temp.range[0]=axis[i][0];
		temp.range[1]=axis[i][1];
		temp.range[2]=axis[i][2];
		VarMap[VariableName[i]]=temp;
		input->Close();
	}
	delete input;
	// end of map making and such
	cout<<"Variables switched!"<<endl;
	//-------------------------------------------------
	TH1F *H1=new TH1F("yeah boii", "", 100, 0, 1);	
	H1->SetLineWidth(2);

	vector<TH1F> dataSample, dataLessCut, dataMoreCut;
	vector<TH1F> MCSample, MCLessCut, MCMoreCut;
	
	THStack *FatStacks=new THStack("FatStacks", "");
	vector<TH1F> smallerQ, smallerG, moreQ, moreG;
	vector<THStack> smaller, more;
	
	vector <TLegend> DataLegend, MCLegend, stackLegend1, stackLegend2;	

	TText DataText[MethodNum*((VarNum-2)+2*8)], MCText[MethodNum*((VarNum-2)+2*8)], MoreEffText[MethodNum*((VarNum-2)+2*8)], LessEffText[MethodNum*((VarNum-2)+2*8)];
	//ratio calculation
	float sample, LessCut, MoreCut, LessRatio, MoreRatio;	
	float Qeff, Geff, Qpart, Gpart;
	float Qerr, Gerr;	

	TLegend *Legend=new TLegend(0.37, 0.9, 0.9, 0.77, "", "NDC");
	Legend->SetTextFont(42);
	Legend->SetTextSize(0.035);			

	for(int z=c; z<otherCatNum; z++){
		TFile *dataFile=new TFile((reader+DataName+CentralName[z]+"TMVA.root").c_str());
		TFile *MCFile=new TFile((inDir+CentralName[z]+"TMVAOutputAll.root").c_str());

		TTree *DataTree=(TTree*)dataFile->Get((DataName+"Tree").c_str());
		TTree *MCTree=(TTree*)MCFile->Get("TestTree");
		
		int a=0; // helps with cycles and naming "rings"

		for (int k=0;k<MethodNum;k++){

			for(int i=0; i<VarNum; i++){

				if (VarMap[VariableName[i]].cycles>1) a=1;
				else a=0;

				for (int m=a; m<VarMap[VariableName[i]].cycles;m++){

					H1->SetBins(VarMap[VariableName[i]].range[0], VarMap[VariableName[i]].range[1], VarMap[VariableName[i]].range[2]);
	
					DataLegend.push_back(*Legend);
					MCLegend.push_back(*Legend);
					stackLegend1.push_back(*Legend);
					stackLegend2.push_back(*Legend);
						
					H1->SetTitle((Method[k]+" application for "+VariableName[i]+Numbers[m]+", cntr "+CentralVal[z]+";"+rings[m]+AxisName[i]+rings[m+a]).c_str());

					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_data").c_str());
					H1->SetFillStyle(0);
					H1->SetLineColor(kBlack);
					dataSample.push_back(*H1);
	
					H1->SetTitle((";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_dataLessCut").c_str());
					H1->SetFillColor(kRed);
					H1->SetLineColor(kRed);
					H1->SetFillStyle(3205);
					dataLessCut.push_back(*H1);

					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_dataMoreCut").c_str());
					H1->SetFillColor(kBlue);
					H1->SetLineColor(kBlue);
					H1->SetFillStyle(3295);
					dataMoreCut.push_back(*H1);
					//----------------------------------------------------------
					H1->SetTitle((Method[k]+" application for "+VariableName[i]+Numbers[m]+", cntr "+CentralVal[z]+";"+rings[m]+AxisName[i]+rings[m+a]).c_str());

					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_MC").c_str());
					H1->SetFillStyle(0);
					H1->SetLineColor(kBlack);
					MCSample.push_back(*H1);

					H1->SetTitle((";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_MCLessCut").c_str());
					H1->SetFillColor(kRed);
					H1->SetLineColor(kRed);
					H1->SetFillStyle(3205);
					MCLessCut.push_back(*H1);

					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_MCMoreCut").c_str());
					H1->SetFillColor(kBlue);
					H1->SetLineColor(kBlue);
					H1->SetFillStyle(3295);
					MCMoreCut.push_back(*H1);

					//---------------------------------------------------------

					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_MoreCut_quark").c_str());
					H1->SetFillColor(kBlue);
					H1->SetLineColor(kBlue);
					H1->SetFillStyle(3295);
					moreQ.push_back(*H1);

					H1->SetTitle((";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_MoreCut_gluon").c_str());
					H1->SetFillColor(kRed);
					H1->SetLineColor(kRed);
					H1->SetFillStyle(3205);
					moreG.push_back(*H1);

					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_LessCut_quark").c_str());
					H1->SetFillColor(kBlue);
					H1->SetLineColor(kBlue);
					H1->SetFillStyle(3295);
					smallerQ.push_back(*H1);

					H1->SetTitle((";"+rings[m]+AxisName[i]+rings[m+a]).c_str());
					H1->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"_LessCut_gluon").c_str());
					H1->SetFillColor(kRed);
					H1->SetLineColor(kRed);
					H1->SetFillStyle(3205);
					smallerG.push_back(*H1);

					FatStacks->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"smaller").c_str());
					smaller.push_back(*FatStacks);

					FatStacks->SetName((Method[k]+"_"+VariableName[i]+Numbers[m]+"more").c_str());
					more.push_back(*FatStacks);
				
					//-------------------------------------------------
				}//end of rings cycle, m
			}//end of variable cycle, i
		}//end of methood cycle, k

		// that last vector, last histogram thing
		H1->SetName("boi");

		// teh plotting!!!!
	
		int b=0; //for that cycle thing
		int indx, OnCounter=-1, VarOnCounter=-1; // because not all of the methods may be turned on

		
		for (int k=0;k<MethodNum;k++){
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
					C.Divide(2,2);
					C.cd(3);			
				
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MC").c_str(), "", "");
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_LessCut_gluon").c_str(), ("classID == 1 && "+Method[k]+comp[0]+Bound).c_str(), "same");
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MoreCut_gluon").c_str(), ("classID == 1 && "+Method[k]+comp[1]+Bound).c_str(), "same");				

					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_LessCut_quark").c_str(), ("classID == 0 && "+Method[k]+comp[0]+Bound).c_str(), "same");
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MoreCut_quark").c_str(), ("classID == 0 && "+Method[k]+comp[1]+Bound).c_str(), "same");

					C.cd(3)->Clear();

					more.at(indx).Add(&moreQ.at(indx));
					more.at(indx).Add(&moreG.at(indx));
					stackLegend2.at(indx).AddEntry(&moreQ.at(indx),(Method[k]+comp[1]+Bound+", quarks").c_str(),"f");
					stackLegend2.at(indx).AddEntry(&moreG.at(indx),(Method[k]+comp[1]+Bound+", gluons").c_str(),"f");

					smaller.at(indx).Add(&smallerQ.at(indx));
					smaller.at(indx).Add(&smallerG.at(indx));

					stackLegend1.at(indx).AddEntry(&moreG.at(indx),(Method[k]+comp[0]+Bound+", gluons").c_str(),"f");
					stackLegend1.at(indx).AddEntry(&smallerQ.at(indx),(Method[k]+comp[0]+Bound+", quarks").c_str(),"f");

					//efficiency is actualy the sample purity after cuts		
					Qpart=smallerQ.at(indx).Integral();
					Gpart=smallerG.at(indx).Integral();				
					Geff=Gpart/(Gpart+Qpart);	

					Qpart=moreQ.at(indx).Integral();
					Gpart=moreG.at(indx).Integral();				
					Qeff=Qpart/(Gpart+Qpart);			

			
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MC").c_str(), "", "");			
					smaller.at(indx).Draw("same");		

					stackLegend1.at(indx).AddEntry(&MCSample.at(indx),(DataName+"_MC, no cuts").c_str(),"l");				
					stackLegend1.at(indx).Draw();	

					LessEffText[indx].SetNDC();
					LessEffText[indx].SetTextSize(0.03);
					LessEffText[indx].SetText(0.3, 0.91, Form("Gluon purity = %1.3f",Geff));
					LessEffText[indx].Draw();			
				
					C.cd(4);

					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MC").c_str(), "", "");
					more.at(indx).Draw("same");

					stackLegend2.at(indx).AddEntry(&MCSample.at(indx),(DataName+"_MC, no cuts").c_str(),"l");	
					stackLegend2.at(indx).Draw();		

					MoreEffText[indx].SetNDC();
					MoreEffText[indx].SetTextSize(0.03);
					MoreEffText[indx].SetText(0.3, 0.91, Form("Quark purity = %1.3f",Qeff));
					MoreEffText[indx].Draw();


					C.cd(1);
					DataTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_data").c_str(), "", "");
					DataTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_dataLessCut").c_str(), (Method[k]+comp[0]+Bound).c_str(), "same");
					DataTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_dataMoreCut").c_str(), (Method[k]+comp[1]+Bound).c_str(), "same");
					DataLegend.at(indx).AddEntry(&dataSample.at(indx), (DataName+"_data"+", no cuts").c_str(), "l");
					DataLegend.at(indx).AddEntry(&dataLessCut.at(indx), (DataName+"_data"+", "+Method[k]+comp[0]+Bound+" (G)").c_str(), "l");
					DataLegend.at(indx).AddEntry(&dataMoreCut.at(indx), (DataName+"_data"+", "+Method[k]+comp[1]+Bound+" (Q)").c_str(), "l");
			
					DataLegend.at(indx).Draw();
	
					sample=dataSample.at(indx).Integral();
					LessCut=dataLessCut.at(indx).Integral();
					MoreCut=dataMoreCut.at(indx).Integral();
					LessRatio=LessCut/sample;
					MoreRatio=MoreCut/sample;

					DataText[indx].SetNDC();
					DataText[indx].SetTextSize(0.03);
					DataText[indx].SetText(0.3, 0.91, Form("G/sample= %1.3f , Q/sample= %1.3f",LessRatio, MoreRatio));
					DataText[indx].Draw();

					C.cd(2);

					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MC").c_str(), "", "");
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MCLessCut").c_str(), (Method[k]+comp[0]+Bound).c_str(), "same");
					MCTree->Draw((VariableName[i]+Numbers[m]+">>"+Method[k]+"_"+VariableName[i]+Numbers[m]+"_MCMoreCut").c_str(), (Method[k]+comp[1]+Bound).c_str(), "same");
					MCLegend.at(indx).AddEntry(&MCSample.at(indx), (DataName+"_MC, no cuts").c_str(), "l");
					MCLegend.at(indx).AddEntry(&MCLessCut.at(indx), (DataName+"_MC, "+Method[k]+comp[0]+Bound+" (G)").c_str(), "l");
					MCLegend.at(indx).AddEntry(&MCMoreCut.at(indx), (DataName+"_MC, "+Method[k]+comp[1]+Bound+" (Q)").c_str(), "l");
		
					MCLegend.at(indx).Draw();

					sample=MCSample.at(indx).Integral();
					LessCut=MCLessCut.at(indx).Integral();
					MoreCut=MCMoreCut.at(indx).Integral(); //this is the strangest bug. The  last one gives zero...
					LessRatio=LessCut/sample;
					MoreRatio=MoreCut/sample;

					MCText[indx].SetNDC();
					MCText[indx].SetTextSize(0.03);
					MCText[indx].SetText(0.3, 0.91, Form("G/sample= %1.3f , Q/sample= %1.3f",LessRatio,MoreRatio));
					MCText[indx].Draw();
					C.Update();

					if (VarOnCounter==0 && OnCounter==0 && (VarOnCount>1 || OnCount>1) && m==a)
					C.SaveAs((outDir+CentralName[z]+"ReportPlot.pdf(").c_str());
					else if(OnCounter==OnCount-1 && VarOnCounter==VarOnCount-1 && VarMap[VariableName[i]].cycles==m+1)
					C.SaveAs((outDir+CentralName[z]+"ReportPlot.pdf)").c_str());
					else
					C.SaveAs((outDir+CentralName[z]+"ReportPlot.pdf").c_str());
					C.Clear();
				}//end of rings cycle, m
			}//end of variable cycle, i
		}//end of methood cycle, k

		MCLegend.clear();
		DataLegend.clear();
		stackLegend1.clear();
		stackLegend2.clear();

		dataSample.clear();
		dataLessCut.clear();
		dataMoreCut.clear();
		MCSample.clear();
		MCLessCut.clear();
		MCMoreCut.clear();

		smallerQ.clear();
		smallerG.clear();
		moreQ.clear();
		moreG.clear();
		smaller.clear();
		more.clear();
		
	}//end of centrality cycle,z;
}
