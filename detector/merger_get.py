#!/usr/bin/env python
# Create a file using input and output index. First even includes 
# S.Chekanov (ANL)

import sys
print "This is the name of the script: ", sys.argv[0]
print "Number of arguments: ", len(sys.argv)
print "The arguments are: " , str(sys.argv)

if (len(sys.argv)<4):
      print "Merger for FastANN files"
      print "merge_get.py first last input output"
      sys.exit()

IStart=int(sys.argv[1])
IEnd=int(sys.argv[2])

Infile=sys.argv[3]
print "Input file=",Infile

Outfile=sys.argv[4]
print "Output FastANN file=",Outfile



with open(Infile) as f:
    first_line = f.readline()
    first_line=first_line.split()
    ntot=int(first_line[0])
    mSize=int(first_line[1])
    mOutputs=int(first_line[2])

print "Input size=",mSize
print "Output size=",mOutputs
print "Number of files to merge=",len(sys.argv)-2

total=IEnd-IStart
firstline=str(total)+" "+str(mSize)+" "+str(mOutputs)+"\n";
print "Collecting data.."
total=0;
with open(Outfile, 'w') as outfile:
        with open(Infile) as infile:
            outfile.write(firstline)
            nn=0;
            for line in infile:
                 line=line.replace("# ","\n")
                 if (nn>=IStart and nn<IEnd): outfile.write(line)
                 nn=nn+1
print "Created file with data ",IStart," - ",IEnd," from input with ",nn
print "Output file written=",Outfile
if nn<IEnd:
          print "Error in creating file. Nr of lines=",nn," is less then  max ",IEnd," required"
          print "Remove the file!"
          os.remove(Outfile)


