# Dump simulations to text 

This directory is used to create data from Delphes/PROMC files
as well as data from toy detectors.

```
source ./setup.sh
```


#Fimulations to text
Make 2 files with training (train1.data), cross validation data (valid1.data) and test (test1.data).
Each input record has 4 values. 1st value (true) is used to create rec-true.
The output record has  2 values. 1st value is difference reco-true (where true is the 1st value
of the input record). Note that reco-true difference is affected by other 3 values
in the input record.
2nd value is "0" (event did not pass) or "1" the event was correctly processed.
Read the file makedata.py where you can modify this transform.


```
python2 makedata.py train1.data # training sample
python2 makedata.py valid1.data # validation sample
python2 makedata.py test1.data  # test sample
```

The files will be stored in the "data" directory


# How to use


```
A_RUN_TOY # create toy-detector data inside ./data directory
A_RUN     # create Delphes data from fast simulations

```


# History

 - Version 1.0 May 2, 2019: First version

# Contact 
Send  comments to S.Chekanov (ANL)
