// S.Chekanov (ANL)

#include "Ana.h"

Int_t Ana::AnalysisJets(vector<LParticle> JetsTrue, vector<LParticle> JetsReco) {

	JetsFastNN.clear();

	// NN jets
	m_nnjetpt.clear();
	m_nnjeteta.clear();
	m_nnjetphi.clear();
	m_nnjetm.clear();
	m_nnjetbtag.clear();

	// true jets
	m_gjetpt.clear();
	m_gjeteta.clear();
	m_gjetphi.clear();
	m_gjetm.clear();
	m_gjetbtag.clear();

	// reco jets
	m_jetpt.clear();
	m_jeteta.clear();
	m_jetphi.clear();
	m_jetm.clear();
	m_jetbtag.clear();

	// matched jets
	m_matchedjetpt.clear();
	m_matchedjeteta.clear();
	m_matchedjetphi.clear();
	m_matchedjetm.clear();
	m_matchedjetbtag.clear();

	const double EtaMax=maxEta;
	const double PhiMax=PI;
	const double intmove=10000; // number for converting to integer frequencies
	const double delta=2.0/nBinsNN;
	const double slicesEta=(2*EtaMax)/slices_etaphi; // slices in eta
	const double slicesPhi=(2*PhiMax)/slices_etaphi; // slices in phi

	//TRandom2 Rphi;

	// fill RECO jets
	for(unsigned int j = 0; j<JetsReco.size(); j++){
		LParticle rjet = (LParticle)JetsReco.at(j);
		TLorentzVector L2 = rjet.GetP();
		double phi = L2.Phi();
		double pt =  L2.Perp();
		double eta = L2.PseudoRapidity();
		double mass =  L2.M();
		int    btag =  rjet.GetType();

		if (pt>minPT) {
			m_jetpt.push_back(pt);
			m_jeteta.push_back( eta );
			m_jetphi.push_back( phi  );
			m_jetm.push_back( mass  );
			m_jetbtag.push_back( btag  );
		}

	}


	for(unsigned int j = 0; j<JetsTrue.size(); j++){
		LParticle tjet = (LParticle)JetsTrue.at(j);
		TLorentzVector L2 = tjet.GetP();

		float phiT = L2.Phi();
		float ptT =  L2.Perp();
		float etaT = L2.PseudoRapidity();
		float massT = L2.M();
		float btagT = (float)tjet.GetType(); // fraction of b-quark momenta in 10*100%
                float jetrT=(float)(tjet.GetCharge()/10000.0); // get jet radius 
                float jetE=L2.E(); // energy 
                jetrT =  jetrT / sqrt(jetE); // effective jet radius in GeV-1/2 (sqrt to increase values) 

		if (ptT>minPT && abs(etaT)<maxEta) {
			m_gjetpt.push_back(ptT);
			m_gjeteta.push_back(etaT);
			m_gjetphi.push_back( phiT );
			m_gjetm.push_back( massT );
			if (btagT>300) m_gjetbtag.push_back( 1 ); // assume 30% fraction for b-tagging 
			else           m_gjetbtag.push_back( 0 );
		}

		h_jetcutflow->Fill(1);


		// reco jets
		if (ptT>minPT && abs(etaT)<maxEta) { // only for the pT cut
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

			// only matched jets
			if (indexMatch>-1){
				LParticle rjet = (LParticle)JetsReco.at(indexMatch);
				TLorentzVector L1 = rjet.GetP();
				m_matchedjetpt.push_back(  L1.Perp() );
				m_matchedjeteta.push_back( L1.PseudoRapidity() );
				m_matchedjetphi.push_back( L1.Phi()  );
				m_matchedjetm.push_back(  L1.M()  );
				m_matchedjetbtag.push_back( rjet.GetType() );
			}

		} // end cut on pT and Eta

		// cout << "True jet=" << j << " " << ptT << " " << etaT << endl;

		// corrected kinematics
		float pt=ptT;
		float eta=etaT;
		float phi=phiT;
		float mass=massT;
		double btagFound=-1.0;

		//if (phiT<0) phiT=abs(phiT)+PI;
		//if (phi<0) phi=abs(phi)+PI;

		for (int m=0; m<nBins-1; m++){
			double dmin=eBins[m];
			double dmax=eBins[m+1];
			double width=dmax-dmin;

			if (ptT>dmin && ptT<=dmax) {
				// Range [0-1]. 0.5 - no difference; 0 - missed reco
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
					if (etaT>d1  && etaT<=d2) etaINSlice[jjj]=1.0f; //     etaINSlice[jjj]=(etaT-dmm)/(0.5*dmm);
					else etaINSlice[jjj]=0;
				}

				float phiINSlice[slices_etaphi-1];
				for (int jjj=0; jjj<slices_etaphi-1; jjj++) {
					float d1=-PhiMax+jjj*slicesPhi;
					float d2=d1+slicesPhi;
					//float dmm=d1+0.5*slicesPhi;
					if (phiT>d1  && phiT<=d2)  phiINSlice[jjj]=1.0f; //    phiINSlice[jjj]=(phiT-dmm)/(0.5*dmm);
					else phiINSlice[jjj]=0;
				}

				fann_type uinput[num_input];

				uinput[0] = ptIN;
				uinput[1] = massIN;
				uinput[2] = etaIN;
				uinput[3] = phiIN;
                                uinput[4] = jetrT; 

				// eta and phi are sliced for ANN
				// this is needed to reproduce spacial defects
				int shift= num_kin;
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


				fann_type * output1 = fann_run(ann1_jets[m], uinput);

				double scale=0;
				// output of NN can have negaive values. So we add scale to fix those.
				//int freqPT[nBinsNN];
				//for (int jjj=0; jjj<nBinsNN; jjj++) freqPT[jjj]=(int)((scale+output[jjj])*intmove);
				//debug:
				//for (unsigned int jjj=0; jjj<nBinsNN; jjj++) cout << freqPT[jjj] << " ";
				//cout << endl;


				// you can use Boost but it is slower
                                /*
				vector<double> freqPT;
                                // create probability distribution
                                double xsum=0; 
                                for (int jjj=0; jjj<nBinsNN-1; jjj++) xsum=xsum+output[jjj];
				for (int jjj=0; jjj<nBinsNN-1; jjj++) freqPT.push_back( (double)(output[jjj]/xsum));
				boost::random::discrete_distribution<> dist1(freqPT);
				int BinSelected = dist1(gen);
				//cout << BinSelected << endl;
                                */

                                int freqPT[nBinsNN-1];
                                for (int jjj=0; jjj<nBinsNN-1; jjj++) freqPT[jjj]=(int)((scale+output1[jjj])*intmove);
				int BinSelected=myRand(BinOverTrue, freqPT, nBinsNN-1); // select random value (bin) assuming frequencies from freqPT
				double recoOvertrue=-1+BinSelected*delta;
				double ptcor= ( (recoOvertrue - jet_eshift)/jet_escale )+1.0;
				pt =  ptT *ptcor; // gain
				//cout << "true=" << ptT << " reco=" << pt <<  " Corr=" << ptcor << " selected bin=" << BinSelected << endl;
				h_ptcor->Fill( ptcor );
				h_rout1_jet->Fill( (float)BinSelected );

				// unpack the outputs for pT
				//cout << "\n New jet:" << endl;
				for (int jjj=0; jjj<nBinsNN-1; jjj++) {
					double d1=-1.0+jjj*delta;
                                        if (isnan(output1[jjj])) {
                                           cout << "ANN1: found NAN in bin " << jjj << endl;
                                           for (unsigned int m1=0; m1<num_input; m1++) cout << "  " << m1<< " input " << uinput[m1] << endl;    
                                    }
					h_out1_jet->Fill( d1+0.5*delta,output1[jjj]);
				}


				// Eta
				fann_type * output2 = fann_run(ann2_jets[m], uinput);
				int freqETA[nBinsNN-1];
				for (int jjj=0; jjj<nBinsNN-1; jjj++) freqETA[jjj]=(int)((scale+output2[jjj])*intmove);
				BinSelected=myRand(BinOverTrue, freqETA, nBinsNN-1); // select random value (bin) assuming frequencies
                               
                                /* slow boost 
                                vector<double> freqETA;
                                xsum=0;
                                for (int jjj=0; jjj<nBinsNN-1; jjj++) xsum=xsum+output[jjj];
                                for (int jjj=0; jjj<nBinsNN-1; jjj++) freqETA.push_back( (double)(output[jjj]/xsum));
                                boost::random::discrete_distribution<> dist2(freqETA);
                                BinSelected = dist2(gen);
                                */
				h_rout2_jet->Fill( (float)BinSelected );
				recoOvertrue=-1+BinSelected*delta;
				double etacor= ( (recoOvertrue - jet_etashift)/jet_etascale )+1.0;
				h_etacor->Fill( etacor );
				eta =  etaT * etacor;
				for (int jjj=0; jjj<nBinsNN-1; jjj++) {
					double d1=-1.0+jjj*delta;
					h_out2_jet->Fill( d1+0.5*delta, output2[jjj]);
				}

				// Phi
				fann_type * output3 = fann_run(ann3_jets[m], uinput);
				int freqPHI[nBinsNN-1];
				for (int jjj=0; jjj<nBinsNN-1; jjj++) freqPHI[jjj]=(int)((scale+output3[jjj])*intmove);
				BinSelected=myRand(BinOverTrue, freqPHI, nBinsNN-1); // select random value (bin) assuming frequencies
                                /*
                                vector<double> freqPHI;
                                xsum=0;
                                for (int jjj=0; jjj<nBinsNN-1; jjj++) xsum=xsum+output[jjj];
                                for (int jjj=0; jjj<nBinsNN-1; jjj++) freqPHI.push_back( (double)(output[jjj]/xsum));
                                boost::random::discrete_distribution<> dist3(freqPHI);
                                BinSelected = dist3(gen);
                                */

				h_rout3_jet->Fill( (float)BinSelected );
				recoOvertrue=-1+BinSelected*delta;
				double phicor= ( (recoOvertrue - jet_etashift)/jet_etascale )+1.0;
				h_phicor->Fill( phicor );
				phi =  phiT * phicor;
				for (int jjj=0; jjj<nBinsNN-1; jjj++) {
					double d1=-1.0+jjj*delta;
					h_out3_jet->Fill( d1+0.5*delta, output3[jjj]);
				}


				// mass
				fann_type * output4 = fann_run(ann4_jets[m], uinput);
				int freqM[nBinsNN-1];
				for (int jjj=0; jjj<nBinsNN-1; jjj++) freqM[jjj]=(int)((scale+output4[jjj])*intmove);
				BinSelected=myRand(BinOverTrue, freqM, nBinsNN-1); // select random value (bin) assuming frequencies
                                /* boost 
                                vector<double> freqM;
                                xsum=0;
                                for (int jjj=0; jjj<nBinsNN-1; jjj++) xsum=xsum+output[jjj];
                                for (int jjj=0; jjj<nBinsNN-1; jjj++) freqM.push_back( (double)(output[jjj]/xsum));
                                boost::random::discrete_distribution<> dist4(freqM);
                                BinSelected = dist4(gen);
                                */
				h_rout4_jet->Fill( (float)BinSelected );
				recoOvertrue=-1+BinSelected*delta;
				// correction
				double mcorr=( (recoOvertrue -  jet_mshift)/jet_mscale ) + 1.0;
				mass =  massT * mcorr; // gain
				for (int jjj=0; jjj<nBinsNN-1; jjj++) {
					double d1=-1.0+jjj*delta;
					h_out4_jet->Fill( d1+0.5*delta, output4[jjj]);
				}

				h_in1_jet->Fill(ptIN);
				h_in2_jet->Fill(etaIN);
				h_in3_jet->Fill(phiIN);
				h_in4_jet->Fill(massIN);
                                h_in5_jet->Fill(jetrT);

				if (phi>PI || phi<-PI) phi=phiT; // overcorrection!

                                // ----------------- feature NN starts here --------------------
                                // structure of input the same but mass is replaced with b-tagging
                                 uinput[0]=ptIN;
                                 uinput[1]=etaIN;
                                 uinput[2]=phiIN;
                                // structure of input the same but mass is replaced with b-tagging
                                 uinput[3] = (float)((btagT/500.) - 1); // normalize to (-1,1)
                                 if (uinput[3]>1.0f)  uinput[3]=1.0f;
                                 uinput[4]=jetrT; // effective jet size 


				fann_type * output5 = fann_run(ann5_jets[m], uinput);
				float prob_efficiency=output5[0];
				btagFound=(double)output5[1];
                                //cout << "B-tag NN input=" << uinput[3] << " btagT=" << btagT << " NN out=" << btagFound << " " << endl;

				h_out5_jet_eff->Fill(prob_efficiency);
				//if (prob_efficiency<-0.9) continue; // skip event since low efficiency
                                h_out6_jet_btag->Fill(btagFound);

			}
		}



                const float btagcut=-0.2; //  look at output of NN and find the feature peak!
		h_jetpt_nn->Fill(pt);

		TLorentzVector l;
		l.SetPtEtaPhiM(pt,eta,phi,mass);
		LParticle p;
		p.SetP(l);
		if (btagFound > btagcut) p.SetType(1);
		else p.SetType(0);

		JetsFastNN.push_back(p);
		h_jetcutflow->Fill(2.);


		if (pt>minPT) {
			m_nnjetpt.push_back(pt);
			m_nnjeteta.push_back(eta);
			m_nnjetphi.push_back(phi);
			m_nnjetm.push_back(mass);
                        int bt=0;
			if (btagFound > btagcut)  bt=1;
			m_nnjetbtag.push_back(bt);
                        // cout << btagFound << " " << bt << endl;
		}


		if (ptT>50)  h_jetres100->Fill(pt/ptT);
		if (ptT>200)  h_jetres1000->Fill(pt/ptT);

	}



	return 0;

}

