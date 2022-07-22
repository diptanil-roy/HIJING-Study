#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TVector3.h>
#include <algorithm>
#include <vector>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TProfile.h>
// Header file for the classes stored in the TTree if any.
#include "TObject.h"

struct corr{
	int ntracks;
	float pt10;
	float pt20;
	float pt30;
	float pt40;
};

double p11(corr c){
	return c.pt10/double(c.ntracks);
}
double p22(corr c){
	return c.pt20/double(c.ntracks);
}
double p33(corr c){
	return c.pt30/double(c.ntracks);
}
double p44(corr c){
	return c.pt40/double(c.ntracks);
}
double pbar11(corr c, double avpT){
	return p11(c) - avpT;
}
double pbar22(corr c, double avpT){
	return p22(c) - 2.*p11(c)*avpT + avpT*avpT;
}
double pbar33(corr c, double avpT){
	return p33(c) - 3.*p22(c)*avpT + 3*p11(c)*avpT*avpT - avpT*avpT*avpT;
}
double pbar44(corr c, double avpT){
	return p44(c) - 4*p33(c)*avpT + 6.*p22(c)*avpT*avpT - 4*p11(c)*avpT*avpT*avpT + avpT*avpT*avpT*avpT;
}
double tau(corr c, int order){
	return 1./pow(double(c.ntracks), order);
}
double c2(corr c, double avpT){
	return (pow(pbar11(c, avpT), 2) - pbar22(c, avpT))/(1. - tau(c, 1));
}
double c3(corr c, double avpT){
	return (pow(pbar11(c, avpT), 3) - 3.*pbar22(c, avpT)*pbar11(c, avpT) + 2*pbar33(c, avpT))/(1. - 3.*tau(c, 1) + 2.*tau(c, 2));
}
double c4(corr c, double avpT){
	return (pow(pbar11(c, avpT), 4) - 6.*pbar22(c, avpT)*pow(pbar11(c, avpT),2) + 3.*pow(pbar22(c, avpT), 2) + 8.*pbar33(c, avpT)*pbar11(c, avpT) - 6*pbar44(c, avpT))/(1. - 6.*tau(c, 1) + 3.*pow(tau(c, 1), 2) + 8.*tau(c, 2) - 6.*tau(c, 3));
}



vector<TH1D *> Method(TString filename = "NoQuenching.root"){

	TFile *f = new TFile(filename.Data());

	TTree *t = (TTree *)f->Get("Correlator");

	corr c = {0};

	t->SetBranchAddress("NTracks", &c.ntracks);
	t->SetBranchAddress("PT10", &c.pt10);
	t->SetBranchAddress("PT20", &c.pt20);
	t->SetBranchAddress("PT30", &c.pt30);
	t->SetBranchAddress("PT40", &c.pt40);

	int nevents = t->GetEntries();

	cout << nevents << endl;

	TProfile *ntracksvspt = new TProfile("NTracks vs Pt", "NTracks vs Pt", 450, -0.5, 449.5, 0, 10);

	for (int i = 0; i < nevents; i++){
		t->GetEntry(i);
		ntracksvspt->Fill(c.ntracks, c.pt10/c.ntracks);
	}

	ntracksvspt->Draw();

	double avpT = 0;

	TH1D *hC2 = new TH1D("hC2", "hC2", 400, -20, 20);
	TH1D *hC3 = new TH1D("hC3", "hC3", 400, -20, 20);
	TH1D *hC4 = new TH1D("hC4", "hC4", 400, -20, 20);

	for (int i = 0; i < nevents; i++){
		t->GetEntry(i);
		avpT = ntracksvspt->GetBinContent(ntracksvspt->FindBin(c.ntracks));
		double C2 = c2(c, avpT);
		double C3 = c3(c, avpT);
		double C4 = c4(c, avpT);

		// cout << C2 << "\t" << C3 << "\t" << C4 << endl;

		hC2->Fill(C2);
		hC3->Fill(C3);
		hC4->Fill(C4);

	}

	TH1D *h = (TH1D *)hC2->Clone();
	TH1D *k = (TH1D *)hC3->Clone();
	TH1D *l = (TH1D *)hC4->Clone();

	vector<TH1D *> v;
	v.push_back(h);
	v.push_back(k);
	v.push_back(l);

	delete hC2;
	delete hC3;
	delete hC4;
	t->Reset();

	return v;

}

void AnalyseCorr(){


	TH1::SetDefaultSumw2();
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
  
	vector<TH1D *> v1;
	v1 = Method("NoQuenching.root");

	/*
	
	vector<TH1D *> v2;
	v2 = Method("Quenching.root");

	const int numberofcases = 2;

	int colors[numberofcases] = {kBlack, kRed};
	int markerstyle[numberofcases] = {20, 20};

	TCanvas *Corr = new TCanvas("Corr", "Corr", 1600, 400);
	Corr->Divide(3);

	auto legend = new TLegend(0.5,0.7,0.9,0.9);
	legend->AddEntry(v1[0],"Unquenched","p");
	legend->AddEntry(v2[0],"Quenched","p");

	TString xaxistitle[3] = {"c_{2}", "c_{3}", "c_{4}"};

	for (int i = 0; i < v1.size(); i++){

		Corr->cd(i+1);
		gPad->SetLogy();

		v1[i]->SetLineColor(colors[0]);
		v1[i]->SetMarkerColor(colors[0]);
		v1[i]->SetMarkerStyle(markerstyle[0]);

		v1[i]->GetXaxis()->SetTitle(xaxistitle[i].Data());

		v2[i]->SetLineColor(colors[1]);
		v2[i]->SetMarkerColor(colors[1]);
		v2[i]->SetMarkerStyle(markerstyle[1]);

		v1[i]->Draw();
		v2[i]->Draw("SAME");
		legend->Draw("SAME");
	}

	*/
}
