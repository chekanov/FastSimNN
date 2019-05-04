// ANL analysis tutorial. S.Chekanov (ANL)

#ifndef Ana_h
#define Ana_h

#include<vector>
#include<iostream>
#include"TROOT.h"
#include<TSystem.h>
#include"stdio.h"
#include"TH1D.h"
#include"TH2D.h"
#include<map>
#include"TFile.h"
#include"TTree.h"
#include"TProfile.h"
#include<TClonesArray.h>
#include"TRandom3.h"
#include"TLorentzVector.h"
#include <string>
#include"SystemOfUnits.h"
#include "TMath.h"
#include "TRandom.h"
#include<fstream>
#include<stdlib.h>
#include "LParticle.h"

// Local include(s):
#include "fann.h"
#include "parallel_fann.h"
#include <libconfig.h++>

using namespace libconfig; 
using namespace std;

const double PI   = TMath::Pi();
const double PI2  = 2*PI;
const double PIover2   = 0.5*PI;


// main analysis class. inherent analysis.h which should be rebuild each time
class Ana  {
    public:
          virtual ~Ana();
          Ana();
          int nevv; 
          int  Finish();
          int  Init();

          int  AnalysisJets(vector<LParticle> JetsTrue, vector<LParticle> JetsReco);

   vector <string> ntup;       // ntuple list
   int  MuPileup; // number of pileup (mu)

   TH1D *h_debug ;             // global debug file
   int   MaxEvents;            // max number of events

   double DeltaR;  // parameter used to match true jets with reco

   double minPT;
   double maxEta;
   // dR cone for b-tag and isolation
   float dRbtag;
   float dRisolation;
   float btag_frac;

  // for correction
   vector<vector<float>> finput_jets;
   vector<vector<float>> foutput_jets;

   static const int nBatch=200000; // number of events in batches for training
 
 
protected:

   TH1D *h_dR;

   string ffile; 
   TFile *RootFile;
   TTree * m_ntuple;

   // objects for output ntuple 
   int RunNumber; // run number
   int EventNumber; // event number
   TClonesArray *a_jets;

   double mcEventWeight;

   bool isMC;
   int systematics;
   int type;
   double weight;

};

#endif
