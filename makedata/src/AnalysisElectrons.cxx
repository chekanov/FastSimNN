// S.Chekanov (ANL)

#include "Ana.h"

// fill electrons
Int_t Ana::AnalysisElectrons(vector<LParticle> True, vector<LParticle> Reco) {

	const double EtaMax=maxEta;
	const double PhiMax=PI;

	for(unsigned int j = 0; j<True.size(); j++){
		LParticle tjet = (LParticle)True.at(j);
		TLorentzVector L2 = tjet.GetP();
		float phiT = L2.Phi();
		float ptT =  L2.Perp();
		float etaT = L2.PseudoRapidity();
                int    chargeT =  tjet.GetCharge();
                double isolationT= tjet.GetType()/1000.; // isolation 

		// reco 
		int indexMatch=-1;
		for(unsigned int i = 0; i<Reco.size(); i++){
			LParticle rjet = (LParticle)Reco.at(i);
			TLorentzVector L1 = rjet.GetP();
			double phi = L1.Phi();
			double eta = L1.PseudoRapidity();
			double dEta=etaT-eta;
			double dPhi=phiT-phi;
			if (abs(dPhi)>PI) dPhi=PI2-abs(dPhi);
			double dR=sqrt(dEta*dEta+dPhi*dPhi);
			if (dR<DeltaR) indexMatch=i;
		}

                float charge=0;
                float phi=0;
                float pt=0;
                float eta=0;  
		if (indexMatch>-1){
			LParticle rjet = (LParticle)Reco.at(indexMatch);
			TLorentzVector L1 = rjet.GetP();
			phi = L1.Phi();
			pt  = L1.Perp();
			eta = L1.PseudoRapidity();
                        charge  = rjet.GetCharge();
		}


                float iout=0; // no match
                if (indexMatch>-1)  iout=1.0f;


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
                  input2.push_back((float)chargeT); 
                  input2.push_back((float)isolationT);

                  // output
                  output2.push_back(dpt);  // difference reco-true 
                  output2.push_back(iout); // efficiency (0 or 1) 

		  finput_electrons.push_back(input2);
		  foutput_electrons.push_back(output2);
                } // forget


	}


   if (nevv%nBatch == 0  && nevv>1) {
                cout << "\033[1;31m Writing data with electrons \033[0m\n";

                std::string s = "out_ann/elec_pt_v"+std::to_string(batch_ele)+".d";
                ofstream myfile;
                myfile.open (s); // output file

                vector<float> input = finput_electrons[0];
                vector<float> output = foutput_electrons[0];
                myfile << finput_electrons.size() <<  " " <<  input.size() << " " << output.size() << endl;

                cout << "Write file=" << s << endl;
                cout << "Total entries to write:" << finput_electrons.size() << endl;
                cout << "Nr of inputs  :" << input.size() << endl;
                cout << "Nr of outputs :" << output.size() << endl;

                        int nn=0;
                        for (unsigned int i=0; i<finput_electrons.size(); i++){
                                vector<float> input = finput_electrons[i];
                                vector<float> output = foutput_electrons[i];
                                float ptT=input[0];
                                float etaT=input[1];
                                float phiT=input[2];
                         
                                float dpt=output[0];
                                int   eff=(int)output[1];

                                 for (unsigned int kk=0; kk< input.size(); kk++) myfile <<  input[kk] << " ";
                                 myfile << "" << endl;
                                 myfile <<  dpt  << " " <<   eff  << endl;
                            }

                            myfile.close();
                            finput_electrons.clear();
                            foutput_electrons.clear();

                            batch_ele++;

  }

	return 0;

}

