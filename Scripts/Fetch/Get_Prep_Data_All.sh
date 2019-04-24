#!/bin/bash 

#----------------------------------------------
# RMS 2017
#----------------------------------------------
#Script to download data for use with Ambient Noise Tomography workflow
#Downloads day data using IRIS FetchData script, then processes it with the 
#pre-processing code all_ant_prep_PA.c

#User needs to enter basic parameters such as the bounding box and optinally networks
#to download from

#Enter the directory where you want to build the data structure here
datadir=/data/dna/rmartin/Ambient_noise/Alaska/20142016test

cwd=`pwd`

if [ ! -d $datadir ]; then
    echo "The directory $datadir does not exist!"
    mkdir $datadir
fi

cd $datadir

if [ ! -d "resp_dir" ]; then 
	mkdir resp_dir
else
    rm -rf resp_dir
    mkdir resp_dir
fi 


latmin=52
latmax=73
lonmin=-171
lonmax=-123

#Not needed if we want all the available stations
NETWORK=''
#If network is not specified, all available data within a region will be obtained
#This could be a lot of data, so be careful! 

FetchScript=/data/dna/rmartin/Ambient_noise/CODES/Scripts/Fetch/FetchData_latest


years=( 2014 2015 2016 2017 2018 )
months=( `seq -w 1 12` )
days=( `seq -w 1 31` )

for year in "${years[@]}"; do 
	for month in "${months[@]}"; do
		for day in "${days[@]}"; do

			startTime="${year}-${month}-${day}T0:0:0"
			endTime="${year}-${month}-${day}T23:59:59"

			#Get all the PZ response data for the period of time
			$FetchScript -vvv -L '--' --lat ${latmin}:${latmax} --lon ${longmin}:${longmax} -C 'LHZ' -s $startTime -e $endTime -sd resp_dir

			#Actually get the data - this can be buggy, so maybe think of a better way
			$FetchScript -vvv -L '--' --lat ${latmin}:${latmax} --lon ${longmin}:${longmax} -C 'LHZ' -s $startTime -e $endTime -o ${year}_${month}_${day}.mseed -m ${year}_${month}_${day}.metafile

			mseed2sac -m ${year}_${month}_${day}.metafile ${year}_${month}_${day}.mseed

			if [ -f list.prepped ]; then 
				rm list.prepped 
			fi

			rm *.list

			for net_station in `ls *SAC | awk -F. '{printf "%s.%s ", $1,$2}' | sort | uniq`; do 
				
				ls ${net_station}.*SAC > ${net_station}.list 

				#Find appropriate pole-zero file 
				pzFile=`ls resp_dir/SACPZ.${net_station}.*.LHZ | head -n 1`

				if (( `echo $pzFile | wc | awk '{print $2}'` > 0 )); then
					#prepare the list of lists - each entry in list.prepped refers to a list of SAC files and a PZ file
					echo "${net_station}.list $pzFile" >> list.prepped
				fi

			done

			#Run the c code to do Bensen (2007) data pre-processing

			/data/dna/rmartin/Ambient_noise/CODES/src/Prep_data/bin/Prep_ANT_data_128.exe list.prepped

			#Make one directory for each day 

			currentdir=${year}/${month}/${day}
			mkdir -p $currentdir

			#all_ant_prep creates re ad im spectrum components
			#move them into the day folder we just made, and clean up
			
			mv *re $currentdir
			mv *im $currentdir
			mv *list $currentdir
			rm *SAC *mseed *metafile


		done #day loop

	done #month loop 

done #year loop 


cd $cwd

