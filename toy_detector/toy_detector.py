#!/usr/bin/python2 
# This is a toy example illustrating a detector response. 
# It creates initial physics list (truth-level) and then apply some smearing 
# to mimic a toy detector 
# @author S.Chekanov (ANL)

import sys, os, re, time
import random
import math


# suggested values to scan for validation:
# RESPONSE: 0.97-1.03
# A:        0.6-0.2
# B:        0.01-0.03


# total number of collision events 
NrOfEvents=100000

# smearing for energies of jets or particles
RESPONSE=0.97 # on average, energies are shifted by -3%  

# In addition, energies are stochatically smeared by a detector 
# using this formula: sigma = B*E+A*sqrt(E)
# Note: for photons and electrons the smearing is x10 smaller! 
A=0.20  # stochastic term of a detector smearing 
B=0.03 # constant term of detector smearing 

# simulate efficiency of some region in Eta (pseudorapidity) 
# assume low efficiency for |Eta|>2.5
Efficiency=0.8 

# input: v - value to be smeared
#        pt - the value to be smeared  usually depends on energy (pT)
#        scale - scale factor. For photons, electrons, muons, set to 0.1
def getSmear(v,pt,scale=1):
     """Apply some detector smearing and shift true value"""
     global A,B,RESPONSE  
     # assume contant B and stochastic A terms
     sigma=scale*(B*pt+A*math.sqrt(pt))
     return random.gauss(v*RESPONSE, sigma)

# create events with particles
for nev in xrange(NrOfEvents):
         tot_particles=int(abs(random.gauss(mu=50, sigma=20)))
         PT,ETA,PHI,MASS=[],[],[],[]
         # create physics lists with particles characterzed by pt, eta, phi, mass
         # mimic initial physics distributions
         for i in range(tot_particles):
                   PT.append(abs(random.gauss(mu=0, sigma=2000))) 
                   ETA.append(random.uniform(3, 3))
                   PHI.append(random.uniform(-math.pi, math.pi))
                   MASS.append(abs(random.gauss(mu=100, sigma=200)))
         ##########################################################
         ## start simulation of detector response. 
         ##########################################################
         PT_D=[]
         ETA_D=[]
         PHI_D=[]
         MASS_D=[]
         for i in range(tot_particles):
              eta=ETA[i]
              # First remove some particles in eta detector region with low efficiency
              if (eta>2.5): # postive eta>2.5 has low efficiency (missing detector module) 
                          if (random.random()>Efficiency): continue;
              pt  =getSmear(PT[i],PT[i])  
              mass=getSmear(MASS[i],PT[i])
              eta =getSmear(ETA[i],PT[i],0.1) # positions are always x10 better measured  
              phi =getSmear(PHI[i],PT[i],0.1) 
              PT_D.append(pt)
              ETA_D.append(eta)
              PHI_D.append(phi)
              MASS_D.append(mass)

         if (nev%1000==0): 
             print "Physics event=",nev," has ",tot_particles," particles. After detector=",len(PT_D)  
         # PT[], ETA[], PHI[], MASS[] - list of particles entering a detector 
         # PT_D[], ETA_D[], PHI_D[], MASS_D[] - list of particles after the detector  
 
