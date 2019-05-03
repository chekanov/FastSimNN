#include<iostream>
#include<fstream>
#include <stdio.h>
#include <stdlib.h>

// check directory
#include <string>
//#include "doublefann.h"
//#include "fann_cpp.h"
#include "fann.h"
#include "parallel_fann.h"

using namespace std;

// print row from input train data
void print_traindata(fann_train_data * data,  int row) {

   fann_type** input = data->input;
   fann_type** output = data->output;

   int totalEvents = data-> num_data;
   int  numInput=data->num_input;
   int  numOut=data->num_output;

   cout << row << " row Input:" << endl;
   for (int k=0; k<numInput; k++){
      cout << "("<<k<<")" << input[row][k] << " ";
   }
   cout << "" << endl;

   cout << row << " row Output:" << endl;
   for (int k=0; k<numOut; k++){
      cout << "("<<k<<")" << output[row][k] << " ";
   }
   cout << "" << endl;

   if (row>totalEvents) {
    cout << "Raw is too large! Larger than the size =" << totalEvents << endl;
    return;
   }

}



