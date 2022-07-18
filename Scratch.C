#include<TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TVector3.h>

void Scratch()
{
	TFile *f1 = new TFile("NoQuenching.root");
	TFile *f2 = new TFile("Quenching.root");

	TTree *t1 = (TTree *)f1->Get("Correlator");
	TTree *t2 = (TTree *)f2->Get("Correlator");

	int n1tracks, n2tracks;
	float pt110, pt210; 

	t1->SetBranchAddress("NTracks", &n1tracks);
	t1->SetBranchAddress("PT10", &pt110);

	t2->SetBranchAddress("NTracks", &n2tracks);
	t2->SetBranchAddress("PT10", &pt210);

	TH1D *pt1 = new TH1D("pt1", "pt1", 400, 0, 10);
	TH1D *pt2 = new TH1D("pt2", "pt2", 400, 0, 10);

	for (int i = 0; i < t1->GetEntries(); i++){
		t1->GetEntry(i);
		pt1->Fill(pt110/float(n1tracks));
	}

	for (int i = 0; i < t2->GetEntries(); i++){
		t2->GetEntry(i);
		pt2->Fill(pt210/float(n2tracks));
	}

	pt1->Scale(1./pt1->Integral());
	pt2->Scale(1./pt2->Integral());

	pt1->SetLineColor(kBlack);
	pt2->SetLineColor(kRed);

	// pt1->Draw("HIST L");
	// pt2->Draw("HIST L SAME");

	pt2->Divide(pt1);

	pt2->Draw("HIST L SAME");
}	