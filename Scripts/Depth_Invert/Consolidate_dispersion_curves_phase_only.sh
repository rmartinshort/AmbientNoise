#!/bin/bash

#RMS Feb 2017
#This file should be in Dispersion curves directory

periodArray=( `seq 9 38` )

#increment should match that used in the tomography setup script! 
increment=0.2
minlat=55.6
maxlat=73.4
minlon=188.6
maxlon=237.4
snr=10
uncertainty=0.2


cwd=`pwd`
datadir=/data/dna/rmartin/Ambient_noise/Alaska/ALL_DATA/COR/STACK/Dispersion_curves_10_3/

if [ ! -d $datadir ]; then 
    echo "Directory $datadir does not exist!"
    exit 1
fi

cd $datadir

if [ -f Alaska_ant_dispersion_ph.db ]; then
    rm Alaska_ant_dispersion_ph.db
fi 

veltypes=( "ph" )

#loops though the assigned periods and appends to a database of the phase/group velocity values,
#ready for the next step

for period in "${periodArray[@]}"; do
    for velocityType in "${veltypes[@]}"; do

	#echo $minlat $maxlat $minlon $maxlon
	echo $velocityType
	gmt surface data${period}s_${velocityType}.txt_${period}.1 -R${minlon}/${maxlon}/${minlat}/${maxlat} -I${increment}/${increment} -Gtmp.grd
	gmt grd2xyz tmp.grd | awk '{printf("%3.1f   %3.1f   %3.0f   %3.5f\n",$1,$2,'$period',$3)}' > tmp.${velocityType}
    done

    paste tmp.ph >> Alaska_ant_dispersion_ph.db
    echo "Done with $period"
done

cd $cwd
