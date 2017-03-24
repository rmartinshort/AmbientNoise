#!/bin/bash

#----------------------------------------------
# RMS 2017
#----------------------------------------------

#Script to make list of stations for which we have ambient noise data
#This file, station.list, gets used in various other scripts during the ambient noise processing
#workflow

datadir="/data/dna/rmartin/Ambient_noise/Alaska/ALL_DATA"

currendir=`pwd`

if [ ! -d $datadir ]; then
        echo "The directory $datadir does not exist!"
	exit 1
fi

cd $datadir

years=( 'seq 2013 2017' )
years=( '2015' )
months=( `seq -w 1 12` )
days=( `seq -w 1 31` )

for year in "${years[@]}"; do
	for month in "${months[@]}"; do
		for day in "${days[@]}"; do

			if [ -d "${year}/${month}/${day}" ]; then 

				ls ${year}/${month}/${day}/*re | awk -F/ '{print $4}' | awk -F. '{print $1}' | awk -F_ '{print $1, $2, $3}' >> station.list.unsort

			fi
		done
	done
done

cat station.list.unsort | sort | uniq > station.list
rm station.list.unsort

cd $currentdir
