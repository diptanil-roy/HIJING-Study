#define Test_cxx
#include "Test.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TVector3.h>
#include <algorithm>
#include <vector>

void Test::Loop()
{

   TH1::SetDefaultSumw2();
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
//   In a ROOT session, you can do:
//      root> .L Test.C
//      root> Test t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   double nentries = fChain->GetEntries();

   Long64_t nbytes = 0, nb = 0;

   int ntracks = 0;
   float pt10 = 0;
   float pt20 = 0;
   float pt30 = 0;
   float pt40 = 0;


   TFile *f = new TFile("NoQuenching.root", "RECREATE");
   TTree *t = new TTree("Correlator", "Correlator");
   t->SetDirectory(f);

   t->Branch("NTracks", &ntracks, "ntracks/I");
   t->Branch("PT10", &pt10, "pt10/F");
   t->Branch("PT20", &pt20, "pt20/F");
   t->Branch("PT30", &pt30, "pt30/F");
   t->Branch("PT40", &pt40, "pt40/F");

   cout << nentries << endl;

   vector<int> allowedparticles;

   allowedparticles.push_back(211);
   allowedparticles.push_back(321);
   allowedparticles.push_back(2212);
   allowedparticles.push_back(-211);
   allowedparticles.push_back(-321);
   allowedparticles.push_back(-2212);
   allowedparticles.push_back(111);
   allowedparticles.push_back(221);
   // allowedparticles.push_back(130); //??
   allowedparticles.push_back(2112);
   allowedparticles.push_back(-2112);

   TProfile *ntracksvspt = new TProfile("NTracks vs Pt", "NTracks vs Pt", 1000, -0.5, 999.5);


   TDatabasePDG *pdg = new TDatabasePDG();

   // for (int jentry=0; jentry<nentries;jentry++) {
   for (int jentry=0; jentry<10000;jentry++) {
   	if (jentry % 10000 == 0) cout << double(jentry)/nentries*100 << "\% events of " << nentries << " completed." << endl;
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      ntracks = 0;
	   pt10 = 0;
	   pt20 = 0;
	   pt30 = 0;
	   pt40 = 0;

      for (int part = 0; part < mParticles_; part++){
      	if (mParticles_mStatus[part] <= 0) continue;
      	TVector3 p(mParticles_mPx[part], mParticles_mPy[part], mParticles_mPz[part]);
      	
      	double pt = p.Perp();
      	double eta = p.Eta();

         int idd = mParticles_mId[part];

         if(abs(idd)>3000) continue;

         if (pt < 0.2 || pt > 30.) continue;
         if (abs(eta) > 1.) continue;

         // if (find(allowedparticles.begin(), allowedparticles.end(), idd) != allowedparticles.end()) continue;
         if (abs(idd) != 211 && abs(idd) != 321 && abs(idd) != 2212 && abs(idd) != 2112 && abs(idd) != 111 && abs(idd) != 221 ) continue;

         // cout << idd << endl;

         TParticlePDG *particle = (TParticlePDG *)pdg->GetParticle(idd);

         double charge = particle->Charge();

         if (charge == 0) continue;

      	// cout << pt << endl;

      	ntracks++;

         if (pt < 0.2 || pt > 5) continue;
         
      	pt10+=pow(pt, 1);
      	pt20+=pow(pt, 2);
      	pt30+=pow(pt, 3);
      	pt40+=pow(pt, 4);
      }

      if (ntracks == 0) continue;
      // cout << ntracks << "\t" << pt10 << endl;
      t->Fill();

      ntracksvspt->Fill(ntracks, pt10/ntracks);
   }
   
   f->cd();
   t->Write();
   ntracksvspt->Write();
   f->Close();
}
