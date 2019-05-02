#!/usr/bin/python2 

# Here is an abstract example of converting an array of values into "detector" modified array.
# It uses 3 other arrays that can influence such transformation (in function dtransform())
# The transformation includes: loses, shifting the mean, and applying some smearing using a Gaussian
# S.Chekanov (ANL)

import sys,random
from math import *

myinput="data/train1.data"
if (len(sys.argv) > 1):
   myinput = "data/"+sys.argv[1]
print "Output file name is =",myinput

# total number of events with a random vector that need to be changed 
NrOfEvents=500000
print "Number of objects=",NrOfEvents


# transformation of the original array to a modified array
# transformation depends on 3 parameters (P1,P2,P3) and many other
# change this transform in as you wish. 
#   - simulate loses via Efficiency
#   - simulate response by shifting mean value "Response" 
#   - simulate resolution by smearing with "Sigma"
def dtransform(VAL, P1, P2, P3):
     """Apply some smearing and shift true value v"""
     VAL_REC=[]
     for i in range(len(VAL)):
            Efficiency=1-0.1*(P1[i]+P2[i]-P3[i])             # make any function from P1, P2, P3 and make sure 0<Efficiency<1
            if (Efficiency<0.1): Efficiency=0.1
            if (random.random()>Efficiency):     continue;   # skip this number

            Sigma= 0.5*(P1[i]+P2[i]-P3[i])                   # or make any function from P1, P2, P3
            if (Sigma<0.1): Sigma=0.1

            Response = 1.0+ (-0.1*P1[i]-0.1*P2[i]+0.1*P3[i]) # or make any function from P1, P2, P3 
            if (Response<0.1): Response=0.2

            VAL_REC.append(random.gauss(VAL[i]*Response, Sigma))
           
     return VAL_REC


ff=open(myinput,"w")
ff.write(str(NrOfEvents)+" "+str(4)+" "+str(2)+"\n")


# create events with particles for each event: 
for nev in xrange(NrOfEvents):
         #tot_particles=int(abs(random.gauss(mu=50, sigma=20))) # number of particles per event is taken at random  

         tot_particles=1;

         # main variable for the transformation (i.e. pT, for example)
         # it has some density distribution. For example -  one-sided Gaussian
         VAL=[]
         for i in range(tot_particles):
                   VAL.append(abs(random.gauss(mu=0, sigma=2000)))

         # 3 variables that can modify VAL variable
         # assume they are randomly distributed between -pi and pi 
         P1=[]
         for i in range(tot_particles):
                   P1.append( random.uniform(-pi, pi) )

         P2=[]
         for i in range(tot_particles):
                   P2.append( random.uniform(-pi, pi) )

         P3=[]
         for i in range(tot_particles):
                   P3.append( random.uniform(-pi, pi) )


         # transform VAL into VAL_REC0
         # use P1, P2, P3 arrays to influence this transformation
         VAL_REC=dtransform(VAL, P1, P2, P3)

         # output is the difference
         out1=0
         exist=0
         if (len(VAL_REC) >0):
                              out1=VAL_REC[0]-VAL[0]
                              exist=1

         if (nev%10000==0): 
             print "event=",nev," has ",len(VAL)," particles. After transform=",len(VAL_REC)  


         ff.write(str(VAL[0])+" "+str(P1[0])+" "+str(P2[0])+" "+str(P3[0])+"\n")
         ff.write(str(out1)+" "+str(exist)+"\n")

print "Write",myinput

