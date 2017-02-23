#!/bin/bash

#Run the packing and correlation over all years/months

StationList=station.list
basedir=$(pwd)
npts=43201
startday=01
endday=31
deleteFlag=0
db=tmp.db
lag=5000
outputCorDir=COR
stackFlag=1
meanFlag=0
sdFlag=0

years=( `seq -w  2013 2017` )
months=( `seq -w  1 12` )

years=( '2015' )

for year in "${years[@]}"; do
    for month in "${months[@]}"; do

	if [ -f "all_data_for_correlations.dat" ]; then
    		rm all_data_for_correlations.dat
	fi

	if [ -f $db ]; then
    		rm $db
	fi

	#Determine the end day value within each month
	echo ${year}/${month}
	cd ${year}/${month}
	find . -empty -type d -delete #deletes empty directories
	endday=`ls -d [0-9][0-9] -v | tail -1`
	cd ../../

	echo $endday


        #./run_pack_spectrums.tcsh $year $month $startday $endday $StationList $basedir $db $deleteFlag $npts
	#./run_cor_rp_single_spect.tcsh $db $outputCorDir $lag $stackFlag $meanFlag $sdFlag $basedir

	echo $db
	echo $outputCorDir
	echo $lag
	echo $stackFlag
	echo $meanFlag
	echo $sdFlag
	echo $basedir 

	./Pack_Spectrums.sh $year $month $startday $endday $StationList $basedir $db $deleteFlag $npts

	echo $db
	echo $outputCorDir
	echo $lag
	echo $stackFlag
	echo $meanFlag
	echo $sdFlag
	echo $basedir
	./Run_Correlation_Single_Spect.sh $db $outputCorDir $lag $stackFlag $meanFlag $sdFlag $basedir

    done
done
