#!/bin/bash

#----------------------------------------------
# RMS 2017
#----------------------------------------------

#Script to make list of stations for which we have ambient noise data

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
