# Toy simulation

This is an example that creates fake data and perform training and validation.
Make sure $ROOTSYS and $FANN are set (point to ROOT and FANN packages).

# How to create input data 

First, make 2 files with training (train1.data) and validation data (valid1.data).
Each input record has 4 values. 1st value (true) is used to create rec-true. 
The output record has  2 values. 1st value is difference reco-true (where true is the 1st value
of the input record). Note that reco-true difference is affected by other 3 values 
in the input record.
2nd value is "0" (event did not pass) or "1" the event was correctly processed.
Read the file makedata.py where you can modify this transform.


```
python2 makedata.py train1.data # training sample
python2 makedata.py valid1.data # validation sample
```

The files will be stored in the "data" directory

# Train NN and make predictions 

The program "ana.cc" is designed to train NN and create predictions.
The program can be compiled as "make". To run on the ANL ATLAS cluster.

```
./A_RUN                         # run  
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

# History

 - Version 1.0 May 2, 2019: First version

# Contact 
Send  comments to S.Chekanov (ANL)
