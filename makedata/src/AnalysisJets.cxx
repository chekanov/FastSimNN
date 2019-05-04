// S.Chekanov (ANL)

#include "Ana.h"


Int_t Ana::AnalysisJets(vector<LParticle> JetsTrue, vector<LParticle> JetsReco) {

	const double EtaMax=maxEta;
	const double PhiMax=PI;

	for(unsigned int j = 0; j<JetsTrue.size(); j++){
		LParticle tjet = (LParticle)JetsTrue.at(j);
		TLorentzVector L2 = tjet.GetP();
		float phiT = L2.Phi();
		float ptT =  L2.Perp();
		float etaT = L2.PseudoRapidity();
		float massT =  L2.M();
                float btagT=(float)tjet.GetType(); // get  b-quark in 10x100%
                float jetrT=(float)(tjet.GetCharge()/10000.0); // get jet radius in Eta and Phi 
                float jetE=L2.E(); // energy 
                jetrT =  jetrT / sqrt(jetE); // effective jet radius in GeV-1/2 (sqrt to increase values) 


		// reco jets
		int indexMatch=-1;
		for(unsigned int i = 0; i<JetsReco.size(); i++){
			LParticle rjet = (LParticle)JetsReco.at(i);
			TLorentzVector L1 = rjet.GetP();
			double phi = L1.Phi();
			double eta = L1.PseudoRapidity();
			double dEta=etaT-eta;
			double dPhi=phiT-phi;
			if (abs(dPhi)>PI) dPhi=PI2-abs(dPhi);
			double dR=sqrt(dEta*dEta+dPhi*dPhi);
			h_dR->Fill(dR);
			if (dR<DeltaR) indexMatch=i;
		}

                float btag=0;
                float phi=0;
                float pt=0;
                float eta=0;  
                float mass=0; 
                 float jetr=0;
		if (indexMatch>-1){
			LParticle rjet = (LParticle)JetsReco.at(indexMatch);
			TLorentzVector L1 = rjet.GetP();
			phi = L1.Phi();
			pt  = L1.Perp();
			eta = L1.PseudoRapidity();
			float mass  = L1.M();
                        btag  = (float)rjet.GetType();
                        jetr=(float)(rjet.GetCharge()/10000.0); // get jet radius 
                        jetE=L1.E(); // energy 
                        jetr=jetr/sqrt(jetE);
		}

		// statistics limitted for efficiency. No matching 
		vector<float> input2;
		vector<float> output2;
		input2.push_back((float)ptT);
		input2.push_back((float)etaT);
		input2.push_back((float)phiT);
                input2.push_back((float)massT); // fraction of b-quark momenta in % (100-0) 
                input2.push_back((float)jetrT); // effective jet size 
                input2.push_back((float)btagT);

                float ibb=0.0f;
		float iout=0.0f; // no match
		if (indexMatch>-1)  iout=1.0f;
                if (btag>0)         ibb=1.0f;  // b-tagged and matched

                output2.push_back(pt); 
                output2.push_back(iout);

		finput_jets.push_back(input2);
		foutput_jets.push_back(output2);

	}


   if (nevv%nBatch == 0  && nevv>1) {
                cout << "\033[1;31m Writing data with jets \033[0m\n";

                std::string s = "data/jet_pt_data"+std::to_string(batch_jet);
                ofstream myfile;
                myfile.open (s); // output file

// create a dataset for a given bin
                        int nn=0;
                        for (unsigned int i=0; i<finput_jets.size(); i++){
                                vector<float> input = finput_jets[i];
                                vector<float> output = foutput_jets[i];
                                float ptT=input[0];
                                float etaT=input[1];
                                float phiT=input[2];
                                float massT=input[3];
                                float jetRT=input[4];
                         
                                float pt=output[0];
                                int  eff=(int)output[1];

                                 for (unsigned int kk=0; kk< input.size(); kk++) myfile <<  input[kk] << " ";
                                 myfile << "" << endl;
                                 myfile <<  pt  << " " <<   eff  << endl;



                            }

                            myfile.close();
                            finput_jets.clear();
                            foutput_jets.clear();

                            batch_jet++;

  }

	return 0;

}

