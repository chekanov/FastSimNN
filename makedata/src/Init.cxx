// S.Chekanov (ANL)

#include "Ana.h"

string getEnvVar( std::string const & key ) {
	char * val = getenv( key.c_str() );
	return val == NULL ? std::string("") : std::string(val);
}

// initialize  the code 
Int_t Ana::Init() {

        minPT=25; 
	maxEta=3.0;
	nevv=0;
	DeltaR=0.2; // cone to match with jets
	MuPileup=0;
        dRbtag=0.4;      //for b-tagging (antiKT4 jets!) 
        dRisolation=0.4; // for EM isolation (as for Delphes)  
        btag_frac=0.2;   // fraction of b-quark


	ffile="Analysis.root";
	cout << "\n -> Output file is =" << ffile << endl;
	RootFile = new TFile(ffile.c_str(), "RECREATE", "Histogram file");
	// define histograms
	h_debug = new TH1D("debug", "debug", 10, 0, 10);


	return 0;
}

