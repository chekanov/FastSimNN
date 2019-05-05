# FastSimNN
Instant simulation of detector responses using ML.
Unlike the traditional Geant4 and fast detector simulation programs,
this approach makes almost instant transformation (1-2 ms/event) of true-level Monte Carlo
record to a record that closely follows the full/fast detector simulation.
It uses an array of neural networks that need to be trained beforehand using true-level and
detector-level records.


This is how it is organized:


* builder  - read Delphes files from HepSim and train NN
* validate - validate NN  
* compare  - compare performance by reconstracting resolutions
* detector - read txt files from Delphes/toy detectors, train NN and make predictions 
* makedata - create txt files from Delphes for NN training, or from toy detectors 


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
 - Version 4.0 May 3, 2019: changing the strategy. Use toy detectors

# Contact 
Send  comments to S.Chekanov (ANL)
