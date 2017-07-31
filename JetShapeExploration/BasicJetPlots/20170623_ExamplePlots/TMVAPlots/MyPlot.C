{
	TCanvas C;
	

	TFile *myfile=new TFile("../TMVAFactory/TMVAOutputMyMoment.root");
	
	TTree *tree=(TTree*)myfile->Get("TestTree");
	
	TH1D *H1=new TH1D("mymoment", "", 300, 0,50);	
	
	std::vector<TH1D> Signal, SignalFill, Background, BackgroundFill;
	std::vector<TLegend> PrettyLegend;


	H1->SetName("Signal");
	H1->SetFillStyle(0);
	H1->SetLineColor(kBlue);
	H1->SetLineWidth(3);
	Signal.push_back(*H1);
	H1->SetLineWidth(1);
	H1->SetName("SignalFill");
	H1->SetFillColor(kBlue);
	H1->SetFillStyle(3290);
	SignalFill.push_back(*H1);
	H1->SetTitle(";Pt^2dR^(-1.5)");
	H1->SetName("Background");
	H1->SetFillStyle(0);
	H1->SetLineColor(kRed);
	H1->SetLineWidth(3);
	Background.push_back(*H1);	
	H1->SetName("BackgroundFill");
	H1->SetFillColor(kRed);
	H1->SetFillStyle(3200);
	H1->SetLineWidth(1);
	BackgroundFill.push_back(*H1);	

	tree->Draw("MyMoment>>Background", "classID == 1", "");		
	tree->Draw("MyMoment>>BackgroundFill", "classID == 1 && BDT_DAB>0", "same");
	tree->Draw("MyMoment>>Signal", "classID == 0", "same");
	tree->Draw("MyMoment>>SignalFill", "classID == 0 && BDT_DAB>0", "same");

	C.SaveAs("MyPlot.pdf");

}
