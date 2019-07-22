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
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TRandom.h>
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

// range of main input variable and output 
const double Xmin=0;
const double Xmax=100;


// range of resolution variable
const double Ymin=-10;
const double Ymax=10;

// influencial variable change from 0 to 1

const double kPI   = TMath::Pi();
const double k2PI  = 2*kPI;

extern double RMS90(TH1*);
int findCeil(int  arr[], int r, int l, int h);
int myRand(int arr[], int freq[], int n);
void print_traindata(fann_train_data * data, int row);

// return  cut
bool cut(double v1, double v2, double v3){
 if (v1<10 && v2<0.5 && v3<0.5) return true;
 return false;
}

// main example
int main(int argc, char **argv)
{

	const bool debug=false;

	    
	// how many slices assume slices to divide the phase space for each output 
        const double desired_error = 0.00001;
        const unsigned int max_epochs = 60;
        const unsigned int epochs_between_reports = 10;
        cout << "Max number of epoch " << max_epochs  <<  endl;
        unsigned int num_threads = 1;

	if (argc != 2) {
		cerr << " Unexpected number of command-line arguments. \n Set: train or run"
		<< " Program stopped! " << endl;
		return 1;
	}

	string rtype("-");
	rtype = argv[1];


	string outputfile="output.root";
        cout << "\n -> Output file is =" << outputfile << endl;
        TFile * RootFile = new TFile(outputfile.c_str(), "RECREATE", "Histogram file");
        TH1D * input= new TH1D("input", "input",100,Ymin,Ymax);
        TH1D * inputNN= new TH1D("inputNN", "inputNN",100,Ymin,Ymax);


        const int events=50000;
        const int events_valid=50000;

	const int nbins=100;
        int num_input=nbins;
        int num_output=nbins;
        const unsigned int num_neurons_hidden_1=(int)(num_input*0.7); 
        // number in hidden layer 2
        const unsigned int num_neurons_hidden_2=(int)(num_input/4.0);
	const unsigned int num_layers = 3;

	double del1=(Xmax-Xmin)/num_input;
	double del2=(Ymax-Ymin)/num_output;

	fann_train_data *  dataset_input=  fann_create_train(events, num_input, num_output);
        fann_train_data *  dataset_valid=  fann_create_train(events_valid, num_input, num_output);

	TRandom random;

        
	for (int ev=0; ev<events+events_valid; ev++){


	 double vv=Xmin+(Xmax-Xmin)*random.Rndm();

         // influence variables
	 double influence1=random.Rndm();
         double influence2=random.Rndm();

      
         // output depends on 2 influencial variables and the main variable 
         double vvout=random.Gaus (0.05*vv+2*influence1, 0.5+0.01*vv+2*influence1+4*influence2); 


	 if (cut(vv, influence1, influence2)==true)
                                	 input->Fill(vvout);


	 fann_type uinput[num_input+2]; // plus 2 influencial variables;
         fann_type uoutput[num_output]; 



         for (int jjj=0; jjj<num_input-1; jjj++) {
                                        float d1=Xmin+jjj* del1;
                                        float d2=d1+del1;
                                        if (vv>d1  && vv<=d2) uinput[jjj]=1.0f;
                                        else uinput[jjj]=0;
                                }

	 uinput[num_input]=influence1;
         uinput[num_input+1]=influence2;


         for (int jjj=0; jjj<num_output-1; jjj++) {
                                        float d1=Ymin+jjj* del2;
                                        float d2=d1+del2;
                                        if (vvout>d1  && vvout<=d2) uoutput[jjj]=1.0f;
                                        else uoutput[jjj]=0;
                                }

	 if (ev<events){
	 for (unsigned int kk=0; kk<num_input; kk++)   dataset_input->input[ev][kk] =uinput[kk];
         for (unsigned int kk=0; kk<num_output; kk++)  dataset_input->output[ev][kk] =uoutput[kk];
         }

         if (ev>=events && ev<events+events_valid){
         for (unsigned int kk=0; kk<num_input; kk++)   dataset_valid->input[ev-events][kk] =uinput[kk];
         for (unsigned int kk=0; kk<num_output; kk++)  dataset_valid->output[ev-events][kk] =uoutput[kk];
         }

	 if (ev%1000 == 0) cout << "Event=" << ev << endl;



	}


                cerr << "  input layer:      " << num_input << " units" << endl;
                cerr << "  hidden layer 1:   " << num_neurons_hidden_1 << "  units" << endl;
                cerr << "  hidden layer 2:   " << num_neurons_hidden_2 << "  units" << endl;
                cerr << "  output layer:     " << num_output << "  units" << endl;
                struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden_1, num_output);
                //fann_set_activation_function_hidden(ann, FANN_SIGMOID);
                //fann_set_activation_function_output(ann, FANN_SIGMOID);
                fann_set_activation_function_hidden(ann, FANN_LINEAR);
                fann_set_activation_function_output(ann, FANN_LINEAR);

                fann_randomize_weights(ann,0,1.0);

        	cout << "Write file with MSE errors" << endl;
                std::ofstream out_error("nn_out/training_mse.d");
                out_error << "# epoch  training-MSE validation-MSE" << endl;


               std::vector<double> last_val_errors;
                double last_val_error=10000;
                double val_error=last_val_error;
                double train_error=100000;
                for(unsigned int i = 1 ; i <= max_epochs ; i++) {
                        //fann_train_epoch(ann, dataTrain);
                        // parallel

                        train_error = num_threads > 1 ? fann_train_epoch_irpropm_parallel(ann, dataset_input, num_threads) : fann_train_epoch(ann, dataset_input);

                        double delta=last_val_error-train_error;
                        if (i%epochs_between_reports==0 || i<epochs_between_reports) {
                                train_error = fann_test_data(ann, dataset_input);
                                val_error = fann_test_data(ann, dataset_valid);
                                cout << "# epoch=" << i << " train MSE=" << train_error << " validation MSE=" << val_error <<  endl;

                                if (train_error<desired_error) {
                                cout << " Have reached the designed error=" << desired_error << endl;
                                break;
                                }
                                //if (train_error>val_error+0.0001) {
                                //cout << " Training has " << train_error<< " This is above the validation error=" << val_error << "\n Stop! " << endl;
                                //break;
                                //}
                                out_error << i << " " << train_error << " " << val_error << endl;
                        }
   

                        // save every 20 epoch
                        if (i%20==0 && i>1) {
                                stringstream ss;
                                ss << i;
                                string str = "nn_out/neural_"+ss.str()+".net";
                                cout << "Save intermidiate result to " << str << endl;
                                fann_save(ann, str.c_str());
                        }

                        } // end loop over epochs


/*

	   for (int ev=0; ev<events; ev++){
		     

           // influence variables
           double influence1=random.Rndm();
           double influence2=random.Rndm();
           if (influence1>0.5 && influence2>0.5) {


           for (int ev=0; ev<num_input; ev++){
		 // use new rescaled input

                fann_type uinput[num_input+2];
                for (int jjj=0; jjj<num_input-1; jjj++) {
                                        if (ev==jjj) uinput[jjj]=1.0f;
                                        else uinput[jjj]=0;
                                }


               uinput[num_input]=influence1;
               uinput[num_input+1]=influence2;


                fann_type * output1 = fann_run(ann, uinput);

		   for (int jjj=0; jjj<num_output-1; jjj++) {
                                        float d1=Xmin+jjj* del2;
                                        inputNN->Fill(d1+0.5*del2, output1[jjj]*10000); //
			                //if (ev == 0) cout << jjj << " " << output1[jjj] << endl;	

		   }

	   }


	   } // end influencial variables

	   } // end run over events
*/

          for (int ev=0; ev<events; ev++){
                 // use new rescaled input


          double vv=Xmin+(Xmax-Xmin)*random.Rndm();
          fann_type uinput[num_input+2];


            // influence variables
           double influence1=random.Rndm();
           double influence2=random.Rndm();
           if (cut(vv, influence1, influence2)==true)  {

                 for (int jjj=0; jjj<num_input-1; jjj++) {
                                        float d1=Xmin+jjj* del1;
                                        float d2=d1+del1;
                                        if (vv>d1  && vv<=d2) uinput[jjj]=1.0f;
                                        else uinput[jjj]=0;
                                }

              uinput[num_input]=influence1;
              uinput[num_input+1]=influence2;



                   fann_type * output1 = fann_run(ann, uinput);

                   for (int jjj=0; jjj<num_output-1; jjj++) {
                                        float d1=Ymin+jjj* del2;
                                        inputNN->Fill(d1+0.5*del2, output1[jjj]); //
                                        if (ev == 0) cout << jjj << " " << output1[jjj] << endl;        

                   }

            } // end influencial cut
           }




	 RootFile->Write();
         RootFile->Print();
         RootFile->Close();
         cout << "Writing ROOT file "+ outputfile << endl;


	return 0;
}
