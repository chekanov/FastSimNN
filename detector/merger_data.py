#!/usr/bin/env python
# This is python module to merge FastANN files 
# S.Chekanov (ANL)

import sys
print "This is the name of the script: ", sys.argv[0]
print "Number of arguments: ", len(sys.argv)
print "The arguments are: " , str(sys.argv)

if (len(sys.argv)<3):
      print "Merger for FastANN files"
      print "merge_data.py [ouput file], input1, input2, input3.."
      sys.exit()

Outfile=sys.argv[1]
print "Output file=",Outfile
filenames=[]
for i in range(2,len(sys.argv)):
     ss=sys.argv[i]
     filenames.append(ss)
     print i-1, "file to merge=",ss

with open(filenames[0]) as f:
    first_line = f.readline()
    first_line=first_line.split()
    ntot=int(first_line[0])
    mSize=int(first_line[1])
    mOutputs=int(first_line[2])

print "Input size=",mSize
print "Output size=",mOutputs
print "Number of files to merge=",len(sys.argv)-2

print "Collecting data.."
total=0;
tmpfile="/tmp/tmp.txt"
with open(tmpfile, 'w') as outfile:
    for fname in filenames:
        nn=0
        with open(fname) as infile:
            for line in infile:
                if (nn>0): 
                        total=total+1 
                        outfile.write(line)
                nn=nn+1

total=total/2

print "Final processing of ",total," inputs.."
firstline=str(total)+" "+str(mSize)+" "+str(mOutputs)+"\n";
with open(Outfile, 'w') as outfile:
        with open(tmpfile) as infile:
            outfile.write(firstline)
            for line in infile:
                outfile.write(line)
 


print "Output file wrritten=",Outfile


