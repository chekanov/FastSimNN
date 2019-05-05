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
                float btagT=(float)tjet.GetType()/1000.0; // get  b-quark in 10x100%
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
			mass  = L1.M();
                        btag  = (float)rjet.GetType();
                        jetr=(float)(rjet.GetCharge()/10000.0); // get jet radius 
                        jetE=L1.E(); // energy 
                        jetr=jetr/sqrt(jetE);
		}


                float ibb=0;
                float iout=0; // no match
                if (indexMatch>-1)  iout=1.0f;
                if (btag>0)         ibb=1.0f;  // b-tagged and matched


               // protect against large fluctuations
               bool forget=false;
               float dpt= (pt-ptT);
               float ratio=dpt/ptT;
               if (iout>0 && abs(ratio) > 0.8) forget=true; 
               if (iout==0) dpt=0;

               if (forget==false) {
		  vector<float> input2;
		  vector<float> output2;
		  input2.push_back((float)ptT);
		  input2.push_back((float)etaT);
		  input2.push_back((float)phiT);
                  input2.push_back((float)massT); // fraction of b-quark momenta in % (100-0) 
                  input2.push_back((float)jetrT); // effective jet size 
                  input2.push_back((float)btagT);

                  // output
                  output2.push_back(dpt);  // difference reco-true 
                  output2.push_back(iout); // efficiency (0 or 1) 

		  finput_jets.push_back(input2);
		  foutput_jets.push_back(output2);
                } // forget


	}


   if (nevv%nBatch == 0  && nevv>1) {
                cout << "\033[1;31m Writing data with jets \033[0m\n";

                std::string s = "out_ann/jet_pt_v"+std::to_string(batch_jet)+".d";
                ofstream myfile;
                myfile.open (s); // output file

                vector<float> input = finput_jets[0];
                vector<float> output = foutput_jets[0];
                myfile << finput_jets.size() <<  " " <<  input.size() << " " << output.size() << endl;

                cout << "Write file=" << s << endl;
                cout << "Total entries to write:" << finput_jets.size() << endl;
                cout << "Nr of inputs  :" << input.size() << endl;
                cout << "Nr of outputs :" << output.size() << endl;

                        int nn=0;
                        for (unsigned int i=0; i<finput_jets.size(); i++){
                                vector<float> input = finput_jets[i];
                                vector<float> output = foutput_jets[i];
                                float ptT=input[0];
                                float etaT=input[1];
                                float phiT=input[2];
                                float massT=input[3];
                                float jetRT=input[4];
                         
                                float dpt=output[0];
                                int   eff=(int)output[1];

                                 for (unsigned int kk=0; kk< input.size(); kk++) myfile <<  input[kk] << " ";
                                 myfile << "" << endl;
                                 myfile <<  dpt  << " " <<   eff  << endl;
                            }

                            myfile.close();
                            finput_jets.clear();
                            foutput_jets.clear();

                            batch_jet++;

  }

	return 0;

}

