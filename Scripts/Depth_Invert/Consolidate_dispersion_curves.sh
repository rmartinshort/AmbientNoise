#!/bin/bash

#RMS Feb 2017
#This file should be in Dispersion curves directory

ProgID="SURF96"
wavetype="R"
mode="0"
unknown="X"
periodArray=( `seq 8 36` )
increment=0.5
minlat=55.5
maxlat=73.5
minlon=188.5
maxlon=237.5
snr=10
uncertainty=0.2


cwd=`pwd`
datadir=/data/dna/rmartin/Ambient_noise/Alaska/ALL_DATA/COR/STACK/Dispersion_curves/

if [ ! -d $datadir ]; then 
    echo "Directory $datadir does not exist!"
    exit 1
fi

cd $datadir

if [ -f Alaska_ant_dispersion.dp ]; then
    rm Alaska_ant_dispersion.dp
fi 

veltypes=( "ph" "gp" )

#loops though the assigned periods and appends to a database of the phase/group velocity values,
#ready for the next step

for period in "${periodArray[@]}"; do
    for velocityType in "${veltypes[@]}"; do

	#echo $minlat $maxlat $minlon $maxlon
	gmt surface data${period}s_${velocityType}.txt_${period}.1 -R${minlon}/${maxlon}/${minlat}/${maxlat} -I${increment}/${increment} -Gtmp.grd
	gmt grd2xyz tmp.grd | awk '{printf("%3.1f   %3.1f   %3.0f   %3.5f\n",$1,$2,'$period',$3)}' > tmp.${velocityType}
    done

    awk '{print $4}' tmp.ph > tmp
    paste tmp.ph tmp >> Alaska_ant_disperison.db
    echo "Done with $period"
done

cd $cwd
