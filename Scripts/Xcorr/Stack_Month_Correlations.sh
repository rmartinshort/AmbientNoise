#!/bin/bash

#----------------------------------------------
# RMS 2017
#----------------------------------------------
#Runs the Stack_Correlations C code 

#Run this after having created the one-sided XC files. Create a new folder called 'STACKS' os similar
#and make the parameter file according to the insructions below

if [ ! "$#" -eq 1 ]; then
    echo "Usage: Stack_Month_correlations.sh paramfile"
   
    echo "Paramfile takes following format:"

    echo "station_list [file, format = net sta cha]"
    echo "start_month start_year"
    echo "end_month end_year"
    echo "output directory [must exist]"
    exit 1
fi



paramfile=$1

#Run the C code to do this stacking
/data/dna/rmartin/Ambient_noise/CODES/src/Stack_correlations/bin/Stack_RP_with_IO.exe $paramfile
							        
