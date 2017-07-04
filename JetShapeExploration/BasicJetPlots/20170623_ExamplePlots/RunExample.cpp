#include <iostream>
#include <string>
#include <cstring>
using namespace std;

#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

#include "ProgressBar.h"

#include "BasicUtilities.h"
#include "Messenger.h"

int main(int argc, char *argv[])
{
   // Checking if input is good
   if(argc != 6)
   {
      cerr << "Usage: " << argv[0] << " Input Output Tag PTHatMin PTHatMax" << endl;
      return -1;
   }

   // Getting inputs
   string Input = argv[1];
   string Output = argv[2];
   string Tag = argv[3];
   double PTHatMin = atof(argv[4]);
   double PTHatMax = atof(argv[5]);

   // Is it data or MC?  Is it pp or PbPb or pPb?  etc.
   bool IsData = IsDataFromTag(Tag);
   bool IsPP = IsDataFromTag(Tag);

   // Prepare input
   TFile *InputFile = TFile::Open(Input.c_str());
   if(InputFile == NULL)
   {
      cerr << "Error!  Input file error!" << endl;
      return -1;
   }

   HiEventTreeMessenger MHiEvent(InputFile);
   JetTreeMessenger MJet(InputFile, "ak4PFJetAnalyzer/t");
   PFTreeMessenger MPF(InputFile, "pfcandAnalyzer/pfTree");
   SkimTreeMessenger MSkim(InputFile);
   TriggerTreeMessenger MHLT(InputFile);

   if(MHiEvent.Tree == NULL)
   {
      cerr << "Error!  HiEvent tree not found in the input file!" << endl;
      return -1;
   }

   // Prepare output
   TFile OutputFile(Output.c_str(), "RECREATE");

   TH1D HN("HN", "Raw event count", 1, 0, 1);
   TH1D HPTHat("HPTHat", ";PTHat;", 100, 0, 500);
   TH1D HPTHatSelected("HPTHatSelected", ";PTHat;", 100, 0, 500);
   TH1D HJetPT("HJetPT", ";Jet PT;", 100, 0, 500);
   TH1D HJetEta("HJetEta", "JetPT > 50;Jet Eta;", 100, -5, 5);
   TH1D HJetPhi("HJetPhi", "JetPT > 50;Jet Phi;", 100, -M_PI, M_PI);
   TH1D HJetEtaSmallPT("HJetEtaSmallPT", "JetPT < 50;Jet Eta;", 100, -5, 5);
   TH1D HJetPhiSmallPT("HJetPhiSmallPT", "JetPT < 50;Jet Phi;", 100, -M_PI, M_PI);
  
   TH2D HJetRawPTvsJetPT("HJetRawPTvsJetPT","RawPT;PT",100,0,500,100,0,500);
   TH1D HPFPT("HPFPT", ";PF PT", 100, 0, 60);
   TH1D HPFE("HPFE", ";PF E", 100, 0, 60);
   TH1D HPFEta("HPFEta" , ";PF Eta", 100, -5, 5);
   TH1D HPFPhi("HPFPhi", ";PF Phi", 100,-M_PI, M_PI);
   // Initiate necessary arrays and histograms 
   const int IDNum=4;
   const int VarNum=8;
   // Parameters for Count: Charged, Electron, Muon, Photon, Hadron
   const int Param[IDNum+1][3]= { {300, 0, 300}, {10, 0, 10}, {10, 0, 10}, {300, 0, 300}, { 60, 0 , 60} }; 
   // Bin Parameters for DeltaR, DelatEta,DeltaPhi distributions 
   const double JetParamX[3][3]={ {100, 0, 0.4}, {100, -0.4, 0.4}, {100, -0.4, 0.4} };
   string IDName[IDNum+1]= {"Charged","Electron", "Muon", "Photon", "Hadron"};
   string PartonName[4]={"All", "Light", "Heavy", "Gluon"};
   string VarName[VarNum+1]={"Count", "PT", "E", "Eta", "Phi", "CountInJet", "DeltaR", "DeltaEta", "DeltaPhi"};
   TH1D* HPTE=new TH1D("PTE", ";", 100, 0, 500);
   TH1D* HCount=new TH1D("Count", ";", 1, 0 , 1);
   TH1D* HEta=new TH1D("Eta", ";", 100, -5, 5);
   TH1D* HPhi=new TH1D("Phi", ";", 100, -M_PI, M_PI);
   TH2D* HJetDistrib= new TH2D("Jet", "", 1, 0, 1, 1, 0 , 1 );
   std::vector<TH1D> vPT, vEta, vPhi, vEnergy, vCount, vCountInJet, vPTPercent, vHadCountDRParton;
   std::vector<std::vector<TH2D> > vPTDist, vPTParton;
   std::vector<std::vector<TH1D> > vCountDist, vMoments;
   std::vector<TH2D> Temp2D;
   std::vector<TH1D> Temp1D;
   int IDCount[IDNum+1]={ 0, 0, 0, 0, 0};
   int indx;
   double DR,DEta,DPhi;
   double PercentDR=0.1, PTRatio[4]={0, 0, 0, 0};
   // moment parameters 
   double  x[4]={0, 1, 1.5, 2}, y[3]={0, 1, 2};
   string xVal[4]={"0", "1", "1.5", "2"};
   string yVal[3]={"0", "1", "2"};
   double PTFrac,DRFrac;
   double moment[3][4][3];
   for(int m=0;m<3;m++){
	   for (int i=0;i<4;i++){
	    	for (int k=0; k<3;k++){
			moment[m][i][k]=0;
		}  
	   }
   }

   // making histogram vectors
   for (int i=0;i<=IDNum;i++){
     HCount->SetName(("H"+IDName[i]+VarName[0]).c_str());
     HCount->SetTitle((";"+VarName[0]).c_str());
     HCount->SetBins(Param[i][0], Param[i][1], Param[i][2]);
     vCount.push_back(*HCount);
     
     HPTE->SetName(("H"+IDName[i]+VarName[1]).c_str());
     HPTE->SetTitle((";"+VarName[1]).c_str());
     vPT.push_back(*HPTE);

     HPTE->SetName(("H"+IDName[i]+VarName[2]).c_str());
     HPTE->SetTitle((";"+VarName[2]).c_str());
     vEnergy.push_back(*HPTE);

     HEta->SetName(("H"+IDName[i]+VarName[3]).c_str());
     HEta->SetTitle((";"+VarName[3]).c_str());
     vEta.push_back(*HEta);
     
     HPhi->SetName(("H"+IDName[i]+VarName[4]).c_str());
     HPhi->SetTitle((";"+VarName[4]).c_str());
     vPhi.push_back(*HPhi);

     HCount->SetName(("HJet"+IDName[i]+VarName[5]).c_str());
     HCount->SetTitle((";"+VarName[5]).c_str());
     HCount->SetBins(Param[i][0], Param[i][1], Param[i][2]);
     vCountInJet.push_back(*HCount);
   }

   // create 1D & 2D histograms for distributions DR, DEta, DPhi
   for (int k=1; k<4;k++){
	HCount->SetName(("HJetHadronCountDR"+PartonName[k]).c_str());
	HCount->SetTitle(";DeltaR");
	HCount->SetBins(100, 0, 0.4);
	vHadCountDRParton.push_back(*HCount);
       for (int i=0; i<=IDNum;i++){
	 HJetDistrib->SetName(("HJet"+IDName[i]+"PTvs"+VarName[5+k]).c_str());
	 HJetDistrib->SetTitle(("PT;"+VarName[5+k]).c_str());
	 HJetDistrib->SetBins(JetParamX[k-1][0],JetParamX[k-1][1],JetParamX[k-1][2], 100, 0, 500 );
	 Temp2D.push_back(*HJetDistrib);
	 
	 HCount->SetName(("HJet"+IDName[i]+"Countvs"+VarName[5+k]).c_str());
	 HCount->SetTitle((";"+VarName[5+k]).c_str());
	 HCount->SetBins(JetParamX[k-1][0],JetParamX[k-1][1], JetParamX[k-1][2]);
	 Temp1D.push_back(*HCount);
       }
       vPTDist.push_back(Temp2D);
       vCountDist.push_back(Temp1D);
       Temp1D.clear();
       Temp2D.clear();
   }
   // quark and gluon separetd PT(DR) disrtibution
   HJetDistrib->SetBins(80, 0, 0.4, 50, 0 ,7);
   for (int i=1;i<4;i++){
	for (int k=0;k<=IDNum;k++){
		HJetDistrib->SetName(("HJet"+IDName[k]+"PT"+PartonName[i]).c_str());
		HJetDistrib->SetTitle("PT;DeltaR");
		Temp2D.push_back(*HJetDistrib);
	}
	vPTParton.push_back(Temp2D);
	Temp2D.clear();
   }
   // percentage in DR<0.1 vector creation
   HCount->SetBins(100, 0 , 1);
   for (int i=0;i<4;i++){
	HCount->SetName(("HJetPTPercent"+PartonName[i]).c_str());
	HCount->SetTitle(";PT ratio");
	vPTPercent.push_back(*HCount);
   }
   // Moment vector creation
   for (int i=1; i<4;i++){
	for (int k=0; k<4;k++){
		for (int j=0;j<3;j++){
			if(k==0 && j==0)
			HCount->SetBins(100, 0, 100);
			else if (k==0 && j>0)
			HCount->SetBins(100, 0, 25);
			else if((k==2 && j>0) || (k==1 && j==2))
			HCount->SetBins(100, 0 , 0.5);
			else if(k==1 && j==1)
			HCount->SetBins(100, 0, 0.6);
			else if (k==1 && j==0)
			HCount->SetBins(100, 0.85, 1 );
			else if(k==3 && j>0)
			HCount->SetBins(100, 0, 0.09);
			else 
			HCount->SetBins(100, 0, 1);
		
			HCount->SetName(("HMoment"+PartonName[i]+"_x="+xVal[k]+"_y="+yVal[j]).c_str());
			HCount->SetTitle(";");
			Temp1D.push_back(*HCount);
		}
	}
	vMoments.push_back(Temp1D);
	Temp1D.clear();
   }



   // Loop over events
   int EntryCount = MHiEvent.Tree->GetEntries() * 0.01;

   ProgressBar Bar(cout, EntryCount);
   Bar.SetStyle(-1);
   for(int iE = 0; iE < EntryCount; iE++)
   {  
      // Progress bar :D
      Bar.Update(iE);
      Bar.PrintWithMod(200);

      // Get event from trees
      MHiEvent.GetEntry(iE);
      MJet.GetEntry(iE);
      MPF.GetEntry(iE);
      MSkim.GetEntry(iE);
      MHLT.GetEntry(iE);

      int used[MPF.ID->size()];
      for (int u=0; u<MPF.ID->size();u++){
	used[u]=0;
      } 	

      // Fill histograms
      HN.Fill(0);
      HPTHat.Fill(MJet.PTHat);
      for (int iJ=0; iJ<MJet.JetCount; iJ++){
	HJetRawPTvsJetPT.Fill(MJet.JetRawPT[iJ],MJet.JetPT[iJ]);
      }

      if(MJet.PTHat < PTHatMin || MJet.PTHat >= PTHatMax)
         continue;

      HPTHatSelected.Fill(MJet.PTHat);

      for(int iJ = 0; iJ < MJet.JetCount; iJ++)
      {
	 
         HJetPT.Fill(MJet.JetPT[iJ]);
         if(MJet.JetPT[iJ] > 50)
         {
            HJetEta.Fill(MJet.JetEta[iJ]);
            HJetPhi.Fill(MJet.JetPhi[iJ]);
         }
         else
         {
            HJetEtaSmallPT.Fill(MJet.JetEta[iJ]);
            HJetPhiSmallPT.Fill(MJet.JetPhi[iJ]);
         }
	 //Jet selection and histogram filling
	 if (MJet.JetPT[iJ]<=100 || abs(MJet.JetEta[iJ])>=1)
	   continue;
	 for (unsigned int i=0; i!=MPF.ID->size(); i++){
	   DR=GetDR(MPF.Eta->at(i), MPF.Phi->at(i), MJet.JetEta[iJ], MJet.JetPhi[iJ]);
	   if (DR>=0.4 || used[i]==1)
	     continue;
	   if (MPF.ID->at(i)>0 && MPF.ID->at(i)<6){
		   used[i]=1;
		   DEta=MJet.JetEta[iJ]-MPF.Eta->at(i);
		   DPhi=MJet.JetPhi[iJ]-MPF.Phi->at(i);
		   if (DPhi<-M_PI) DPhi=DPhi+2*M_PI;
		   else if (DPhi>M_PI) DPhi=DPhi-2*M_PI;
		   indx=MPF.ID->at(i)-1;
		   PTFrac=MPF.PT->at(i)/MJet.JetPT[iJ];
		   DRFrac=DR/0.4;		   
			
		   if (DR<PercentDR){
			PTRatio[0]=PTRatio[0]+PTFrac;
		   }
		   if (MJet.RefPartonFlavor[iJ]==1 || MJet.RefPartonFlavor[iJ]==2 || MJet.RefPartonFlavor[iJ]==3){
		   	vPTParton.at(0).at(indx).Fill(DR,MPF.PT->at(i));
			if (indx==4){
	  	   		vHadCountDRParton.at(0).Fill(DR);
		      	}
			if (DR<PercentDR){
				PTRatio[1]=PTRatio[1]+PTFrac;
			}
			for (int j=0;j<4;j++){
				for (int m=0;m<3;m++){
					moment[0][j][m]+=pow(PTFrac,x[j])*pow(DRFrac,y[m]);
				}
			}
			
		   }	
		   else if (MJet.RefPartonFlavor[iJ]==4 || MJet.RefPartonFlavor[iJ]==5){
			vPTParton.at(1).at(indx).Fill(DR,MPF.PT->at(i));
			if (indx==4){
	  	   		vHadCountDRParton.at(1).Fill(DR);
		      	}
			if (DR<PercentDR){
				PTRatio[2]=PTRatio[2]+PTFrac;
		   	}
			for (int j=0;j<4;j++){
				for (int m=0;m<3;m++){
					moment[1][j][m]+=pow(PTFrac,x[j])*pow(DRFrac,y[m]);
				}
			}
		   }
		   else if (MJet.RefPartonFlavor[iJ]==21){
			vPTParton.at(2).at(indx).Fill(DR,MPF.PT->at(i));
			if (indx==4){
	  	   		vHadCountDRParton.at(2).Fill(DR);
		      	}
			if (DR<PercentDR){
				PTRatio[3]=PTRatio[3]+PTFrac;
			}
			for (int j=0;j<4;j++){
				for (int m=0;m<3;m++){
					moment[2][j][m]+=pow(PTFrac,x[j])*pow(DRFrac,y[m]);
				}
			}
		   }
		   IDCount[indx]++;
		   vPTDist.at(0).at(indx).Fill(DR,MPF.PT->at(i));
		   vPTDist.at(1).at(indx).Fill(DEta,MPF.PT->at(i));
		   vPTDist.at(2).at(indx).Fill(DPhi,MPF.PT->at(i));
		     
		   vCountDist.at(0).at(indx).Fill(DR);
		   vCountDist.at(1).at(indx).Fill(DEta);
		   vCountDist.at(2).at(indx).Fill(DPhi);
	   } //if ID
	 }// for cycle i
	 for(int k=0;k<4;k++){
		if (PTRatio[k]!=0)
	 	vPTPercent.at(k).Fill(PTRatio[k]);
	 	PTRatio[k]=0;
	 }
	 for (int k=0;k<=IDNum;k++){
	   	if (IDCount[k]!=0)
 	   	vCountInJet.at(k).Fill(IDCount[k]);
	   	IDCount[k]=0;
     	 }
	 
	for (int k=0;k<3;k++){
		for (int m=0; m<4;m++){
			for(int n=0;n<3;n++){
				if (moment[k][m][n]!=0)
				vMoments.at(k).at(m*3+n).Fill(moment[k][m][n]);
				moment[k][m][n]=0;
			} 
		}
	}	
      }// Jet cycle
     
      for(unsigned int i=0; i!=MPF.ID->size(); i++){
	HPFPT.Fill(MPF.PT->at(i));
	HPFE.Fill(MPF.E->at(i));
	HPFEta.Fill(MPF.Eta->at(i));
	HPFPhi.Fill(MPF.Phi->at(i));
       
	
	if (MPF.ID->at(i)>0 && MPF.ID->at(i)<6){
	  indx=MPF.ID->at(i)-1;
	  IDCount[indx]++;
	  vPT.at(indx).Fill(MPF.PT->at(i));
	  vEnergy.at(indx).Fill(MPF.E->at(i));
	  vEta.at(indx).Fill(MPF.Eta->at(i));
	  vPhi.at(indx).Fill(MPF.Phi->at(i));
	}  

      }
      for (int k=0;k<=IDNum;k++){
 	if(IDCount[k]!=0)
	vCount.at(k).Fill(IDCount[k]);
	IDCount[k]=0;
      }
   }// event cycle

   Bar.Update(EntryCount);
   Bar.Print();
   Bar.PrintLine();

   // Write stuff out
   HN.Write();
   HPTHat.Write();
   HPTHatSelected.Write();

   HJetPT.Write();
   HJetEta.Write();
   HJetPhi.Write();
   HJetEtaSmallPT.Write();
   HJetPhiSmallPT.Write();
   HPFPT.Write();
   HPFE.Write();
   HPFEta.Write();
   HPFPhi.Write();

   for (int k=0;k<=IDNum;k++){
     vCount.at(k).Write();
     vPT.at(k).Write();
     vEnergy.at(k).Write();
     vEta.at(k).Write();
     vPhi.at(k).Write();
     vCountInJet.at(k).Write();
     for (int j=0;j<3;j++){
       vPTDist.at(j).at(k).Write();
       vCountDist.at(j).at(k).Write();
     }
   }

  for (int i=0;i<3;i++){
	for (int k=0;k<=IDNum;k++){
		vPTParton.at(i).at(k).Write();
	}
   }

  for(int i=0;i<4;i++){
	vPTPercent.at(i).Write();
	if(i<3)
	vHadCountDRParton.at(i).Write();
  }

  for (int i=0;i<3;i++){
	for (int k=0;k<12;k++){
		vMoments.at(i).at(k).Write();
	}
  }

       // Cleanup
   InputFile->Close();

   return 0;
}






