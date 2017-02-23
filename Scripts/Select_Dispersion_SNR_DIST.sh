#!/bin/bash

#----------------------------------------------
# RMS 2017
#----------------------------------------------
#Runs the select_dispersion_snr_and_dist_phase_only code 

symdir=/data/dna/rmartin/Ambient_noise/Alaska/TEST_WORKFLOW/2/COR/STACKS/SYM

nPerFTAN=32
lagTimeinCors=5001
minperiod=5
maxperiod=40
periodincrement=1
outputdir=/data/dna/rmartin/Ambient_noise/Alaska/TEST_WORKFLOW/2/COR/Dispersion_curves2


cwd=`pwd`
echo $cwd

cp generate_sacfilelist.py $symdir
cd $symdir
./generate_sacfilelist.py
echo sacFileList > param_SD.in
echo $nPerFTAN $lagTimeinCors >> param_SD.in
echo $minperiod $maxperiod $periodincrement >> param_SD.in
echo $outputdir >> param_SD.in

echo "--------------------------------------"
cat param_SD.in
echo "--------------------------------------"

# Run the C code to select the required parameters
/data/dna/software/ANT/MAIN_ANT_CODES/src/DISPERSION_SELECTION/PHASE_ONLY/rms_version/bin/Select_Dispersion_SNR_and_DIST_group_only.exe param_SD.in
/data/dna/software/ANT/MAIN_ANT_CODES/src/DISPERSION_SELECTION/PHASE_ONLY/rms_version/bin/Select_Dispersion_SNR_and_DIST_phase_only.exe param_SD.in

cd $cwd

