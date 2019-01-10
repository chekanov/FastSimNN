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
#include"TRandom2.h"
#include"TLorentzVector.h"
#include <string>
#include"SystemOfUnits.h"
#include "TMath.h"
#include "TRandom.h"
#include<fstream>
#include<stdlib.h>
#include "LParticle.h"
#include <stdlib.h>

// Local include(s):
#include "fann.h"
#include "parallel_fann.h"
#include <libconfig.h++>

//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/discrete_distribution.hpp>
//boost::mt19937 gen;

const double PI   = TMath::Pi();
const double PI2  = 2*PI;
const double PIover2   = 0.5*PI;


using namespace libconfig;
using namespace std;


// main analysis class. inherent analysis.h which should be rebuild each time
class Ana  {
    public:
          virtual ~Ana();
          Ana();
          int nev; 
	  int  Init();
          int  Finish();
          int  AnalysisJets(vector<LParticle> JetsTrue, vector<LParticle> JetsReco);
          int  AnalysisMuons(vector<LParticle> MuonsTrue, vector<LParticle> MuonsReco);
          int  AnalysisElectrons(vector<LParticle> ElectronsTrue, vector<LParticle> ElectronsReco);
          int  AnalysisPhotons(vector<LParticle> PhotonsTrue, vector<LParticle> PhotonsReco);
          // random
          int findCeil(int  arr[], int r, int l, int h);
          int myRand(int arr[], int freq[], int n);

   vector <string> ntup;       // ntuple list
   TH1D *h_debug ;             // global debug file
   int   MaxEvents;            // max number of events
   TH1D *h_dR;

   TTree* m_ntuple;

  // dR cone for b-tag and isolation
   float dRbtag;
   float dRisolation;
   float btag_frac;
 
  // NN structure 
   unsigned int num_layers = 3;
   int nBinsNN;   // number of bins for resolution plots

    // NN structure 
   unsigned int num_input;
   unsigned int num_output;
   unsigned int num_input_eff;
   unsigned int num_output_eff;
   unsigned int num_kin;

   int  MuPileup; 
   int nBins;
   double DeltaR;  // parameter used to match true jets with reco
   double  *eBins;
   double *initialRME;
   double *finalRME;
   // jets
   struct fann **ann1_jets; 
   struct fann **ann2_jets;
   struct fann **ann3_jets;
   struct fann **ann4_jets;
   struct fann **ann5_jets;

   string *ann1_jets_name;
   string *ann2_jets_name;
   string *ann3_jets_name;
   string *ann4_jets_name;
   string *ann5_jets_name;


   // muons
   struct fann **ann1_muons;
   struct fann **ann2_muons;
   struct fann **ann3_muons;
   struct fann **ann4_muons;
   struct fann **ann5_muons;

   string *ann1_muons_name;
   string *ann2_muons_name;
   string *ann3_muons_name;
   string *ann4_muons_name;
   string *ann5_muons_name;

   // electrons
   struct fann **ann1_electrons;
   struct fann **ann2_electrons;
   struct fann **ann3_electrons;
   struct fann **ann4_electrons;
   struct fann **ann5_electrons;
   
   string *ann1_electrons_name;
   string *ann2_electrons_name;
   string *ann3_electrons_name;
   string *ann4_electrons_name;
   string *ann5_electrons_name;


   struct fann **ann1_photons;
   struct fann **ann2_photons;
   struct fann **ann3_photons;
   struct fann **ann4_photons;
   struct fann **ann5_photons;

   string *ann1_photons_name;
   string *ann2_photons_name;
   string *ann3_photons_name;
   string *ann4_photons_name;
   string *ann5_photons_name;



   // to deal with scale extensions
   float jet_escale;
   float jet_eshift;
   float jet_etascale;
   float jet_etashift;
   float jet_mscale;
   float jet_mshift;
   int slices_etaphi;

   // EM objects
   float em_escale;
   float em_eshift;
   float em_etascale;
   float em_etashift;
   float EMminPT;
   float EMmaxEta;
   int   EMnBins; 

 
   int *BinOverTrue; // bins for reco/true distributions 

   TH1D *h_jetpt;
   TH1D *h_jetpt_truth;
   TH1D *h_jetpt_nn;
   TH1D *h_jetcutflow;

   TH1D *h_ptcor;
   TH1D *h_etacor; 
   TH1D *h_phicor;

   double minPT;
   double maxEta;
 
protected:

   string ffile; 
   TFile *RootFile;

  TH1D *h_jetres100;
  TH1D *h_jetres1000;

  TH1D *h_in1_jet;
  TH1D *h_in2_jet;
  TH1D *h_in3_jet;
  TH1D *h_in4_jet;
  TH1D *h_in5_jet;

   // mu
  TH1D *h_in1_mu;
  TH1D *h_in2_mu;
  TH1D *h_in3_mu;
  TH1D *h_in4_mu;


  TH1D *h_out1_jet;
  TH1D *h_out2_jet;
  TH1D *h_out3_jet;
  TH1D *h_out4_jet;

  TH1D *h_out1_mu;
  TH1D *h_out2_mu;
  TH1D *h_out3_mu;
  TH1D *h_out4_mu;


  TH1D *h_out5_jet_eff;
  TH1D *h_out6_jet_btag;
  TH1D *h_out5_mu_eff;
  TH1D *h_out5_ph_eff;
  TH1D *h_out5_el_eff;

   //random bins generated
  TH1D *h_rout1_jet;
  TH1D *h_rout2_jet;
  TH1D *h_rout3_jet;
  TH1D *h_rout4_jet;

  TH1D *h_rout1_mu;
  TH1D *h_rout2_mu;
  TH1D *h_rout3_mu;
  TH1D *h_rout4_mu;


   // reco jets
   std::vector<Double32_t> m_jetpt; //!
   std::vector<Double32_t> m_jeteta; //!
   std::vector<Double32_t> m_jetphi; //!
   std::vector<Double32_t> m_jetm; //!
   std::vector<Int_t>      m_jetbtag; //!

   // matched jets
   std::vector<Double32_t> m_matchedjetpt; //!
   std::vector<Double32_t> m_matchedjeteta; //!
   std::vector<Double32_t> m_matchedjetphi; //!
   std::vector<Double32_t> m_matchedjetm; //!
   std::vector<Int_t>      m_matchedjetbtag; //!

   // truth jets
   std::vector<Double32_t> m_gjetpt; //!
   std::vector<Double32_t> m_gjeteta; //!
   std::vector<Double32_t> m_gjetphi; //!
   std::vector<Double32_t> m_gjetm; //!
   std::vector<Int_t>      m_gjetbtag; //!

   // NN jets
   std::vector<Double32_t> m_nnjetpt; //!
   std::vector<Double32_t> m_nnjeteta; //!
   std::vector<Double32_t> m_nnjetphi; //!
   std::vector<Double32_t> m_nnjetm; //!
   std::vector<Int_t>      m_nnjetbtag; //!

   // reco muons
   std::vector<Double32_t> m_mupt; //!
   std::vector<Double32_t> m_mueta; //!
   std::vector<Double32_t> m_muphi; //!
   std::vector<Int_t>      m_mucharge; //!

   std::vector<Double32_t> m_gmupt; //!
   std::vector<Double32_t> m_gmueta; //!
   std::vector<Double32_t> m_gmuphi; //!
   std::vector<Int_t>      m_gmucharge; //!

   std::vector<Double32_t> m_nnmupt; //!
   std::vector<Double32_t> m_nnmueta; //!
   std::vector<Double32_t> m_nnmuphi; //!
   std::vector<Int_t>      m_nnmucharge; //!

   // elec
   std::vector<Double32_t> m_elpt; //!
   std::vector<Double32_t> m_eleta; //!
   std::vector<Double32_t> m_elphi; //!
   std::vector<Int_t>      m_elcharge; //!

   std::vector<Double32_t> m_gelpt; //!
   std::vector<Double32_t> m_geleta; //!
   std::vector<Double32_t> m_gelphi; //!
   std::vector<Int_t>      m_gelcharge; //!

   std::vector<Double32_t> m_nnelpt; //!
   std::vector<Double32_t> m_nneleta; //!
   std::vector<Double32_t> m_nnelphi; //!
   std::vector<Int_t>      m_nnelcharge; //!

   // gamma 
   std::vector<Double32_t> m_phpt; //!
   std::vector<Double32_t> m_pheta; //!
   std::vector<Double32_t> m_phphi; //!

   std::vector<Double32_t> m_gphpt; //!
   std::vector<Double32_t> m_gpheta; //!
   std::vector<Double32_t> m_gphphi; //!

   std::vector<Double32_t> m_nnphpt; //!
   std::vector<Double32_t> m_nnpheta; //!
   std::vector<Double32_t> m_nnphphi; //!


   // objects for output ntuple 
   int RunNumber; // run number
   int EventNumber; // event number
/*
   TClonesArray *a_jets;
   TClonesArray *t_jets;
   TClonesArray *f_jets;
*/

   double mcEventWeight;

 

   vector<LParticle> JetsReco; 
   vector<LParticle> JetsTrue;
   vector<LParticle> JetsFastNN;

   bool isMC;
   int systematics;
   int type;
   double weight;

};

#endif
