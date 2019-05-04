// S.Chekanov (ANL)

#include "Ana.h"


Int_t Ana::AnalysisData(vector<LParticle> JetsTrue, vector<LParticle> JetsReco) {

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
		if (indexMatch>-1){
			LParticle rjet = (LParticle)JetsReco.at(indexMatch);
			TLorentzVector L1 = rjet.GetP();
			float phi = L1.Phi();
			float pt  = L1.Perp();
			float eta = L1.PseudoRapidity();
			float mass  = L1.M();
                        btag  = (float)rjet.GetType();
                        float jetr=(float)(rjet.GetCharge()/10000.0); // get jet radius 
                        float jetE=L1.E(); // energy 
                        jetr=jetr/sqrt(jetE);

			vector<float> input;
			vector<float> output;
			input.push_back(ptT);
			input.push_back(etaT);
			input.push_back(phiT);
			input.push_back(massT);
                        input.push_back(jetrT);
            
			output.push_back(pt);
			output.push_back(eta);
			output.push_back(phi);
			output.push_back(mass);
                        output.push_back(jetr);

			// push vectors
			finput_jets.push_back(input);
			foutput_jets.push_back(output);
		}

		// statistics limitted for efficiency. No matching 
		vector<float> input2;
		vector<float> output2;
		input2.push_back((float)ptT);
		input2.push_back((float)etaT);
		input2.push_back((float)phiT);
                input2.push_back((float)btagT); // fraction of b-quark momenta in % (100-0) 
                input2.push_back((float)jetrT); // effective jet size 

                float ibb=-1.0f;
		float iout=-1.0f; // no match
		if (indexMatch>-1)  iout=1.0f;
                if (btag>0)         ibb=1.0f;  // b-tagged and matched

                output2.push_back(iout);
                output2.push_back(ibb);

		finput_jets_eff.push_back(input2);
		foutput_jets_eff.push_back(output2);


	}



	return 0;

}

