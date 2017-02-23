#!/bin/bash

#----------------------------------------------
# RMS 2017
#----------------------------------------------
#Runs the Pack_Spectrums C code

echo "setting the LD_LIBRARY_PATH to /usr/local/lib64:${LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH=/usr/local/lib64:${LD_LIBRARY_PATH}

#defaults:
stack_flag=1
mean_flag=0
sd_flag=0

#check usage
if [ "$#" -eq 7 ]; then
	data_base=$1
	output_directory=$2
	lag_time=$3 
	stack_flag=$4
	mean_flag=$5
	sd_flag=$6
	new_base_dir=$7
else 
        echo "Incorrect number of arguments entered!!"
	echo "Can input the parameters on the command line. Going into interactive mode..."
        echo "ie: run_cor_rp.tcsh data_base waveform_base_directory output_directory lag_time"
	echo "Please input the data_base:"
	data_base=`return_string `
	echo "Please input the output_directory:"
	output_directory=`return_string `
	echo "Please input the maximum lag time:"
	lag_time=`return_int `
	echo "To tell the program to return the stacked correlation, mean correlation, and standard deviation of the correlation, either enter 1 for yes or 0 for no at the following prompts:"
	echo "Would you like the stacked value?"
	stack_flag=`return_int `
	echo "Would you like the mean correlation?"
	mean_flag=`return_int `
	echo "Would you like the standard deviation?"
	sd_flag=`return_in `
	echo "Please input the base directory with the raw data (contains all_data_cor_correlations.dat and year/month/headers)"
	new_base_dir=`return_string `
fi

#Create the parameter file for input to the C code 
echo "$data_base" > param.cor
echo "$output_directory" >> param.cor
echo "$lag_time" >> param.cor
echo "$stack_flag $mean_flag $sd_flag" >> param.cor
echo "0 0" >> param.cor   #option to start at some given correlation pair. Give the i,j values of the correlation pair
echo "$new_base_dir" >> param.cor

#Edit the excutable once the new version is ready
#cor_rp_single_spect param.cor
/data/dna/software/ANT/MAIN_ANT_CODES/src/COR_RP_SINGLE_SPECT/rms_version/bin/Cor_Single_Spect.exe param.cor

echo "Done with correlations"
