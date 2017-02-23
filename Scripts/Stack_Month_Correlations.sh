#!/bin/bash

#----------------------------------------------
# RMS 2017
#----------------------------------------------
#Runs the Stack_Correlations C code 

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
/data/dna/software/ANT/MAIN_ANT_CODES/src/STACK_RP/rms_version/bin/Stack_RP_with_IO.exe $paramfile
							        
