#!/bin/bash
# FastHepSim setup script. 
# Fast simulation of HepSim files with Delphes
# S.Chekanov (ANL) chekanov@anl.gov

echo "Setting up FastHepSim.." 

HH=`hostname -A`

echo "HOST=$HH"

if [[ $HH =~ .*atlaslogin.* ]]; then
  echo "HEP ANL ROOT setup"
  source /users/admin/share/sl7/setup.sh
  export DELPHES=../../Processing/delphes
  export LD_LIBRARY_PATH=$DELPHES:$LD_LIBRARY_PATH
else
  echo "Error! No such computer!"
fi


