// S.Chekanov

#include "Ana.h"

string getEnvVar( std::string const & key ) {
	char * val = getenv( key.c_str() );
	return val == NULL ? std::string("") : std::string(val);
}

// initialize your calculations (histograms)
Int_t Ana::Init() {


	// jet cuts
	minPT=25;
	maxEta=2.5;
	nev=0;
	DeltaR=0.2; // cone to match with jets
	MuPileup=0;
	jet_escale=0;
	jet_mscale=0;
	jet_etascale=0;
        dRbtag=0.4;       //for b-tagging 
        dRisolation=0.4; // for EM isolations 
        btag_frac=0.2;  // fraction of b-quark

	static const char *conf_file = "out_ann/config.cfg";
	Config cfg;
	try
	{
		cout << "Read configuration=" << conf_file << endl;
		cfg.readFile(conf_file);
	}
	catch(const FileIOException &fioex)
	{
		std::cerr << "I/O error while reading file: " <<  conf_file << std::endl;
		return(EXIT_FAILURE);
	}
	catch(const ParseException &pex)
	{
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
		<< " - " << pex.getError() << std::endl;
		return(EXIT_FAILURE);
	}


	const Setting& root = cfg.getRoot();

	// ML
	Setting &mL = root["DelphesML"];
	if (!(mL.lookupValue("pileup_mu", MuPileup)
                        && mL.lookupValue("dR_btag", dRbtag)
                        && mL.lookupValue("dR_isolation", dRisolation)
                        && mL.lookupValue("bquark_frac", btag_frac)
	                && mL.lookupValue("number_of_inputs",   num_input)
                        && mL.lookupValue("number_of_kinematics",   num_kin)
                        && mL.lookupValue("number_of_outputs",   num_output)
	                && mL.lookupValue("number_of_inputs_eff",  num_input_eff)
	                && mL.lookupValue("number_of_outputs_eff",  num_output_eff)
	     ))  {
		cout << "Error: some values are not in DelphesML configuration!" << endl;
		exit(0);
	}

	Setting &njets = root["Jets"];
	if (!(njets.lookupValue("EnergyBinsNr", nBins)
	                && njets.lookupValue("MinPT", minPT)
	                && njets.lookupValue("MaxEta", maxEta)
	                && njets.lookupValue("PtBinsForResolution", nBinsNN)
	                && njets.lookupValue("SlicesEtaPhi4Input", slices_etaphi)
	                && njets.lookupValue("EnergyScale",jet_escale)
	                && njets.lookupValue("EnergyShift",jet_eshift)
	                && njets.lookupValue("MassScale",jet_mscale)
	                && njets.lookupValue("MassShift",jet_mshift)
	                && njets.lookupValue("EtaScale",jet_etascale)
	                && njets.lookupValue("EtaShift",jet_etashift))

	   ) {
		cout << "Error: some values are not in Jet configuration!" << endl;
		exit(0);
	}


        Setting &em = root["EMObjects"];
        if (!(em.lookupValue("EnergyBinsNr", EMnBins)
                        && em.lookupValue("MinPT", EMminPT)
                        && em.lookupValue("MaxEta", EMmaxEta)
                        && em.lookupValue("EnergyScale",em_escale)
                        && em.lookupValue("EnergyShift",em_eshift)
                        && em.lookupValue("EtaScale",em_etascale)
                        && em.lookupValue("EtaShift",em_etashift))

           ) {
                cout << "Error: some values are not in EMObjects configuration!" << endl;
                exit(0);
        }





	cout << "############# ANN settingsi ############" << endl;
	cout << "EnergyScale=" << jet_escale << endl;
	cout << "EnergyShift=" << jet_eshift << endl;
	cout << "MassScale=" << jet_mscale << endl;
	cout << "MassShift=" << jet_mshift << endl;
	cout << "EtaScale=" << jet_etascale << endl;
	cout << "EtaShift=" << jet_etashift << endl;
	cout << "SlicesEtaPhi4Input=" << slices_etaphi << endl;


	if (jet_escale == 0 || jet_mscale == 0 || jet_etascale == 0){
		cout << "Error: failed to read scale factors from configuration file !" << endl;
		exit(0);
	}

	eBins = new double[nBinsNN];
	const Setting &bin_settings = njets.lookup("PtBins");
	for (int n = 0; n < (int)bin_settings.getLength(); ++n) eBins[n]=bin_settings[n];
	if ((int)bin_settings.getLength() != nBins) {
		cout << "getLength() =" << bin_settings.getLength() << "  nBinsNN=" << nBins << endl;
		cout << "Error: Bin sizes for jets cannot be read!" << endl;
		exit(0);
	}


	mcEventWeight=1.0;

	// vector with bins for reco/true
	ann1_jets_name = new string[nBinsNN-1];
	ann2_jets_name = new string[nBinsNN-1];
	ann3_jets_name = new string[nBinsNN-1];
	ann4_jets_name = new string[nBinsNN-1];
	ann5_jets_name = new string[nBinsNN-1];

	ann1_jets = new fann*[nBinsNN-1];
	ann2_jets = new fann*[nBinsNN-1];
	ann3_jets = new fann*[nBinsNN-1];
	ann4_jets = new fann*[nBinsNN-1];
	ann5_jets = new fann*[nBinsNN-1];


        // muons
        ann1_muons_name = new string[nBinsNN-1];
        ann2_muons_name = new string[nBinsNN-1];
        ann3_muons_name = new string[nBinsNN-1];
        ann4_muons_name = new string[nBinsNN-1];
        ann5_muons_name = new string[nBinsNN-1];

        ann1_muons = new fann*[nBinsNN-1];
        ann2_muons = new fann*[nBinsNN-1];
        ann3_muons = new fann*[nBinsNN-1];
        ann4_muons = new fann*[nBinsNN-1];
        ann5_muons = new fann*[nBinsNN-1];

        // electrons
        ann1_electrons_name = new string[nBinsNN-1];
        ann2_electrons_name = new string[nBinsNN-1];
        ann3_electrons_name = new string[nBinsNN-1];
        ann4_electrons_name = new string[nBinsNN-1];
        ann5_electrons_name = new string[nBinsNN-1];

        ann1_electrons = new fann*[nBinsNN-1];
        ann2_electrons = new fann*[nBinsNN-1];
        ann3_electrons = new fann*[nBinsNN-1];
        ann4_electrons = new fann*[nBinsNN-1];
        ann5_electrons = new fann*[nBinsNN-1];

        // photons
        ann1_photons_name = new string[nBinsNN-1];
        ann2_photons_name = new string[nBinsNN-1];
        ann3_photons_name = new string[nBinsNN-1];
        ann4_photons_name = new string[nBinsNN-1];
        ann5_photons_name = new string[nBinsNN-1];

        ann1_photons = new fann*[nBinsNN-1];
        ann2_photons = new fann*[nBinsNN-1];
        ann3_photons = new fann*[nBinsNN-1];
        ann4_photons = new fann*[nBinsNN-1];
        ann5_photons = new fann*[nBinsNN-1];




	initialRME = new double[nBinsNN-1];
	finalRME   = new double[nBinsNN-1];
	BinOverTrue = new int[nBinsNN-1];

	for (int jjj=0; jjj<nBinsNN-1; jjj++) BinOverTrue[jjj]=jjj;

	//eBins = new double[nBins];
	for (int m=0; m<nBins; m++){
		cout << "Used jet bins=" << eBins[m] << endl;
	};

	cout << " ###  Initialize ANN in Nr bins=" <<nBins-1<<  endl;

	for (int m=0; m<nBins-1; m++){

		initialRME[m]=0;
		finalRME[m]=0;

		// jets and efficiencis
		ann1_jets_name[m]="out_ann/ann1_jet_"+std::to_string(m)+".net";
		ann2_jets_name[m]="out_ann/ann2_jet_"+std::to_string(m)+".net";
		ann3_jets_name[m]="out_ann/ann3_jet_"+std::to_string(m)+".net";
		ann4_jets_name[m]="out_ann/ann4_jet_"+std::to_string(m)+".net";
		ann5_jets_name[m]="out_ann/ann5_jet_"+std::to_string(m)+".net";

		cout << "Read ANN from files.."  << endl;
		ann1_jets[m] = fann_create_from_file(ann1_jets_name[m].c_str());
		ann2_jets[m] = fann_create_from_file(ann2_jets_name[m].c_str());
		ann3_jets[m] = fann_create_from_file(ann3_jets_name[m].c_str());
		ann4_jets[m] = fann_create_from_file(ann4_jets_name[m].c_str());
		ann5_jets[m] = fann_create_from_file(ann5_jets_name[m].c_str());

                // muon
                ann1_muons_name[m]="out_ann/ann1_muon_"+std::to_string(m)+".net";
                ann2_muons_name[m]="out_ann/ann2_muon_"+std::to_string(m)+".net";
                ann3_muons_name[m]="out_ann/ann3_muon_"+std::to_string(m)+".net";
                ann4_muons_name[m]="out_ann/ann4_muon_"+std::to_string(m)+".net";
                ann5_muons_name[m]="out_ann/ann5_muon_"+std::to_string(m)+".net";

                ann1_muons[m] = fann_create_from_file(ann1_muons_name[m].c_str());
                ann2_muons[m] = fann_create_from_file(ann2_muons_name[m].c_str());
                ann3_muons[m] = fann_create_from_file(ann3_muons_name[m].c_str());
                ann4_muons[m] = fann_create_from_file(ann4_muons_name[m].c_str());
                ann5_muons[m] = fann_create_from_file(ann5_muons_name[m].c_str());

                // electron
                ann1_electrons_name[m]="out_ann/ann1_electron_"+std::to_string(m)+".net";
                ann2_electrons_name[m]="out_ann/ann2_electron_"+std::to_string(m)+".net";
                ann3_electrons_name[m]="out_ann/ann3_electron_"+std::to_string(m)+".net";
                ann4_electrons_name[m]="out_ann/ann4_electron_"+std::to_string(m)+".net";
                ann5_electrons_name[m]="out_ann/ann5_electron_"+std::to_string(m)+".net";

                ann1_electrons[m] = fann_create_from_file(ann1_electrons_name[m].c_str());
                ann2_electrons[m] = fann_create_from_file(ann2_electrons_name[m].c_str());
                ann3_electrons[m] = fann_create_from_file(ann3_electrons_name[m].c_str());
                ann4_electrons[m] = fann_create_from_file(ann4_electrons_name[m].c_str());
                ann5_electrons[m] = fann_create_from_file(ann5_electrons_name[m].c_str());

                // photon
                ann1_photons_name[m]="out_ann/ann1_photon_"+std::to_string(m)+".net";
                ann2_photons_name[m]="out_ann/ann2_photon_"+std::to_string(m)+".net";
                ann3_photons_name[m]="out_ann/ann3_photon_"+std::to_string(m)+".net";
                ann4_photons_name[m]="out_ann/ann4_photon_"+std::to_string(m)+".net";
                ann5_photons_name[m]="out_ann/ann5_photon_"+std::to_string(m)+".net";

                ann1_photons[m] = fann_create_from_file(ann1_photons_name[m].c_str());
                ann2_photons[m] = fann_create_from_file(ann2_photons_name[m].c_str());
                ann3_photons[m] = fann_create_from_file(ann3_photons_name[m].c_str());
                ann4_photons[m] = fann_create_from_file(ann4_photons_name[m].c_str());
                ann5_photons[m] = fann_create_from_file(ann5_photons_name[m].c_str());


		if (m==nBins-2) {
			cout << "# Structure for last ANN" << endl;
			fann_print_connections(ann1_jets[m]);
			fann_print_parameters(ann1_jets[m]);
		};



	}



	ffile="Analysis.root";
	cout << "\n -> Output file is =" << ffile << endl;
	RootFile = new TFile(ffile.c_str(), "RECREATE", "Histogram file");
	// define histograms
	h_debug = new TH1D("debug", "debug", 10, 0, 10);
	h_dR = new TH1D("jet_dR", "truth-jet distance", 500, 0, 7);

	h_jetpt = new TH1D("jet_PT", "jet_PT reconstructed", 100, 0, 1000);  h_jetpt->Sumw2();
	h_jetpt_truth = new TH1D("jet_truth_PT", "jet_PT truth", 100, 0, 1000);  h_jetpt_truth->Sumw2();
	h_jetpt_nn = new TH1D("jet_nn_PT", "jet_PT from NN", 100, 0, 1000);  h_jetpt_nn->Sumw2();
	h_jetcutflow = new TH1D("jet_cutflow", "jet cutflow", 10, 0, 10);

	h_jetres100 = new TH1D("jet_res50", "pT/pTtrue ratio", 200, 0.0, 3.0);
	h_jetres1000 = new TH1D("jet_res200", "pT/pTtrue ratio", 200, 0.0, 3.0);

	h_ptcor = new TH1D("jet_ptcor", "PT correction", 200, 0.0, 3);
	h_etacor = new TH1D("jet_etacor", "Eta correction", 200, 0.0, 3);
	h_phicor = new TH1D("jet_phicor", "Phi correction", 200, 0.0, 3);

	h_in1_jet = new TH1D("in1_jet", "in1 pt jet", nBinsNN, -1.1, 1.1);
	h_in2_jet = new TH1D("in2_jet", "in2 eta jet", nBinsNN, -1.1, 1.1);
	h_in3_jet = new TH1D("in3_jet", "in3 phi jet", nBinsNN, -1.1, 1.1);
	h_in4_jet = new TH1D("in4_jet", "in4 M jet", nBinsNN, -1.1, 1.1);
        h_in5_jet = new TH1D("in5_jet", "in5 dR", nBinsNN, -1.1, 1.1);

	h_out1_jet = new TH1D("out1_jet", "out1", nBinsNN, -1.1, 1.1);
	h_out2_jet = new TH1D("out2_jet", "out2", nBinsNN, -1.1, 1.1);
	h_out3_jet = new TH1D("out3_jet", "out3", nBinsNN, -1.1, 1.1);
	h_out4_jet = new TH1D("out4_jet", "out4", nBinsNN, -1.1, 1.1);
	h_out5_jet_eff = new TH1D("out5_jet_eff", "out5 efficiency", nBinsNN, -1.1, 1.1);
	h_out6_jet_btag = new TH1D("out6_jet_btag", "out6 b-tagging", nBinsNN, -1.1, 1.1);

	h_rout1_jet = new TH1D("rout1_jet", "out1 jet random bin", nBinsNN, 0, nBinsNN);
	h_rout2_jet = new TH1D("rout2_jet", "out2 jet random bin", nBinsNN, 0, nBinsNN);
	h_rout3_jet = new TH1D("rout3_jet", "out3 jet random bin", nBinsNN, 0, nBinsNN);
	h_rout4_jet = new TH1D("rout4_jet", "out4 jet random bin", nBinsNN, 0, nBinsNN);


        h_in1_mu = new TH1D("in1_mu", "in1 mu", nBinsNN, -1.1, 1.1);
        h_in2_mu = new TH1D("in2_mu", "in2 mu", nBinsNN, -1.1, 1.1);
        h_in3_mu = new TH1D("in3_mu", "in3 mu", nBinsNN, -1.1, 1.1);
        h_in4_mu = new TH1D("in4_mu", "in4 mu (charge)", nBinsNN, -1.1, 1.1);

        h_out1_mu = new TH1D("out1_mu", "out1 mu", nBinsNN, -1.1, 1.1);
        h_out2_mu = new TH1D("out2_mu", "out2 mu", nBinsNN, -1.1, 1.1);
        h_out3_mu = new TH1D("out3_mu", "out3 mu", nBinsNN, -1.1, 1.1);
        h_out4_mu = new TH1D("out4_mu", "out4 mu", nBinsNN, -1.1, 1.1);
        h_rout1_mu = new TH1D("rout1_mu", "out1 mu random bin", nBinsNN, 0, nBinsNN);
        h_rout2_mu = new TH1D("rout2_mu", "out2 mu random bin", nBinsNN, 0, nBinsNN);
        h_rout3_mu = new TH1D("rout3_mu", "out3 mu random bin", nBinsNN, 0, nBinsNN);
        h_rout4_mu = new TH1D("rout4_mu", "out4 mu random bin", nBinsNN, 0, nBinsNN);

        h_out5_mu_eff = new TH1D("out5_mu_eff", "out5 mu efficiency", nBinsNN, -1.1, 1.1);
        h_out5_el_eff = new TH1D("out5_el_eff", "out5 el efficiency", nBinsNN, -1.1, 1.1);
        h_out5_ph_eff = new TH1D("out5_ph_eff", "out5 ph efficiency", nBinsNN, -1.1, 1.1);


	// create ntuple
	m_ntuple  = new TTree("Ntuple","Ntuple");
	m_ntuple->Branch("RunNumber",   &RunNumber,  "RunNumber/I");
	m_ntuple->Branch("EventNumber", &EventNumber,   "EventNumber/I");
	m_ntuple->Branch("AntiKt4JetPt",    &m_jetpt);
	m_ntuple->Branch("AntiKt4JetEta",   &m_jeteta);
	m_ntuple->Branch("AntiKt4JetPhi",   &m_jetphi);
	m_ntuple->Branch("AntiKt4JetM",     &m_jetm);
	m_ntuple->Branch("AntiKt4JetBtag",  &m_jetbtag);
	// matched
	m_ntuple->Branch("AntiKt4MatchedJetPt",    &m_matchedjetpt);
	m_ntuple->Branch("AntiKt4MatchedJetEta",   &m_matchedjeteta);
	m_ntuple->Branch("AntiKt4MatchedJetPhi",   &m_matchedjetphi);
	m_ntuple->Branch("AntiKt4MatchedJetM",     &m_matchedjetm);
	m_ntuple->Branch("AntiKt4MatchedJetBtag",  &m_matchedjetbtag);
	// truth
	m_ntuple->Branch("AntiKt4TruthJetPt",    &m_gjetpt);
	m_ntuple->Branch("AntiKt4TruthJetEta",   &m_gjeteta);
	m_ntuple->Branch("AntiKt4TruthJetPhi",   &m_gjetphi);
	m_ntuple->Branch("AntiKt4TruthJetM",     &m_gjetm);
	m_ntuple->Branch("AntiKt4TruthJetBtag",  &m_gjetbtag);
	// NN jets
	m_ntuple->Branch("AntiKt4NNJetPt",    &m_nnjetpt);
	m_ntuple->Branch("AntiKt4NNJetEta",   &m_nnjeteta);
	m_ntuple->Branch("AntiKt4NNJetPhi",   &m_nnjetphi);
	m_ntuple->Branch("AntiKt4NNJetM",     &m_nnjetm);
	m_ntuple->Branch("AntiKt4NNJetBtag",  &m_nnjetbtag);

        // muons
        m_ntuple->Branch("muonPt",    &m_mupt);
        m_ntuple->Branch("muonEta",   &m_mueta);
        m_ntuple->Branch("muonPhi",   &m_muphi);
        m_ntuple->Branch("muonCharge",   &m_mucharge);

        m_ntuple->Branch("muonTruthPt",    &m_gmupt);
        m_ntuple->Branch("muonTruthEta",   &m_gmueta);
        m_ntuple->Branch("muonTruthPhi",   &m_gmuphi);
        m_ntuple->Branch("muonTrueCharge",   &m_gmucharge);

        m_ntuple->Branch("muonNNPt",    &m_nnmupt);
        m_ntuple->Branch("muonNNEta",   &m_nnmueta);
        m_ntuple->Branch("muonNNPhi",   &m_nnmuphi);
        m_ntuple->Branch("muonNNCharge",   &m_nnmucharge);

        // electrons
        m_ntuple->Branch("electronPt",    &m_elpt);
        m_ntuple->Branch("electronEta",   &m_eleta);
        m_ntuple->Branch("electronPhi",   &m_elphi);
        m_ntuple->Branch("electronCharge",   &m_elcharge);

        m_ntuple->Branch("electronTruthPt",    &m_gelpt);
        m_ntuple->Branch("electronTruthEta",   &m_geleta);
        m_ntuple->Branch("electronTruthPhi",   &m_gelphi);
        m_ntuple->Branch("electronTruthCharge",   &m_gelcharge);
 
        m_ntuple->Branch("electronNNPt",    &m_nnelpt);
        m_ntuple->Branch("electronNNEta",   &m_nneleta);
        m_ntuple->Branch("electronNNPhi",   &m_nnelphi);
        m_ntuple->Branch("electronNNCharge",   &m_nnelcharge);

        // photons
        m_ntuple->Branch("photonPt",    &m_phpt);
        m_ntuple->Branch("photonEta",   &m_pheta);
        m_ntuple->Branch("photonPhi",   &m_phphi);

        m_ntuple->Branch("photonTruthPt",    &m_gphpt);
        m_ntuple->Branch("photonTruthEta",   &m_gpheta);
        m_ntuple->Branch("photonTruthPhi",   &m_gphphi);

        m_ntuple->Branch("photonNNPt",    &m_nnphpt);
        m_ntuple->Branch("photonNNEta",   &m_nnpheta);
        m_ntuple->Branch("photonNNPhi",   &m_nnphphi);



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
		ntup.push_back(temp);
	}
	cout << "-> Number of files=" << ntup.size()  << endl;
	myfile.close();
	for (unsigned int i=0; i<ntup.size(); i++) {
		cout << i << " file to analyse="+ntup[i] << endl;
	}


	return 0;
}



// get a random value from a PDF
int Ana::findCeil(int  arr[], int r, int l, int h)
{
        int mid;
        while (l < h)
        {
                mid = l + ((h - l) >> 1); // Same as mid = (l+h)/2
                (r > arr[mid]) ? (l = mid + 1) : (h = mid);
        }
        int va=(arr[l] >= r) ? l : -1;
        // if negative, assume no correction (center)
        if (va<0) va=(int)(h/2.0);
        //if (va<0)  va=rand() % h; // random smearing  
        return va;
}

// The main function that returns a random number from arr[] according to
// distribution array defined by freq[]. n is size of arrays.
int Ana::myRand(int arr[], int freq[], int n)
{
        // Create and fill prefix array
        int prefix[n], i;
        prefix[0] = freq[0];
        for (i = 1; i < n; ++i)
                prefix[i] = prefix[i - 1] + freq[i];

        // prefix[n-1] is sum of all frequencies. Generate a random number
        // with value from 1 to this sum
        if (prefix[n - 1] != 0) {
                int r = (rand() % prefix[n - 1]) + 1;
                //long  R1=Reta.Rndm()*RAND_MAX;
                //int r = (R1 % prefix[n - 1]) + 1;
                //cout << "Random value on [0 " << RAND_MAX << endl;
                // Find index of ceiling of r in prefix arrat
                int indexc = findCeil(prefix, r, 0, n - 1);
                return arr[indexc];
        }

          //int va=rand() % n;
           int va=(int)(n/2.0); // assume no correction when error
           return va; // random smearing  
}

