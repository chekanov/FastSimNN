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
        batch_jet=0;


	ffile="Analysis.root";
	cout << "\n -> Output file is =" << ffile << endl;
	RootFile = new TFile(ffile.c_str(), "RECREATE", "Histogram file");
	// define histograms
	h_debug = new TH1D("debug", "debug", 10, 0, 10);



        // read files from data.in file and put to a vector
        string name="data.in";
        ifstream myfile;
        myfile.open(name.c_str(), ios::in);
        if (!myfile) {
                cerr << "Can't open input file:  " << name << endl;
                exit(1);
        } else {
                cout << "-> Read data file=" << name << endl;
        }
        string temp;
        while (myfile >> temp) {
                //the following line trims white space from the beginning of the string
                temp.erase(temp.begin(), std::find_if(temp.begin(), temp.end(), not1(ptr_fun<int, int>(isspace))));
                if (temp.find("#") == 0) continue;

                // detect files with pileup (automatically)
                std::size_t found = temp.find("rfast006");
                if (found!=std::string::npos) MuPileup=140; // detect pileup events from HepSim
                // detect files with pileup (automatically)
                found = temp.find("rfast007");
                if (found!=std::string::npos) MuPileup=40; // detect pileup events from HepSim

                ntup.push_back(temp);
        }
        cout << "-> Number of files=" << ntup.size()  << endl;
        myfile.close();
        for (unsigned int i=0; i<ntup.size(); i++) {
                cout << i << " file to analyse="+ntup[i] << endl;
        }


	return 0;
}

