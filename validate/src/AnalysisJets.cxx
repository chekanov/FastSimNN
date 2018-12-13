// S.Chekanov (ANL)

#include "Ana.h"

#include <TRandom2.h>

//BOOST library
//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/discrete_distribution.hpp>
//boost::mt19937 gen;


const double PI   = TMath::Pi();
const double PI2  = 2*PI;
const double PIover2   = 0.5*PI;

TRandom2 Reta;


int findCeil(int  arr[], int r, int l, int h)
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
	return va;

}

// The main function that returns a random number from arr[] according to
// distribution array defined by freq[]. n is size of arrays.
int myRand(int arr[], int freq[], int n)
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

	return (int)(n/2.0); // assume no correction when error

}


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
	const double delta=2.0/(nBinsNN-1);
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

		double phiT = L2.Phi();
		double ptT =  L2.Perp();
		double etaT = L2.PseudoRapidity();
		double massT =  L2.M();
		float  btagT =  (float)tjet.GetType(); // fraction of b-quark momenta in 100%

		if (ptT>minPT && abs(etaT)<maxEta) {
			m_gjetpt.push_back(ptT);
			m_gjeteta.push_back(etaT);
			m_gjetphi.push_back( phiT );
			m_gjetm.push_back( massT );
			if (btagT>50) m_gjetbtag.push_back( 1 );
			else          m_gjetbtag.push_back( 0 );
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
		float btag=btagT;

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
					float dmm=d1+0.5*slicesEta;
					if (etaT>d1  && etaT<=d2)    etaINSlice[jjj]=(etaT-dmm)/(0.5*dmm);
					else etaINSlice[jjj]=0;
				}

				float phiINSlice[slices_etaphi-1];
				for (int jjj=0; jjj<slices_etaphi-1; jjj++) {
					float d1=-PhiMax+jjj*slicesPhi;
					float d2=d1+slicesPhi;
					float dmm=d1+0.5*slicesPhi;
					if (phiT>d1  && phiT<=d2)    phiINSlice[jjj]=(phiT-dmm)/(0.5*dmm);
					else phiINSlice[jjj]=0;
				}

				fann_type uinput[num_input];

				uinput[0] = ptIN;
				uinput[1] = massIN;
				uinput[2] = etaIN;
				uinput[3] = phiIN;

				// eta and phi are sliced for ANN
				// this is needed to reproduce spacial defects
				int shift=4;
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


				fann_type *output;

				output = fann_run(ann1_jets[m], uinput);

				double scale=0;
				// output of NN can have negaive values. So we add scale to fix those.
				//int freqPT[nBinsNN];
				//for (int jjj=0; jjj<nBinsNN; jjj++) freqPT[jjj]=(int)((scale+output[jjj])*intmove);
				//debug:
				//for (unsigned int jjj=0; jjj<nBinsNN; jjj++) cout << freqPT[jjj] << " ";
				//cout << endl;


				int freqPT[nBinsNN-1];
				for (int jjj=0; jjj<nBinsNN-1; jjj++) freqPT[jjj]=(int)((scale+output[jjj])*intmove);

				// you can use Boost but it is slower
				// vector<double> freqPT;
				//for (int jjj=0; jjj<nBinsNN; jjj++) freqPT.push_back( (double)(scale+output[jjj])*intmove) ;
				//boost::random::discrete_distribution<> dist(freqPT);
				//int BinSelected = dist(gen);
				//cout << BinSelected << endl;

				int BinSelected=myRand(BinOverTrue, freqPT, nBinsNN-1); // select random value (bin) assuming frequencies from freqPT
				double recoOvertrue=-1+BinSelected*delta;
				double ptcor= ( (recoOvertrue - jet_eshift)/jet_escale )+1.0;
				pt =  ptT *ptcor; // gain
				//cout << "true=" << ptT << " reco=" << pt <<  " Corr=" << ptcor << " selected bin=" << BinSelected << endl;
				h_ptcor->Fill( ptcor );
				h_rout1->Fill( (float)BinSelected );

				// unpack the outputs for pT
				//cout << "\n New jet:" << endl;
				for (int jjj=0; jjj<nBinsNN-1; jjj++) {
					double d1=-1.0+jjj*delta;
					h_out1->Fill( d1+0.5*delta, output[jjj]);
				}


				// Eta
				output = fann_run(ann2_jets[m], uinput);
				int freqETA[nBinsNN-1];
				for (int jjj=0; jjj<nBinsNN-1; jjj++) freqETA[jjj]=(int)((scale+output[jjj])*intmove);
				BinSelected=myRand(BinOverTrue, freqETA, nBinsNN-1); // select random value (bin) assuming frequencies

				h_rout2->Fill( (float)BinSelected );

				recoOvertrue=-1+BinSelected*delta;
				double etacor= ( (recoOvertrue - jet_etashift)/jet_etascale )+1.0;
				h_etacor->Fill( etacor );
				eta =  etaT * etacor;
				for (int jjj=0; jjj<nBinsNN-1; jjj++) {
					double d1=-1.0+jjj*delta;
					h_out2->Fill( d1+0.5*delta, output[jjj]);
				}

				// Phi
				output = fann_run(ann3_jets[m], uinput);
				int freqPHI[nBinsNN-1];
				for (int jjj=0; jjj<nBinsNN-1; jjj++) freqPHI[jjj]=(int)( (scale+output[jjj])*intmove);
				BinSelected=myRand(BinOverTrue, freqPHI, nBinsNN-1); // select random value (bin) assuming frequencies

				h_rout3->Fill( (float)BinSelected );

				recoOvertrue=-1+BinSelected*delta;
				double phicor= ( (recoOvertrue - jet_etashift)/jet_etascale )+1.0;
				h_phicor->Fill( phicor );
				phi =  phiT * phicor;
				for (int jjj=0; jjj<nBinsNN; jjj++) {
					double d1=-1.0+jjj*delta;
					h_out3->Fill( d1+0.5*delta, output[jjj]);
				}


				// mass
				output = fann_run(ann4_jets[m], uinput);
				int freqM[nBinsNN-1];
				for (int jjj=0; jjj<nBinsNN-1; jjj++) freqM[jjj]=(int)( (scale+output[jjj])*intmove);
				BinSelected=myRand(BinOverTrue, freqM, nBinsNN-1); // select random value (bin) assuming frequencies

				h_rout4->Fill( (float)BinSelected );

				recoOvertrue=-1+BinSelected*delta;
				// correction
				double mcorr=( (recoOvertrue -  jet_mshift)/jet_mscale ) + 1.0;
				mass =  massT * mcorr; // gain
				for (int jjj=0; jjj<nBinsNN-1; jjj++) {
					double d1=-1.0+jjj*delta;
					h_out4->Fill( d1+0.5*delta, output[jjj]);
				}

				h_in1->Fill(ptIN);
				h_in2->Fill(etaIN);
				h_in3->Fill(phiIN);
				h_in4->Fill(massIN);

				if (phi>PI || phi<-PI) phi=phiT; // overcorrection!


				// feature NN
				fann_type input_eff[num_input_eff];
				input_eff[0] = ptIN;
				input_eff[1] = etaIN;
				input_eff[2] = phiIN;
				input_eff[3] = (float)(btagT/100.) - 1; // normalize  -1 - 0
				if (input_eff[3]>1) input_eff[3]=1;

				output = fann_run(ann5_jets[m], input_eff);
				float prob_efficiency=output[0];
				btag=output[1];
				h_out5_eff->Fill(prob_efficiency);
				if (prob_efficiency<-0.8) continue; // skip event since low efficiency
				h_out6_btag->Fill(btag);


			}
		}


		h_jetpt_nn->Fill(pt);

		TLorentzVector l;
		l.SetPtEtaPhiM(pt,eta,phi,mass);
		LParticle p;
		p.SetP(l);
		if (btag>-0.75) p.SetType(1);
		else p.SetType(0);

		JetsFastNN.push_back(p);
		h_jetcutflow->Fill(2.);


		if (pt>minPT) {
			m_nnjetpt.push_back(pt);
			m_nnjeteta.push_back(eta);
			m_nnjetphi.push_back(phi);
			m_nnjetm.push_back(mass);
			if (btag>-0.75)  m_nnjetbtag.push_back(1);
			else m_nnjetbtag.push_back(0);
		}


		if (ptT>50)  h_jetres100->Fill(pt/ptT);
		if (ptT>200)  h_jetres1000->Fill(pt/ptT);

	}


	m_ntuple->Fill();



	return 0;

}

