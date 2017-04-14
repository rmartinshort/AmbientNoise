#!/bin/bash

#RMS 2017
#This should be inside the dispersion curves directory

datadir=/data/dna/rmartin/Ambient_noise/Alaska/ALL_DATA/COR/STACK/Dispersion_curves

if [ ! -d $datadir ]; then 
    echo "Directory $datadir does not exist!"
    exit 1
fi


cwd=`pwd`
cd $datadir

dbname=Alaska_ant_dispersion.db
odir=../ForHermannInvert

mkdir -p $odir

echo $dbname > params_herm.in
echo $odir >> params_herm.in


/data/dna/rmartin/Ambient_noise/CODES/src/To_Hermann/bin/dispersion_db_to_hermann.exe params_herm.in


#Compress the full directory ready for copying
cd ../
echo "Compressing grid point directory"
tar -zcvf ForHermanInvert.tar.gz ForHermannInvert
echo "Done!"

cd $cwd
