# Toy simulation
 
This is an example that creates fake data and perform training and validation

```
python2 makedata.py train1.data
python2 makedata.py valid1.data
make
./A_RUN
```


# Now to run
The program is designed and compiled to run on the ANL ATLAS cluster.
You need to link it against Delphes. 

# Input of data
The inputs for this program are data from the HepSim repository (http://atlaswww.hep.anl.gov/hepsim/). 
The training is done using ttbar+jets and gamma+jet samples (weighted). This allows
tests of jets in the range (25 GeV -3 TeV), photons, muons and electrons. 


# History

 - Version 1.0 December 1, 2018: Initial version
 - Version 2.0 December 14, 2018: Corrected Eta resolution
 - Version 3.0 December 16, 2018: Added muons, electrons, photons

# Contact 
Send  comments to S.Chekanov (ANL)
