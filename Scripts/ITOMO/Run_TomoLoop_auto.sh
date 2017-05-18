#!/bin/bash

#RMS Feb 2017
#This should be in the dispersion curves directory

#This is the 'auto' version of this script, where values of the inversion parameters are chosen by A python program and
#then input here

alpha=$1
beta=$2
sigma=$3


#parameters for the checkerboard  test
latmin=56.5
latmax=70.0
latinc=2
longmin=`echo 196.0 | awk '{print $1-360}'`
longmax=`echo 231.0 | awk '{print $1-360}'`
longinc=4
minvel=-10  
maxvel=10
avevel=5

types=( "ph" "gp" )
per=( `seq 8 40` )

for type in "${types[@]}"; do
    for per in "${per[@]}"; do 

	echo $type
	echo $per

	echo "data${per}s_${type}.txt" > param.dat
	echo "$latmin $latmax $latinc" >> param.dat
	echo "$longmin $longmax $longinc" >> param.dat
	echo "$minvel $maxvel $avevel" >> param.dat
	echo "0" >> param.dat
	echo "rays.${per}.${type}.dat" >> param.dat
	echo "checkerboard.${per}.${type}.dat" >> param.dat

create_ant_checkerboard param.dat
 
./Run_Tomo_auto.sh rays.${per}.${type}.dat $per $alpha $beta $sigma
./Run_Tomo_auto.sh data${per}s_${type}.txt $per $alpha $beta $sigma

done
done

