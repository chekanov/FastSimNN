# Toy simulation

This is an example that creates fake data and perform training and validation.
Make sure $ROOTSYS and $FANN are set (point to ROOT and FANN packages).
 

# Now to run

The program is designed and compiled to run on the ANL ATLAS cluster.

```
python2 makedata.py train1.data
python2 makedata.py valid1.data
make
./A_RUN
```

# History

 - Version 1.0 December 1, 2018: Initial version
 - Version 2.0 December 14, 2018: Corrected Eta resolution
 - Version 3.0 December 16, 2018: Added muons, electrons, photons

# Contact 
Send  comments to S.Chekanov (ANL)
