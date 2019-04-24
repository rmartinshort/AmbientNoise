#!/bin/bash

#RMS Feb 2017
#This should be in the dispersion curves directory

#parameters for the checkerboard  test
latmin=55.5
latmax=73.5
latinc=2
longmin=`echo 188.5 | awk '{print $1-360}'`
longmax=`echo 237.5 | awk '{print $1-360}'`
longinc=4
minvel=-10  
maxvel=10
avevel=5

types=( "ph" "gp" )
per=( `seq 5 40` )

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

/data/dna/rmartin/Ambient_noise/CODES/src/checkerboard/bin/create_ant_checkerboard.exe param.dat
 
./Run_Tomo.sh rays.${per}.${type}.dat $per
./Run_Tomo.sh data${per}s_${type}.txt $per

done
done
																							end 
rm *.azi *.prot *.red *.resid
