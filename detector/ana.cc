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
// check directory
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1D.h>
#include <TH2D.h>
#include "TMath.h"
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
void print_traindata(fann_train_data * data, int row);


// main example
int main(int argc, char **argv)
{

	const bool debug=false;

	// how many slices assume slices to divide the phase space for each output 
	const int maxSlice=120;
	cout << "\n\nStart calculations with " << maxSlice << " output slices" << endl;
        // how many slices for input variables 
	const int maxInSlice=10;
	cout << "Start calculations with " << maxInSlice << " input slices" << endl;

	const double nnToFreq=1000000;
	cout << "NN to freq. conversion " << nnToFreq << " slices" << endl;

        const double desired_error = (const float) 0.001;
        const unsigned int max_epochs = 200;
        const unsigned int epochs_between_reports = 20;
        cout << "Max number of epoch " << max_epochs  <<  endl;


	if (argc != 2) {
		cerr << " Unexpected number of command-line arguments. \n Set: train or run"
		<< " Program stopped! " << endl;
		return 1;
	}

	string rtype("-");
	rtype = argv[1];

	// before doing anything, read train data and shuffle
	const char* trainFile="data/train.data";
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

	// read data for training..
	struct fann_train_data * dataTrain = fann_read_train_from_file(trainFile);
	cout << "Shuffle: " << trainFile << endl;
	fann_shuffle_train_data(dataTrain);

        const char* validFile="data/valid.data";
        cout << "Read cross validation sample : " << validFile << endl;
        struct fann_train_data *dataValid = fann_read_train_from_file( validFile );
        cout << "Shuffle: " << validFile << endl;
        fann_shuffle_train_data(dataValid);


	cout << "calculate min and max for differences" << endl;
	double cmin = 1e10;  double cmax = -1e10;
	int totalEvents = dataTrain-> num_data;
	int  numInput=dataTrain->num_input;

	// min and max for outputs
	for (int ev=0; ev<totalEvents; ev++) {
		fann_type** output = dataTrain->output;
		float v1=output[ev][0];
		if (v1 < cmin) cmin = v1;
		if (v1 > cmax) cmax = v1;
	}

	// min and max values of inputs to put to the grid
	double cminIN[numInput], cmaxIN[numInput];
        for (int jjj=0; jjj<numInput; jjj++) {cminIN[jjj]=1e10; cmaxIN[jjj]= -1e10;};
	for (int ev=0; ev<totalEvents; ev++) {
		fann_type** input = dataTrain->input;
		for (int jjj=0; jjj<numInput; jjj++) {
			float v1=input[ev][jjj];
			if (v1 < cminIN[jjj]) cminIN[jjj] = v1;
			if (v1 > cmaxIN[jjj]) cmaxIN[jjj] = v1;
		}
	}

	cout << "Min and Max values for input variables: " << endl;
	for (int jjj=0; jjj<numInput; jjj++)
		cout << "n=" << jjj << " min=" << cminIN[jjj] << " " << cmaxIN[jjj] << endl;

	long before;
	unsigned int num_threads = 16;
	const unsigned int num_input = numInput+numInput*(maxInSlice); // 4 original values + 4*maxInSlice grid;
	const unsigned int num_output = maxSlice+1; // Nr of layers = Nr of slices plus efficiency bin 
	const unsigned int num_layers = 3;
	// number in hidden layer 1
	const unsigned int num_neurons_hidden_1=(int)(num_input/1.5);
	// number in hidden layer 2
	const unsigned int num_neurons_hidden_2=(int)(num_input/2.0);

	// NN name
	const char* nn_name="nn_out/neural_final.net";

	string outputfile="data/input.root";
	if (rtype == "run") outputfile="data/output.root";
	cout << "\n -> Output file is =" << outputfile << endl;
	TFile * RootFile = new TFile(outputfile.c_str(), "RECREATE", "Histogram file");

	TH1D * recOVtrue= new TH1D("rec-true", "rec-true as read from input file",maxSlice,cmin,cmax);
	TH1D * input1eff= new TH1D("input_eff", "efficiency from input file",2,0,2);

	for (unsigned int ev=0; ev<dataTrain->num_data; ev++) {
		fann_type** output = dataTrain->output;
		float v1=output[ev][0];
		float v2=output[ev][1];
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
	const double Xmin=mean-3*rms;
	const double Xmax=mean+3*rms;
	cout << "  Used min and max " << Xmin << " " << Xmax << endl;

	double mean_eff=input1eff->GetMean();
	cout << "  efficiency mean=" << mean_eff << endl;

	TH1D * input1res= new TH1D("input_res", "rec-true after sliced resolution",maxSlice,Xmin,Xmax);

	const double del=(Xmax - Xmin) / (float)maxSlice;
	cout << "  Used step to bin resolution= " << del << endl;


	// keep binning for resolution
	int  BinOverTrue[maxSlice];
	for (int jjj=0; jjj<maxSlice; jjj++) {
		float d1=Xmin+jjj* del;
		BinOverTrue[jjj]=(int)d1;
	}

	// ***********************************************************************
	// ---------------------- this part for training -------------------------
	// ***********************************************************************
	if (rtype == "train") {

          fann_type** input = dataTrain->input;
          fann_type** output = dataTrain->output;

           // remove events outside  3*sigma
           int ntot=0;
           for (int ev=0; ev<totalEvents; ev++){
           float v1=output[ev][0]; // this one is difference rec-true
           float v2=output[ev][1]; // this is efficiency 0 or 1
           // do not consider events outside the range
           if (v1 < Xmin || v1>Xmax) continue; 
            ntot++;
            }
            totalEvents=ntot;


            // rebuild train data
	    // empty data
	    fann_train_data *  dataset1=  fann_create_train(totalEvents, num_input, num_output);

		for (int ev=0; ev<totalEvents; ev++){
			// slice output (1st variable only)
			float Slice[maxSlice];
			float v1=output[ev][0]; // this one is difference rec-true
			float v2=output[ev][1]; // this is efficiency 0 or 1
 
                        // do not consider events outside the range
                        if (v1 < Xmin || v1>Xmax) continue; 

			for (int jjj=0; jjj<maxSlice; jjj++) {
				float d1=Xmin+jjj* del;
				float d2=d1+del;
				if (v1>d1  && v1<=d2) Slice[jjj]=1.0f;
				else Slice[jjj]=0;
				if (v2==0) Slice[jjj]=0; // 0 if did not pass efficiency
			}
			// slice all input variables
			float InSlice[numInput][maxInSlice];
			for (int jj=0; jj<numInput; jj++) {
				float vv=input[ev][jj];
				float XM=cminIN[jj];
				float del2=(cmaxIN[jj] - cminIN[jj]) / (float)maxInSlice;
				for (int jjj=0; jjj<maxInSlice; jjj++) {
					float d1=XM+jjj* del2;
					float d2=d1+del2;
					if (vv>d1  && vv<=d2) InSlice[jj][jjj]=1.0f;
					else InSlice[jj][jjj]=0;
				}
			}



			// check the histogram
			for (int jjj=0; jjj<maxSlice; jjj++) {
				float d1=Xmin+jjj*del;
				input1res->Fill(d1+0.5*del,Slice[jjj]);
			}

			// prepare new output for NN (resolution)
			for (int kk=0; kk<maxSlice; kk++)  dataset1->output[ev][kk] =Slice[kk];
			// efficiency value is unchanged
			dataset1->output[ev][maxSlice] =v2;

			// prepare new input using grid
                        // normalize first 4 values
			for (int kk=0; kk<numInput; kk++)  dataset1->input[ev][kk]=(input[ev][kk]-cminIN[kk])/(cmaxIN[kk]-cminIN[kk]); // 4 original (rescaled) values
                        // deal witn the rest
			int kstart=numInput;
			for (int jj=0; jj<numInput; jj++) {
				for (int kk=0; kk<maxInSlice; kk++)  {
					dataset1->input[ev][kstart] =InSlice[jj][kk];
					kstart=kstart+1;
				}
			}
		}


          fann_type** inputV = dataValid->input;
          fann_type** outputV = dataValid->output;


           // remove events outside  3*sigma
           int ntotV=0;
           for (unsigned int ev=0; ev<dataValid->num_data; ev++){
           float v1=outputV[ev][0]; // this one is difference rec-true
           float v2=outputV[ev][1]; // this is efficiency 0 or 1
           // do not consider events outside the range
           if (v1 < Xmin || v1>Xmax) continue;
           ntotV++;
           }
           const int totalEventsV=ntotV;
            // rebuild validation data
            // empty data
            fann_train_data *  dataset1V=  fann_create_train(totalEventsV, num_input, num_output);


               // now rebuild  validation data set similarly..
                for (int ev=0; ev<totalEventsV; ev++){
                        // slice output (1st variable only)
                        float Slice[maxSlice];
                        float v1=outputV[ev][0]; // this one is difference rec-true
                        float v2=outputV[ev][1]; // this is efficiency 0 or 1

                        // do not consider events outside the range
                        if (v1 < Xmin || v1>Xmax) continue;


                        for (int jjj=0; jjj<maxSlice; jjj++) {
                                float d1=Xmin+jjj* del;
                                float d2=d1+del;
                                if (v1>d1  && v1<=d2) Slice[jjj]=1.0f;
                                else Slice[jjj]=0;
                                if (v2==0) Slice[jjj]=0; // 0 if did not pass efficiency
                        }
                        // slice all input variables
                        float InSlice[numInput][maxInSlice];
                        for (int jj=0; jj<numInput; jj++) {
                                float vv=inputV[ev][jj];
                                float XM=cminIN[jj];
                                float del2=(cmaxIN[jj] - cminIN[jj]) / (maxInSlice);
                                for (int jjj=0; jjj<maxInSlice; jjj++) {
                                        float d1=XM+jjj* del2;
                                        float d2=d1+del2;
                                        if (vv>d1  && vv<=d2) InSlice[jj][jjj]=1.0f;
                                        else InSlice[jj][jjj]=0;
                                }
                        }

                        // prepare new output for NN (resolution)
                        for (int kk=0; kk<maxSlice; kk++)  dataset1V->output[ev][kk] =Slice[kk];
                        // efficiency value is unchanged
                        dataset1V->output[ev][maxSlice] =v2;
                        // prepare new input using grid
                        for (int kk=0; kk<numInput; kk++)  dataset1V->input[ev][kk]=(inputV[ev][kk]-cminIN[kk])/(cmaxIN[kk]-cminIN[kk]); // 4 original (rescaled) values
                        int kstart=numInput;
                        for (int jj=0; jj<numInput; jj++) {
                                for (int kk=0; kk<maxInSlice; kk++)  {
                                        dataset1V->input[ev][kstart] =InSlice[jj][kk];
                                        kstart=kstart+1;
                                }
                        }

                } // end rebuilding input validation sample 


		cerr << "  input layer:      " << num_input << " units" << endl;
		cerr << "  hidden layer 1:   " << num_neurons_hidden_1 << "  units" << endl;
		//cerr << "  hidden layer 2:   " << num_neurons_hidden_2 << "  units" << endl;
		cerr << "  output layer:     " << num_output << "  units" << endl;

                // default is 3-layers
		struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden_1, num_output);
                if (num_layers==4) ann = fann_create_standard(num_layers, num_input, num_neurons_hidden_1, num_neurons_hidden_2, num_output);

		// scale input. Output does not need to be scaled
		//fann_scale_input_train_data(dataset1, 0, 1.0);
		//   fann_set_scaling_params(dataset1, ann, -1.0, 1.0, -1.0, 1.0);



		//struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden_1, num_neurons_hidden_2, num_output);
		//struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
		//fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
		//fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
		//fann_set_activation_function_hidden(ann, FANN_SIGMOID);
		//fann_set_activation_function_output(ann, FANN_SIGMOID);

        //	fann_set_activation_function_hidden(ann, FANN_SIGMOID);
	//	fann_set_activation_function_output(ann, FANN_SIGMOID);

               fann_set_activation_function_hidden(ann, FANN_LINEAR);
               fann_set_activation_function_output(ann, FANN_LINEAR);

	      //fann_set_activation_function_output(ann, FANN_SIGMOID_STEPWISE);
	      //fann_set_activation_function_hidden(ann, FANN_SIGMOID_STEPWISE);


		fann_randomize_weights(ann,0,1.0);
		if (debug) fann_print_connections(ann);
		fann_print_parameters(ann);

		cout << "Total number of neurons=" << fann_get_total_neurons(ann) << endl;
		cout << "Total number of connections=" << fann_get_total_connections(ann) << endl;
		cout << "-> Training using " << num_threads << " threads" << endl;

		cout << "Write file with MSE errors" << endl;
		std::ofstream out_error("nn_out/training_mse.d");
		out_error << "# epoch  training-MSE validation-MSE" << endl;


		// debug first 3 rows
		if (debug) {
			print_traindata(dataset1,0);
			print_traindata(dataset1,1);
			print_traindata(dataset1,2);
		}

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
				val_error = fann_test_data(ann, dataset1V);
                                train_error = fann_test_data(ann, dataset1);
                                cout << "# epoch=" << i << " train MSE=" << train_error << " validation MSE=" << val_error <<  endl;
			
                                if (train_error<desired_error) {
                                cout << " Have reached the designed error=" << desired_error << endl;
                                break;
                                }
                                if (train_error>val_error+0.001) {
                                cout << " Training has " << train_error<< " This is above the validation error=" << val_error << "\n Stop! " << endl;
                                break;
                                }
                                out_error << i << " " << train_error << " " << val_error << endl;
                        }

                        /*
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
                        */


			// save every 20 epoch
			if (i%20==0 && i>1) {
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
			if (i%epochs_between_reports==0) last_val_error=train_error;
                        */

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




	/*************************************************************************
	* Running over NN and create NN prediction
	*************************************************************************/
	if (rtype == "run") {

		ofstream myfile;
		myfile.open ("data/neuralnet.data"); // output file

		TH1D * out1res= new TH1D("nn_res", "resolution from NN nodes",maxSlice,Xmin,Xmax);
		TH1D * out1eff= new TH1D("nn_eff", "efficiency from NN nodes",100,0,1.0);
		TH1D * out2res= new TH1D("predicted_res", "predicted resolution (final)",maxSlice,Xmin,Xmax);

		cout << endl << "Testing network: open " << nn_name << endl;
		struct fann *ann_new = fann_create_from_file(nn_name);
		const char* testFile="data/test.data";
		// this is for debuging inputs
		//const char* testFile="data/train1.data";
		cout << "Read test: " << testFile << endl;
		struct fann_train_data *data = fann_read_train_from_file( testFile );
                fann_shuffle_train_data(data);
		// write header file
		myfile << data->num_data << " " << data->num_input << " " << data->num_output << "\n";

		// for NN, scale it
		//fann_scale_input_train_data(data, 0, 1.0);

		//cout << "..randomize data.. " << endl;
		//fann_shuffle_train_data(data);

		int totalEvents = data-> num_data;
		fann_type** input = data->input;
		fann_type** output = data->output;

		for (int ev=0; ev<totalEvents; ev++){

                        if (ev%10000==0) cout << "event=" << ev << endl;

			// write original
			for (unsigned int kk=0; kk< data->num_input; kk++) myfile <<  input[ev][kk] << " ";
			myfile << "" << endl;

			// prepare new rescaled  input
			fann_type uinput[num_input];

			// slice input variables
			float InSlice[numInput][maxInSlice];
			for (int jj=0; jj<numInput; jj++) {
				float vv=input[ev][jj];
				float XM=cminIN[jj];
				float del2=(cmaxIN[jj] - cminIN[jj]) / (maxInSlice);
				for (int jjj=0; jjj<maxInSlice; jjj++) {
					float d1=XM+jjj* del2;
					float d2=d1+del2;
					if (vv>d1  && vv<=d2) InSlice[jj][jjj]=1.0f;
					else InSlice[jj][jjj]=0;
				}
			}

			// normalize 4 first values
			for (int kk=0; kk<numInput; kk++)  uinput[kk]=(input[ev][kk]-cminIN[kk])/(cmaxIN[kk]-cminIN[kk]); // 4 original (rescaled) values
			int kstart=numInput;
                        // deal with the rest
			for (int jj=0; jj<numInput; jj++) {
				for (int kk=0; kk<maxInSlice; kk++)  {
					uinput[kstart] =InSlice[jj][kk];
					kstart=kstart+1;
				}
			}


			// debug first 2 rows
			if (debug) {
				if (ev<5){
					cout << ev << " row Input:" << endl;
					for (unsigned int k=0; k<num_input; k++){
						cout <<  "("<<k<<")" << uinput[k] << " ";
					}
					cout << "" << endl;
				}
			}

			// use new rescaled input
			fann_type * output1 = fann_run(ann_new, uinput);


			int INSlice[maxSlice];
			for (int jjj=0; jjj<maxSlice; jjj++) {
				// cout << output1[jjj] << endl;
				float d1=Xmin+jjj* del;
				//float d2=d1+del;
				INSlice[jjj]=(int)( output1[jjj]*nnToFreq ); // convert to int
				out1res->Fill(d1+0.5*del,(double)INSlice[jjj]); //
			}



			//float true_value=input[ev][0];
			int jjj=maxSlice; // efficiency
			float efficiency=output1[jjj];
			out1eff->Fill( efficiency );


			//cout << "efficiency =" <<  output1[jjj] << endl;

			float isExist=1;
			// efficiency. Make randon (0-1)
			float r = ((double) rand() / (RAND_MAX));
			if (r<(1-efficiency))  isExist=0;

			//cout << r << "  " << output1[jjj] << endl;

			float reco_value=0;
			if (isExist>0) {
				int BinSelected=myRand(BinOverTrue, INSlice, maxSlice); // select random value (bin) assuming frequencies
				reco_value=BinSelected;
				out2res->Fill( reco_value );
			}
			myfile <<  reco_value  << " " <<   isExist  << endl;

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
