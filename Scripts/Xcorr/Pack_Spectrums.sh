#!/bin/bash

#----------------------------------------------
# RMS 2017
#----------------------------------------------
#Runs the Pack_Spectrums C code

if [ "$#" -eq 9 ]; then 

	year=$1
	month=$2
	start_month_day=$3
	end_month_day=$4
	station_list=$5
	base_dir=$6
	db_name=$7
	delete_flag=$8
	npts=$9

else

	echo "Can input the parameters on the command line. Going into interactive mode..."
	echo "ie: run_make_ant_db.tcsh year month start_month_day end_month_day station_lists base_dir db_name"
	echo "Please input the year:"
	year=`return_int`
	echo "Plese input the month:"
	month=`return_int`
	echo "Please input the starting day for the month:"
	start_month_day=`return_int `
	echo "Please input the ending day for the month:"
	end_month_day=`return_int`
	echo "Please input the station list name:"
	station_list=`return_string`
	echo "Please input the base directory:"
	base_dir=`return_string`
	echo "Please input the output name:"
	db_name=`return_string`
	echo "Do you wish to delete the original sac files (1=yes, 0=no)?"
	delete_flag=`return int`
	echo "How many points are in each spectral sac file (LHZ = 43201)?"
	npts=`return int`

fi

#Go into the directory and get the starting and ending jday
echo '-----------------------------------------'
echo "Setting parameters"
echo '-----------------------------------------'

#Note that get start time is a also a C code, found in 
#/data/dna/software/ANT/sac_readers/src/GET_START_TIME

#Get a sac_files that existed on the first day and the last day (doesn't have to be the same file - we just want the time)

firstday_sacfile=$(ls ${base_dir}/${year}/${month}/${start_month_day}/*_re | head -1)
lastday_sacfile=$(ls ${base_dir}/${year}/${month}/${end_month_day}/*_re | head -1)

start_jday=`/data/dna/rmartin/Ambient_noise/CODES/src/SAC_readers/bin/get_start_time.exe $firstday_sacfile | awk '{print $2}'`
end_jday=`/data/dna/rmartin/Ambient_noise/CODES/src/SAC_readers/bin/get_start_time.exe $lastday_sacfile | awk '{print $2}'`

#Check to see that these day determinations are reasonable - sometimes they are not and the day directory at fault
#needs to be deleted
if [ $start_jday -gt $end_jday ]; then
        rm -rf ${base_dir}/${year}/${month}/${end_month_day}
        end_month_day=30 #will be the case in all months except feb, which seems to be handled correctly
	lastday_sacfile=$(ls ${base_dir}/${year}/${month}/${end_month_day}/*_re | head -1)
	start_jday=`get_start_time $firstday_sacfile | awk '{print $2}'`
	end_jday=`get_start_time $lastday_sacfile | awk '{print $2}'`
fi


#Create the parameter file for input to the C code 
echo "$year $month" > param.dat
echo "$start_month_day $end_month_day" >> param.dat
echo "$start_jday $end_jday" >> param.dat
echo "$station_list" >> param.dat
echo "$base_dir" >> param.dat
echo "$db_name" >> param.dat
echo "$delete_flag" >> param.dat
echo "$npts" >> param.dat

echo "Param file made:"
cat param.dat

echo "You can reuse this in future runs as:"
echo "make_ant_db param.dat"

#read -n1 -r -p  "if this file looks correct, hit space to continue" key
#if [ "$key" = '' ]; then 
   #Edit the excutable once the new version is ready
   #pack_spectrums param.dat
   
/data/dna/rmartin/Ambient_noise/CODES/src/Correlate_single_spectrum/bin/Pack_Spectrums.exe param.dat
#echo "$db_name made"

#else

#    exit 1
#fi 
