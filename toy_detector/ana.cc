/***************************************************************************
 *  How to use ProMC files from HepSim, and how to build anti-KT jets 
 *  S.Chekanov (ANL) chekanov@anl.gov
 *  A library for HEP events storage and processing based on Google's PB   
 *  The project web site: http://atlaswww.hep.anl.gov/hepsim/
****************************************************************************/

#include<iostream>
#include<fstream>
#include<stdlib.h>
#include <sstream>      // std::stringstream

// check directory
#include <sys/stat.h>
#include"time.h"
#include <dirent.h>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>

// check directory
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1D.h>
#include <TH2D.h>
#include "TMath.h"
#include"time.h"
#include <dirent.h>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <TProfile2D.h>

//#include "doublefann.h"
//#include "fann_cpp.h"
#include "fann.h"
#include "parallel_fann.h"

using namespace std;

const double kPI   = TMath::Pi();
const double k2PI  = 2*kPI;

extern double RMS90(TH1*);
int findCeil(int  arr[], int r, int l, int h);
int myRand(int arr[], int freq[], int n);

// main example
int main(int argc, char **argv)
{




	// how many slices assume 200 slices
	const int maxSlice=100;


	cout << "\n\n start calculations with " << maxSlice << " slices" << endl;

	if (argc != 2) {
		cerr << " Unexpected number of command-line arguments. \n Set: train or run"
		<< " Program stopped! " << endl;
		return 1;
	}

	string rtype("-");
	rtype = argv[1];


	// before doing anything, read train data and shuffle
	const char* trainFile="data/train1.data";
	cout << "Read file: " << trainFile << endl;

	std::vector<int> ints;
	ifstream infile(trainFile);
	if (infile.good())
	{
		string sLine;
		getline(infile, sLine);
		//cout << sLine << endl;
		//convert to a stream
		std::stringstream in(  sLine  );
		copy( std::istream_iterator<int, char>(in), std::istream_iterator<int, char>(), back_inserter( ints ) );
	}
	int Nsample=ints[0];
	int inNodes=ints[1];
	//int outNodes=ints[2];
	infile.close();
	cout << "Nr of events in trained sample=" << Nsample << endl;
	cout << "Nr of input nodes in the matrix=" << inNodes << endl;
	//cout << "Nr of output nodes in the matrix=" << outNodes << endl;
	//exit(0);

	// read data for training..
	struct fann_train_data * dataTrain = fann_read_train_from_file(trainFile);
	// we have already scaled data using CM energy
	//fann_scale_input_train_data(dataTrain, 0, 1);
	cout << "Shuffle: " << trainFile << endl;
	fann_shuffle_train_data(dataTrain);



	cout << "calculate min and max for differences" << endl;

	double cmin = 1e10;  double cmax = -1e10;

        int totalEvents = dataTrain-> num_data;

	for (unsigned int nn=0; nn<totalEvents; nn++) {
		fann_type** output = dataTrain->output;
		float v1=output[nn][0];
		if (v1 < cmin) cmin = v1;
		if (v1 > cmax) cmax = v1;
		//cout << v1 << endl;
		//cout <<  nn << " " << dataTrain->num_input << endl;
	}




	long before;
	unsigned int num_threads = 16;
	const unsigned int num_input = inNodes;
	const unsigned int num_output = maxSlice+1; // plus efficiency
	const unsigned int num_layers = 3;
	// numeber in hidden layer 1
	const unsigned int num_neurons_hidden_1=20; // (int)(inNodes/2.0);
	// number in hidden layer 2
	const unsigned int num_neurons_hidden_2=(int)(inNodes/4.0);

	const double desired_error = (const float) 0.005;
	const unsigned int max_epochs = 40;
	const unsigned int epochs_between_reports = 10;
	// NN name
	const char* nn_name="nn_out/neural_final.net";


        string outputfile="data/input.root";
        if (rtype == "run") {
                string outputfile="data/output.root";
        }
        cout << "\n -> Output file is =" << outputfile << endl;
        TFile * RootFile = new TFile(outputfile.c_str(), "RECREATE", "Histogram file");

	TH1D * recOVtrue= new TH1D("rec-true", "rec-true",maxSlice,cmin,cmax);
	TH1D * input1eff= new TH1D("input_eff", "input_eff",2,0,2);

	for (unsigned int nn=0; nn<dataTrain-> num_data; nn++) {
		fann_type** output = dataTrain->output;
		float v1=output[nn][0];
		float v2=output[nn][1];
		if (v2>0) recOVtrue->Fill(v1); // fill rec-true
		input1eff->Fill(v2); // fill efficiency

		//cout <<  nn << " " << dataTrain->num_input << endl;
	}


	cout << "#####  Rec-true distribution:" << endl;
	double mean=recOVtrue->GetMean();
	double rms=recOVtrue->GetRMS();
	double rms90=RMS90( recOVtrue );
	cout << "  mean=" << mean << endl;
	cout << "  RMS=" << rms << endl;
	cout << "  RMS90=" << rms90 << endl;
	cout << "  min and max " << cmin << " - " << cmax << endl;
	// redefine ranges based on RMS
	double Xmin=mean-3*rms;
	double Xmax=mean+3*rms;
	cout << "  Used min and max " << Xmin << " " << Xmax << endl;

	double mean_eff=input1eff->GetMean();
	cout << "  efficiency mean=" << mean_eff << endl;


	TH1D * input1res= new TH1D("input_res", "input_res",maxSlice,Xmin,Xmax);

	double del=(Xmax - Xmin) / maxSlice;
	cout << "  Used step= " << del << endl;


        // keep binning for resolution  
        int  BinOverTrue[maxSlice-1];
        for (int jjj=0; jjj<maxSlice-1; jjj++) {
                             float d1=Xmin+jjj* del;
                             BinOverTrue[jjj]=(int)d1; 
                        }

// ***********************************************************************
// ---------------------- this part for training -------------------------
// ***********************************************************************
	if (rtype == "train") {

		// rebuild train data
		// empty data
		fann_train_data *  dataset1=  fann_create_train(totalEvents, num_input, num_output);
		fann_type** input = dataTrain->input;
		fann_type** output = dataTrain->output;

		for (unsigned int nn=0; nn<totalEvents; nn++){


			for (int kk=0; kk<num_input; kk++)  dataset1->input[nn][kk] =input[nn][kk];

			//float in=input[nn][0]; // 1st variable is active input
			float Slice[maxSlice-1];
			float v1=output[nn][0]; // this one is difference rec-true
			float v2=output[nn][1]; /// this is efficiency 0 or 1 

			for (int jjj=0; jjj<maxSlice-1; jjj++) {
				float d1=Xmin+jjj* del;
				float d2=d1+del;
				if (v1>d1  && v1<=d2) Slice[jjj]=1.0f;
				else Slice[jjj]=0;
                                if (v2<0) Slice[jjj]=0;
			}


			// check the histogram
			for (int jjj=0; jjj<maxSlice-1; jjj++) {
				float d1=Xmin+jjj* del;
				input1res->Fill(d1+0.5*del,Slice[jjj]);
			}

                        // prepare new output for NN (resolution)
			for (int kk=0; kk<maxSlice-1; kk++)  dataset1->output[nn][kk] =Slice[kk];
			// efficiency value is unchanged
			dataset1->output[nn][num_output-1] =v2;

		}

		cerr << "  input layer:      " << num_input << " units" << endl;
		cerr << "  hidden layer 1:   " << num_neurons_hidden_1 << "  units" << endl;
		//cerr << "  hidden layer 2:   " << num_neurons_hidden_2 << "  units" << endl;
		cerr << "  output layer:     " << num_output << "  units" << endl;

		struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden_1, num_output);


		// scale input. Output does not need to be scaled
		fann_scale_input_train_data(dataset1, 0, 1.0);
		//   fann_set_scaling_params(dataset1, ann, -1.0, 1.0, -1.0, 1.0);



		//struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden_1, num_neurons_hidden_2, num_output);
		//struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
		//fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
		//fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
		//fann_set_activation_function_hidden(ann, FANN_SIGMOID);
		//fann_set_activation_function_output(ann, FANN_SIGMOID);

		fann_set_activation_function_hidden(ann, FANN_SIGMOID);
		fann_set_activation_function_output(ann, FANN_SIGMOID);

		//fann_set_activation_function_output(ann, FANN_SIGMOID_STEPWISE);
		//fann_set_activation_function_hidden(ann, FANN_SIGMOID_STEPWISE);


		fann_randomize_weights(ann,0,1.0);
		//fann_print_connections(ann);
		fann_print_parameters(ann);

		cout << "Total number of neurons=" << fann_get_total_neurons(ann) << endl;
		cout << "Total number of connections=" << fann_get_total_connections(ann) << endl;
		cout << "-> Training using " << num_threads << " threads" << endl;

		cout << "Write output.d: p efficiency purity " << endl;
		std::ofstream out_error("nn_out/training_mse.d");
		out_error << "# epoch  training-MSE validation-MSE" << endl;


		std::vector<double> last_val_errors;
		double last_val_error=10000;
		double val_error=last_val_error;
		double train_error=100000;
		for(unsigned int i = 1 ; i <= max_epochs ; i++) {
			//fann_train_epoch(ann, dataTrain);

			// parallel
			train_error = num_threads > 1 ? fann_train_epoch_irpropm_parallel(ann, dataset1, num_threads) : fann_train_epoch(ann, dataset1);


			double delta=last_val_error-train_error;
			if (i%epochs_between_reports==0 || i<epochs_between_reports) {
				train_error = fann_test_data(ann, dataset1);
				// val_error = fann_test_data(ann, dataTrainVal);
				cout << "# epoch=" << i << " MSE=" << train_error << " Delta=" << delta << endl;
				// out_error << i << " " << train_error << " " << val_error << endl;
			}

			if (train_error<desired_error) break;

			if (i%50==0) {
				last_val_errors.push_back(val_error);
				auto smallest = std::min_element(last_val_errors.begin(), last_val_errors.end());
				last_val_error = (*smallest);
				val_error = fann_test_data(ann, dataset1);
				if ( val_error > last_val_error +0.001 ) {
					cout << "Step! MSE too large assuming cross validation. MSE=" << val_error << " Last MSE=" <<  last_val_error << endl;
					break;
				}
				cout << "    -> cross valid MSE=" << val_error << " Last min valid MSE=" << last_val_error << " out of N checks=" << last_val_errors.size() << endl;
			}



			// save every 100 epoch
			if (i%50==0 && i>1) {
				stringstream ss;
				ss << i;
				string str = "nn_out/neural_"+ss.str()+".net";
				cout << "Save intermidiate result to " << str << endl;
				fann_save(ann, str.c_str());
			}

			// stop learning?
			/*
			if (delta<1e-06 && i>epochs_between_reports) {
			               cout << "Current error=" << train_error << " last error=" << last_val_error << " delta=" << delta << endl;
			               cout << "Exit! Stop learning.." << endl;
			               break;
			               }
			*/

			if (i%epochs_between_reports==0) last_val_error=train_error;

			//if ( val_error > last_val_error )
			//    break;
			//last_val_error = val_error;
		}

		out_error.close();

		double mse=fann_get_MSE(ann);
		cout << "Final MSE error=" << mse <<endl;
		cout << "End. Save to: " << nn_name << endl;
		fann_save(ann,  nn_name);
		fann_destroy(ann);


		RootFile->Write();
		RootFile->Print();
		RootFile->Close();
		cout << "Writing ROOT file "+ outputfile << endl;
		return 0;


	}




	if (rtype == "run") {


                ofstream myfile;
                myfile.open ("data/neuralnet.data"); // output file 

		TH1D * out1res= new TH1D("output_res", "output_res",maxSlice,Xmin,Xmax);
		TH1D * out1eff= new TH1D("output_eff", "output_eff",100,0,1.0);

		cout << endl << "Testing network: open " << nn_name << endl;
		struct fann *ann_new = fann_create_from_file(nn_name);
		const char* testFile="data/valid1.data";
		cout << "Read test: " << testFile << endl;
		struct fann_train_data *data = fann_read_train_from_file( testFile );
                // write header file
                myfile << data->num_data << " " << data->num_input << " " << data->num_output << "\n";

                // for NN, scale it
		fann_scale_input_train_data(data, 0, 1.0);


		//cout << "..randomize data.. " << endl;
		//fann_shuffle_train_data(data);

		int totalEvents = data-> num_data;
		//fann_type** input = data->input;
		//fann_type** output = data->output;

                // read data again (original, no scaling)
                struct fann_train_data *original = fann_read_train_from_file( testFile );
                fann_type** original_input = original->input;

		for (int m=0; m<totalEvents; m++){

			//fann_scale_input( ann_new, data->input[m] );

                        int INSlice[maxSlice-1];

                        // write original 
                        for (int kk=0; kk<data->num_input; kk++) myfile <<  original_input[m][kk] << " "; 
                        myfile << "" << endl;

			fann_type * output1 = fann_run(ann_new, data->input[m]);
			for (int jjj=0; jjj<maxSlice-1; jjj++) {
				// cout << output1[jjj] << endl;
				float d1=Xmin+jjj* del;
				//float d2=d1+del;
                                INSlice[jjj]=(int)( output1[jjj]*1000000 ); // convert to int 
				out1res->Fill(d1+0.5*del,(double)INSlice[jjj]); //  
			}

                        float true_value=original_input[m][0];
                        int jjj=maxSlice; // efficiency
                        out1eff->Fill(output1[jjj]);


                        //cout << "efficiency =" <<  output1[jjj] << endl;

                        float isExist=1;
                        // efficiency. Make randon (0-1) 
                        float r = ((double) rand() / (RAND_MAX)); 
                        if (r<(1-output1[jjj]))  isExist=0;

                       //cout << r << "  " << output1[jjj] << endl;

                        float reco_value=0;
                        if (isExist>0) {
                          int BinSelected=myRand(BinOverTrue, INSlice, maxSlice-1); // select random value (bin) assuming frequencies
                          reco_value=BinSelected;
                        }

                        myfile <<  reco_value  << " " <<   isExist  << endl;


			/*
			           float Slice[maxSlice-1];
			           float v1=output[nn][0];
			           float v2=output[nn][1];
			           for (int jjj=0; jjj<maxSlice-1; jjj++) {
			               float d1=cmin+jjj* del;
			               float d2=d1+del;
			               if (v1>d1  && v1<=d2) Slice[jjj]=1.0f;
			               else Slice[jjj]=0;
			            }
			*/

		}



		cout << "\n#### Predicted Rec-true distribution:" << endl;
		double mean_1=out1res->GetMean();
		double rms_1=out1res->GetRMS();
		double rms90_1=RMS90( out1res );
		cout << "  mean=" << mean_1 << " true=" << mean << endl;
		cout << "  RMS=" << rms_1 << " true=" << rms << endl;
		cout << "  RMS90=" << rms90_1 << " true = " << rms90 << endl;
		double mean_2eff=out1eff->GetMean();
		cout << "  efficiency mean=" << mean_2eff  << " true=" << mean_eff << endl;


                myfile.close();
		RootFile->Write();
		RootFile->Print();
		RootFile->Close();
		cout << "Writing ROOT file "+ outputfile << endl;




	}



	return 0;
}
