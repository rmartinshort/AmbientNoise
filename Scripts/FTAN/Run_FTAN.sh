#!/bin/bash 

#RMS 2017
#Run the FTAN program in SYM directory

#########################
#####parameter block#####
#keep:                  #
pio4=-1		#    initial phase value for noise correlations
vel_min=1.5	#    minimum possible group or phase velocity
vel_max=5		#    maximum possible group or phase velocity
fmatch=2		#    2 - parameter used in the phase-match filtering. Varying doesn't have much of an effect
snr_threshhold=30	#    minimum value for snr to consider an ok measurement
ffact=1		#    
taper=1		#    
snr=.2		# 
amp_flag=0        #    0 - do not output the amplitude space. 1 - do output the amplitude space
#adjust the min and max	#
#   period:		#
per_min=5		#    minimum period to try to measure
per_max=40	#    maximum period to try to measure
#########################


if [ "$#" == 2 ]; then
	station_lst_file=$1
	indir=$2
else
	echo "Please input the station list (list of stations in second column)"
	station_lst_file=`return_string `
	echo "Please input the directory with symmetric correlations"
	indir=`return_string `
fi

cd $indir

#stations=( `cat ${station_lst_file} | awk '{print $2}'` )
#echo $stations 
#cat $station_lst_file | awk '{print $2}'

for station in $(cat $station_lst_file | awk '{print $2}'); do
	ls COR_*_${station}_*s >> filelist_ftan
done

awk '{print "'$pio4' '$vel_min' '$vel_max' '$per_min' '$per_max' '$snr_threshhold' '$ffact' '$taper' '$snr' '$fmatch' '$amp_flag' "  $1}' filelist_ftan > param_ph.ftan.dat
/data/dna/rmartin/Ambient_noise/CODES/src/FTAN/bin/ftan_2 param_ph.ftan.dat 
