#!/bin/bash

#----------------------------------------------
# RMS 2017
#----------------------------------------------
#Runs the select_dispersion_snr_and_dist_phase_only code 

symdir=/data/dna/rmartin/Ambient_noise/Alaska/ALL_DATA/COR/STACK/SYM/region_files

nPerFTAN=32
lagTimeinCors=5001
minperiod=8
maxperiod=40
periodincrement=1
outputdir=/data/dna/rmartin/Ambient_noise/Alaska/ALL_DATA/COR/STACK/Dispersion_curves_10_3

#make output directory if it doesn't already exist
if [ ! -d $outputdir ]; then
    echo "Directory $outputdir not found. Making it now"
    mkdir $outputdir
fi

cwd=`pwd`
echo $cwd

cp generate_sacfilelist.py $symdir
cd $symdir

#Check to see if required python script is present

if [ ! -f generate_sacfilelist.py ]; then
     echo "The python script generate_sacfilelist.py should be in directory $symdir"
     exit 1
fi

./generate_sacfilelist.py
echo sacFileList > param_SD.in
echo $nPerFTAN $lagTimeinCors >> param_SD.in
echo $minperiod $maxperiod $periodincrement >> param_SD.in
echo $outputdir >> param_SD.in

echo "--------------------------------------"
cat param_SD.in
echo "--------------------------------------"

# Run the C code to select the required parameters
/data/dna/rmartin/Ambient_noise/CODES/src/Dispersion_selection/bin/Select_Dispersion_SNR_and_DIST_group_only.exe param_SD.in
/data/dna/rmartin/Ambient_noise/CODES/src/Dispersion_selection/bin/Select_Dispersion_SNR_and_DIST_phase_only.exe param_SD.in
