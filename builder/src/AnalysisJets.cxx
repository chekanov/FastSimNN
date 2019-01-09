// S.Chekanov (ANL)

#include "Ana.h"


Int_t Ana::AnalysisJets(vector<LParticle> JetsTrue, vector<LParticle> JetsReco) {

	const double EtaMax=maxEta;
	const double PhiMax=PI;
	const double delta=2.0/(nBinsNN);                // slices for resolution 
        const double slicesEta=(2*EtaMax)/slices_etaphi; // slices in eta  
        const double slicesPhi=(2*PhiMax)/slices_etaphi; // slices in phi  

	for(unsigned int j = 0; j<JetsTrue.size(); j++){
		LParticle tjet = (LParticle)JetsTrue.at(j);
		TLorentzVector L2 = tjet.GetP();
		double phiT = L2.Phi();
		double ptT =  L2.Perp();
		double etaT = L2.PseudoRapidity();
		double massT =  L2.M();
                double btagT=(double)tjet.GetType(); // get  b-quark in 10x100%
                double jetrT=(double)(tjet.GetCharge()/10000.0); // get jet radius 

		for (int m=0; m<nBins-1; m++){
			double dmin=eBins[m];
			double dmax=eBins[m+1];
			//double width=dmax-dmin;
			//double dcenter=dmin+0.5*width;
			if (ptT>dmin && ptT<=dmax)  eventsJetBins[m]++;
		}


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
                        double jetr=(double)(rjet.GetCharge()/10000.0); // get jet radius 

			vector<float> input;
			vector<float> output;
			input.push_back((float)ptT);
			input.push_back((float)etaT);
			input.push_back((float)phiT);
			input.push_back((float)massT);
                        input.push_back((float)jetrT);

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

                float ibb=-1.0f;
		float iout=-1.0f; // no match
		if (indexMatch>-1)  iout=1.0f;
                if (btag>0)         ibb=1.0f;  // b-tagged and matched

                output2.push_back(iout);
                output2.push_back(ibb);

		finput_jets_eff.push_back(input2);
		foutput_jets_eff.push_back(output2);


	}


	if (nevv%nBatch == 0  && nevv>0) {
                cout << "\033[1;31m Training of jets \033[0m\n";
		cout << "Jets: ### Reset MSE after =" << nevv << " training " << nEpoch << " epoches " << endl;

		for (int m=0; m<nBins-1; m++){
			double dmin=eBins[m];
			double dmax=eBins[m+1];
			double width=dmax-dmin;
			if (eventsJetBins[m]<MinEntries) continue;
			cout << "-> Jets:  Training for energy bin = " << m << endl;

			// empty data
			fann_train_data *  dataset1=  fann_create_train(eventsJetBins[m], num_input, num_output);
                        fann_train_data *  dataset2=  fann_create_train(eventsJetBins[m], num_input, num_output);
                        fann_train_data *  dataset3=  fann_create_train(eventsJetBins[m], num_input, num_output);
                        fann_train_data *  dataset4=  fann_create_train(eventsJetBins[m], num_input, num_output);

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
				float eta=output[1];
				float phi=output[2];
				float mass=output[3];
                                float jetR=output[4];


				if (ptT>dmin && ptT<=dmax) {

                                        float dminmax=dmin+0.5*width;
					float ptIN=((ptT-dminmax)/(0.5*width));
					float etaIN=etaT/EtaMax; // range -1 -1
					float phiIN=phiT/PhiMax; // range -1-1 from -pi - pi
					float massIN=-1+(massT/(0.5*dmin));

                                        // sliced input for NN
                                        float etaINSlice[slices_etaphi-1];
                                        // bin the resolution plots
                                        for (int jjj=0; jjj<slices_etaphi-1; jjj++) {
                                                float d1=-EtaMax+jjj*slicesEta;
                                                float d2=d1+slicesEta;
                                                //float dmm=d1+0.5*slicesEta;
                                                if (etaT>d1  && etaT<=d2) etaINSlice[jjj]=1.0f; //    etaINSlice[jjj]=(etaT-dmm)/(0.5*dmm);
                                                else etaINSlice[jjj]=0;
                                        }

                                        float phiINSlice[slices_etaphi-1];
                                        for (int jjj=0; jjj<slices_etaphi-1; jjj++) {
                                                float d1=-PhiMax+jjj*slicesPhi;
                                                float d2=d1+slicesPhi;
                                                //float dmm=d1+0.5*slicesPhi;
                                                if (phiT>d1  && phiT<=d2) phiINSlice[jjj]=1.0f; //    phiINSlice[jjj]=(phiT-dmm)/(0.5*dmm);
                                                else phiINSlice[jjj]=0;
                                        }


	                   		// assume -1 - 1 range.
					// scale to avoid sharp behaviour near 0
					// for pileup we make the distribution narrower and shift to -0.5 to fit to [-1,1]
					float ptOUT= jet_escale*((pt/ptT)-1) + jet_eshift;
					//float etaOUT=norm*(eta-etaT);
					float etaOUT=0;
					// make broader rescale for a better efficiency for -1 and 1 range
					if (etaT !=0) etaOUT=jet_etascale*(abs(eta/etaT)-1) + jet_etashift;
					//float phiOUT=norm*(phi-phiT);
					float  phiOUT=0;
					if (phiT !=0) phiOUT=jet_etascale*(abs(phi/phiT)-1) +  jet_etashift; 
					//float eeOUT=abs(ee-eeT)/dmax;
					// shrink pT (important for pileup!)
					float mOUT=jet_mscale*((mass/massT) -1) + jet_mshift;
					//float shiftOUT=0.0f;
					//if (ee-eeT>0) shiftOUT=1.0f; // gain or positive shift

					fann_type uinput[num_input];
                                        // outputs
					fann_type uoutput1[num_output];
                                        fann_type uoutput2[num_output];
                                        fann_type uoutput3[num_output];
                                        fann_type uoutput4[num_output];
 
					uinput[0] = ptIN;
                                        uinput[1] = massIN;
                                        uinput[2] = etaIN;
                                        uinput[3] = phiIN;
                                        uinput[4] = jetRT; 

                                        // eta and phi are sliced for ANN
                                        // this is needed to reproduce spacial defects 
                                        int shift=num_kin; 
                                        int kshift=0; 
                                        for (int jjj=0; jjj<slices_etaphi-1; jjj++) {
                                                uinput[shift+kshift] =  etaINSlice[jjj];
                                                kshift++;
                                        }
                                        shift=shift+slices_etaphi-1;
                                        kshift=0;
                                        for (int jjj=0; jjj<slices_etaphi-1; jjj++) {
                                                uinput[shift+kshift] =  phiINSlice[jjj];
                                                kshift++;
                                        }


                                        // debug input
                                        for (int jjj=0; jjj<num_input; jjj++) {
                                           if (isnan(uinput[jjj])) {
                                           cout << "NaN was dedected on input " << jjj << endl;
                                           }
                                         } 


					for (int jjj=0; jjj<num_output; jjj++) {uoutput1[jjj]=0; 
                                                                                uoutput2[jjj]=0; 
                                                                                uoutput3[jjj]=0; 
                                                                                uoutput4[jjj]=0; } 

					// bin the resolution plots
					for (int jjj=0; jjj<nBinsNN-1; jjj++) {
						float d1=-1.0+jjj*delta;
						float d2=d1+delta;
						if (ptOUT>d1  && ptOUT<=d2)    uoutput1[jjj]=1.0f;
						if (etaOUT>d1 && etaOUT<=d2)   uoutput2[jjj]=1.0f;
						if (phiOUT>d1 && phiOUT<=d2)   uoutput3[jjj]=1.0f;
						if (mOUT>d1   && mOUT<=d2)     uoutput4[jjj]=1.0f;
					}

					for (int kk=0; kk<num_input; kk++)  {
                                                       dataset1->input[nn][kk] =uinput[kk];
                                                       dataset2->input[nn][kk] =uinput[kk];
                                                       dataset3->input[nn][kk] =uinput[kk];
                                                       dataset4->input[nn][kk] =uinput[kk];
                                        }

 
					for (int kk=0; kk<num_output; kk++) {
                                                        dataset1->output[nn][kk] =uoutput1[kk];
                                                        dataset2->output[nn][kk] =uoutput2[kk];
                                                        dataset3->output[nn][kk] =uoutput3[kk];
                                                        dataset4->output[nn][kk] =uoutput4[kk];
                                        }

                                        // debug output 
                                        //cout <<  " " << endl;
                                        //for (int jjj=0; jjj<nBinsNN-1; jjj++) {cout << uoutput1[jjj] << " ";}
                                        //cout <<  " " << endl;

					h_in1->Fill(ptIN);
					h_in2->Fill(etaIN);
					h_in3->Fill(phiIN);
					h_in4->Fill(massIN);

					h_out1->Fill(ptOUT);
					h_out2->Fill(etaOUT);
					h_out3->Fill(phiOUT);
					h_out4->Fill(mOUT);

					nn++;
				} // end energy range


			} // end over data

                        double mse=0;
			double mse1=0;
                        double mse2=0; 
                        double mse3=0;
                        double mse4=0;
			for (int e=0; e<nEpoch; e++) {
                          mse1 = num_threads > 1 ? fann_train_epoch_irpropm_parallel(ann1_jets[m], dataset1, num_threads) : fann_train_epoch(ann1_jets[m], dataset1);
                          mse2 = num_threads > 1 ? fann_train_epoch_irpropm_parallel(ann2_jets[m], dataset2, num_threads) : fann_train_epoch(ann2_jets[m], dataset2);
                          mse3 = num_threads > 1 ? fann_train_epoch_irpropm_parallel(ann3_jets[m], dataset3, num_threads) : fann_train_epoch(ann3_jets[m], dataset3);
                          mse4 = num_threads > 1 ? fann_train_epoch_irpropm_parallel(ann4_jets[m], dataset4, num_threads) : fann_train_epoch(ann4_jets[m], dataset4);

                          mse=mse1+mse2+mse3+mse4;  
    			  if (e%100==0 ||  (e<10) || (e%20==0)) cout << " epoch=" << e << " MSE=" << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << mse1 << ", " <<  mse2 << ", " << mse3 << ", " << mse4 << " tot=" << mse << endl;
			  if (mse<MSESTOP) break;
			}
			cout << "  Bin=" << m << " with " << eventsJetBins[m] << " events has total MSE=" <<  mse  << " after epoch Nr=" << nEpoch << endl;


			fann_destroy_train(dataset1) ; // clear
                        fann_destroy_train(dataset2) ; // clear
                        fann_destroy_train(dataset3) ; // clear
                        fann_destroy_train(dataset4) ; // clear

			// empty data for feature NN 
			fann_train_data *  dataset_eff=  fann_create_train(eventsJetBins[m], num_input_eff, num_output_eff);
			nn=0;
    			cout << "\n  -> Jets: Training for features  = " << m << " sample size=" << finput_jets_eff.size() << endl;
			for (unsigned int i=0; i<finput_jets_eff.size(); i++){
				//cout << i << endl;
				vector<float> input2 = finput_jets_eff[i];
				vector<float> output2 =  foutput_jets_eff[i];
				float ptT=input2[0];
				float etaT=input2[1];
				float phiT=input2[2];
                                float btagT=(float)input2[3]; // 0 - 1000 
                                // outputs
				float match=output2[0];
                                float btag=output2[1];
				h_out5->Fill(match);
                                h_out6->Fill(btag); // -1 or 1 

				if (ptT>dmin && ptT<dmax) {
				        float dminmax=dmin+0.5*width;
                                        float ptIN=((ptT-dminmax)/(0.5*width));
					float etaIN=etaT/EtaMax; // range -1 -1
					float phiIN=phiT/PhiMax; // range -1-1 from -pi - pi
					fann_type uinput[num_input_eff];
					fann_type uoutput[num_output_eff];
					uinput[0]=ptIN;
					uinput[1]=etaIN;
					uinput[2]=phiIN;
                                        uinput[3]=(float)((btagT/500.) - 1); // btag normalize (-1,1)  
                                        if (uinput[3]>1.0f) uinput[3]=1.0f;
                                        uinput[4]=0; // reserved for charge

                                       // sliced input for NN
                                        float etaINSlice[slices_etaphi-1];
                                        // bin the resolution plots
                                        for (int jjj=0; jjj<slices_etaphi-1; jjj++) {
                                                float d1=-EtaMax+jjj*slicesEta;
                                                float d2=d1+slicesEta;
                                                //float dmm=d1+0.5*slicesEta;
                                                if (etaT>d1  && etaT<=d2) etaINSlice[jjj]=1.0f; //    etaINSlice[jjj]=(etaT-dmm)/(0.5*dmm);
                                                else etaINSlice[jjj]=0;
                                        }

                                        float phiINSlice[slices_etaphi-1];
                                        for (int jjj=0; jjj<slices_etaphi-1; jjj++) {
                                                float d1=-PhiMax+jjj*slicesPhi;
                                                float d2=d1+slicesPhi;
                                                //float dmm=d1+0.5*slicesPhi;
                                                if (phiT>d1  && phiT<=d2) phiINSlice[jjj]=1.0f; //    phiINSlice[jjj]=(phiT-dmm)/(0.5*dmm);
                                                else phiINSlice[jjj]=0;
                                        }


                                        // eta and phi are sliced for ANN
                                        // this is needed to reproduce spacial defects 
                                        int shift=num_kin;
                                        int kshift=0;
                                        for (int jjj=0; jjj<slices_etaphi-1; jjj++) {
                                                uinput[shift+kshift] =  etaINSlice[jjj];
                                                kshift++;
                                        }
                                        shift=shift+slices_etaphi-1;
                                        kshift=0;
                                        for (int jjj=0; jjj<slices_etaphi-1; jjj++) {
                                                uinput[shift+kshift] =  phiINSlice[jjj];
                                                kshift++;
                                        }


                                        //debug input 
                                        //cout <<  " " << endl;
                                        //for (int jjj=0; jjj<num_input_eff; jjj++) {cout << uinput[jjj] << " ";}
                                        //cout <<  " " << endl;


                                        // outputs
					uoutput[0]=(float)match;
                                        uoutput[1]=(float)btag;

                                        // debug
                                        //if (uoutput[0]>0 && btag>0) cout << "In " << uinput[3]  << "  out=" << uoutput[1] << endl;

					for (unsigned int kk=0; kk<num_input_eff; kk++)  dataset_eff->input[nn][kk] =uinput[kk];
					for (unsigned int kk=0; kk<num_output_eff; kk++)  dataset_eff->output[nn][kk] =uoutput[kk];

					nn++;
				} // end energy range
			}// end of dataset

			for (int e=0; e<nEpoch*2; e++) {
                            float mmse = num_threads > 1 ? fann_train_epoch_irpropm_parallel(ann5_jets[m], dataset_eff, num_threads) : fann_train_epoch(ann5_jets[m], dataset_eff);

			     if (e%100==0 || (e<10)) cout << "jet: bin=" << m << " epoch=" << e << " MSE=" << mmse << endl;
		             if (mse<MSESTOP) break;
			}
			fann_destroy_train(dataset_eff) ; // clear


		} // end run over energy bins



		cout << "Jets: ###  MSE for this batch after " << nevv << endl;
		for (int m=0; m<nBins-1; m++) eventsJetBins[m] =0;


		double sumMSE=0;
		for (int m=0; m<nBins-1; m++){
			double mse=fann_get_MSE(ann1_jets[m]);
			cout << "Event=" << nevv << " Bin=" << m << "  MSE=" << mse << endl;
			if (mse<1000) sumMSE=sumMSE+mse;
		};
		cout << "Jets: ## Total MSE for all bins for this batch " << sumMSE << endl;
		// batch if
		finput_jets.clear();
		foutput_jets.clear();
		finput_jets_eff.clear();
		foutput_jets_eff.clear();

	}


	return 0;

}

