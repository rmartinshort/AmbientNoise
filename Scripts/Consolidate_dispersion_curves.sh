#!/bin/bash

#RMS Feb 2017
#This file should be in Dispersion curves directory

ProgID="SURF96"
wavetype="R"
mode="0"
unknown="X"
periodArray=( `seq 8 35` )
increment=0.1
minlat=55.5
maxlat=73.5
minlon=188.5
maxlon=237.5
snr=30
uncertainty=0.2

if [ -f Alaska_ant_dispersion.dp ]; then
    rm Alaska_ant_dispersion.dp
fi 

veltypes=( "ph" )

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
