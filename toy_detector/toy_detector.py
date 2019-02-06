#!/usr/bin/python2 
# This is a toy example illustrating a detector response. 
# It creates a list of particles characterized with 4 values (PT,ETA,PHI,MASS)
# (truth-level). Then it applies some detector smearing and loses to mimic a typical detector
# ------------
# How to debug:
# ------------
# To fill histograms, uncomment lines with #datamelt statements and run inside DataMelt
# @author S.Chekanov (ANL)

import random
from math import *
pi=3.14159

#datamelt debug: fill resolution histogram
#-----------------------------------------
#from jhplot import *
#c1 = HPlot()
#c1.visible(True)
#c1.setRangeX(0.6,1.4)
#h1 = H1D("resolution",100, -2, 2.0)
#-----------------------------------------

# total number of collision events 
NrOfEvents=100000

#################################################
# smearing for energies of jets or particles
#################################################

RESPONSE=0.98 # on average, energies are shifted by -2%  
# suggested variations: (-2%, 2%) 

# In addition, energies are stochastically smeared by a detector 
# using this formula: sigma = B*E+A*sqrt(E)
# Note:  positions are smeared with sigma x10 smaller  

# Smearing for jets: (photons have A=0.1, B=0.0) 
A=0.50  # stochastic term of a detector smearing for jet 
B=0.03  # constant term of detector smearing 
# Suggested variations:
# RESPONSE: 0.95-1.0 
# A:        0.6-0.02
# B:        0.01-0.03

# simulate efficiency of some "bad" region in Eta (pseudorapidity) 
# assume 80% efficiency for |Eta|>2.5
# in addition, |Eta|>2.5 has larger resolution (by a factor 3). See below.
Efficiency=0.8 

# input:  v - value to be smeared
#        pt - the value to be smeared  usually depends on energy (pT)
#        scale - scale factor. For photons, electrons, muons, set to 0.1
def getSmear(v,pt,scale=1):
     """Apply some detector smearing and shift true value v"""
     global A,B,RESPONSE  
     # assume constant B and stochastic A terms
     sigma=scale*(B*pt+A*sqrt(pt))
     return random.gauss(v*RESPONSE, sigma)

# create events with particles for each event: 
for nev in xrange(NrOfEvents):
         tot_particles=int(abs(random.gauss(mu=50, sigma=20))) # number of particles per event is taken at random  
         PT,ETA,PHI,MASS=[],[],[],[]
         # create physics lists with particles characterized by pt, eta, phi, mass
         # mimic initial physics distributions
         for i in range(tot_particles):
                   PT.append(abs(random.gauss(mu=0, sigma=2000))) 
                   ETA.append(random.uniform(3, 3))
                   PHI.append(random.uniform(-pi, pi))
                   MASS.append(abs(random.gauss(mu=100, sigma=200)))
         ##########################################################
         ## start simulation of detector response. 
         ##########################################################
         PT_D=[]
         ETA_D=[]
         PHI_D=[]
         MASS_D=[]
         for i in xrange(tot_particles):
              pt  =getSmear(PT[i],PT[i])  
              mass=getSmear(MASS[i],PT[i])
              eta =getSmear(ETA[i],PT[i],0.1) # positions are always x10 better measured  
              phi =getSmear(PHI[i],PT[i],0.1) 
              # First remove some particles in eta detector region with low efficiency
              if (abs(eta)>2.5): # positive eta>2.5 has low efficiency (missing detector module) 
                         eta =getSmear(ETA[i],PT[i],0.3) # x3 larger smearing 
                         if (random.random()>Efficiency): continue;

              PT_D.append(pt)
              ETA_D.append(eta)
              PHI_D.append(phi)
              MASS_D.append(mass)

              # datamelt debug: fill histogram
              # h1.fill(pt/PT[i])

         if (nev%1000==0): 
             print "Physics event=",nev," has ",tot_particles," particles. After detector=",len(PT_D)  
         # OUTPUT:
         # PT[], ETA[], PHI[], MASS[] - list of particles entering a detector 
         # PT_D[], ETA_D[], PHI_D[], MASS_D[] - list of particles after the detector
         

#-----------------------------------
# datamelt debug: show histogram
#c1.draw(h1)
#-----------------------------------
