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
          int  AnalysisMuons(vector<LParticle> MuonsTrue, vector<LParticle> MuonsReco);
          int  AnalysisElectrons(vector<LParticle> ElectronsTrue, vector<LParticle> ElectronsReco);
          int  AnalysisPhotons(vector<LParticle> PhotonsTrue, vector<LParticle> PhotonsReco);

   vector <string> ntup;       // ntuple list
   TH1D *h_debug ;             // global debug file
   int   MaxEvents;            // max number of events

   int  MuPileup; // number of pileup (mu)
   static const int num_threads = 16; // number of threads 
   static const int nBins=34;      // number of energy bins 
   static const int nBatch=200000; // number of events in batches for training
   static const int nEpoch=250;    // max number of epochs 
   static const int nBinsNN=201;   // number of bins for resolution plots
   static const int MinEntries=20; // min nr of entries in pT for NN training (per bunch);
   static const int num_layers = 3;
   //number in hidden layer 1
   static  const int num_neurons_hidden_1=40;


   double DeltaR;  // parameter used to match true jets with reco
   double MSESTOP;  // when stop training..

   // number of divisions in eta and phi to reproduce spacial structure  
   static const int slices_etaphi=30;

   // NN structure for resolution 
   // pT, eta,phi(slices), mass
   // 5 inputs + 2*(slices_etaphi-1) 
   static const int num_kin=5; // 5 kinematic variables + grid 
   static const int num_input=num_kin+2*(slices_etaphi-1);
   static const int num_output=nBinsNN-1;

   // this is input and output for NN for efficiency
   // 5 inputs plus 2*(slices_etaphi-1)
   static const int num_input_eff=num_kin+2*(slices_etaphi-1);
   // number of outputs
   static const int num_output_eff=2;
   static const int num_layers_eff=3;
   static const int num_neurons_hidden_eff=20;

   // dR cone for b-tag and isolation
   float dRbtag;
   float dRisolation;
   float btag_frac;

   bool firstTime[nBins-1]; // if false, continue training;
   // if ANN is found, we will read the old one. 
   double eBins[nBins];
   // for correction
   vector<vector<float>> finput_jets;
   vector<vector<float>> foutput_jets;
   // for efficiency
   vector<vector<float>> finput_jets_eff;
   vector<vector<float>> foutput_jets_eff;

   // for muons 
   vector<vector<float>> finput_muons;
   vector<vector<float>> foutput_muons;
   // for efficiency
   vector<vector<float>> finput_muons_eff;
   vector<vector<float>> foutput_muons_eff;

   vector<vector<float>> finput_electrons;
   vector<vector<float>> foutput_electrons;
   // for efficiency
   vector<vector<float>> finput_electrons_eff;
   vector<vector<float>> foutput_electrons_eff;

   vector<vector<float>> finput_photons;
   vector<vector<float>> foutput_photons;
   // for efficiency
   vector<vector<float>> finput_photons_eff;
   vector<vector<float>> foutput_photons_eff;

   
   double initialRME[nBins-1];
   double finalRME[nBins-1];
   int    eventsJetBins[nBins-1]; 
   int    eventsMuonBins[nBins-1];
   int    eventsElectronBins[nBins-1];
   int    eventsPhotonBins[nBins-1];

   // correction net
   struct fann *ann1_jets[nBins-1];
   string ann1_jets_name[nBins-1];

   struct fann *ann2_jets[nBins-1];
   string ann2_jets_name[nBins-1];

   struct fann *ann3_jets[nBins-1];
   string ann3_jets_name[nBins-1];

   struct fann *ann4_jets[nBins-1];
   string ann4_jets_name[nBins-1];

   // feature net
   struct fann *ann5_jets[nBins-1];
   string ann5_jets_name[nBins-1];

   // muons
   struct fann *ann1_muons[nBins-1];
   string ann1_muons_name[nBins-1];

   struct fann *ann2_muons[nBins-1];
   string ann2_muons_name[nBins-1];

   struct fann *ann3_muons[nBins-1];
   string ann3_muons_name[nBins-1];

   struct fann *ann4_muons[nBins-1];
   string ann4_muons_name[nBins-1];

   // feature net
   struct fann *ann5_muons[nBins-1];
   string ann5_muons_name[nBins-1];


   // electrons 
   struct fann *ann1_electrons[nBins-1];
   string ann1_electrons_name[nBins-1];

   struct fann *ann2_electrons[nBins-1];
   string ann2_electrons_name[nBins-1];

   struct fann *ann3_electrons[nBins-1];
   string ann3_electrons_name[nBins-1];

   struct fann *ann4_electrons[nBins-1];
   string ann4_electrons_name[nBins-1];

   // feature net
   struct fann *ann5_electrons[nBins-1];
   string ann5_electrons_name[nBins-1];


  // photons
   struct fann *ann1_photons[nBins-1];
   string ann1_photons_name[nBins-1];

   struct fann *ann2_photons[nBins-1];
   string ann2_photons_name[nBins-1];

   struct fann *ann3_photons[nBins-1];
   string ann3_photons_name[nBins-1];

   struct fann *ann4_photons[nBins-1];
   string ann4_photons_name[nBins-1];

   // feature net
   struct fann *ann5_photons[nBins-1];
   string ann5_photons_name[nBins-1];





   // to deal with scale extensions
   float jet_escale;
   float jet_eshift;
   float jet_etascale;
   float jet_etashift;
   float jet_mscale;
   float jet_mshift;

   float em_escale;
   float em_eshift;
   float em_etascale;
   float em_etashift;

   TH1D *h_jetpt;
   TH1D *h_jetpt_truth;

   double minPT;
   double maxEta;
 
 
protected:

   TH1D *h_dR;

   TH1D *h_in1;
   TH1D *h_in2;
   TH1D *h_in3;
   TH1D *h_in4;
   TH1D *h_in5;

   TH1D *h_out1;
   TH1D *h_out2;
   TH1D *h_out3;
   TH1D *h_out4;
   TH1D *h_out5;
   TH1D *h_out6;

   TH1D *h_mu_in1;
   TH1D *h_mu_in2;
   TH1D *h_mu_in3;
   TH1D *h_mu_in4;

   TH1D *h_mu_out1;
   TH1D *h_mu_out2;
   TH1D *h_mu_out3;
   TH1D *h_mu_out4;
   TH1D *h_mu_out5;

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
