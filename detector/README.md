# Toy simulation

This is an example that reads either fake data or Delphes data (as txt files)
and performs training and validation.
Make sure $ROOTSYS and $FANN are set (point to ROOT and FANN packages).
For the ANL cluster, setup this example as:

```
source ./setup.sh
```

# Train NN and make predictions 

The program "ana.cc" is designed to train NN and create predictions.
The program can be compiled as "make". To run on the ANL ATLAS cluster.

```
./A_RUN        # run NN training and then validation 
```
It will train NN  using the file train1.data, and then validate the training 
(or make predictions) using valid1.data.
The NN will be written inside "nn_out". 

The NN-predicted file is "data/neuralnet.data". It has the same input as valid1.data,
but each record has predicted reco-true value and "efficiency" (values 0 or 1).   

# Compare NN data with validation data

You can compare validation sample in "valid1.data" with NN sample "neuralnet.data" 
using "plotme.py" script ("python plotme.py"). It will make the "resolution" plots. 
You can set cuts "Cut1" on the 2nd value of the input, to see how the predicted value
can change the resolution.

Another script is "plot_nn.py". It plots resolution histograms directly from ROOT files
created from ana.cc. It plots the original reco-true, NN neuron response  and frequencies from the NN response.


# History

 - Version 1.0 May 2, 2019: First version

# Contact 
Send  comments to S.Chekanov (ANL)
