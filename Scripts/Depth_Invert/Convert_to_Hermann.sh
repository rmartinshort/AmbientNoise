#!/bin/bash

#RMS 2017
#This should be inside the dispersion curves directory

datadir=/data/dna/rmartin/Ambient_noise/Alaska/ALL_DATA/COR/STACK/Dispersion_curves_lr


if [ ! -d $datadir ]; then 
    echo "Directory $datadir does not exist!"
    exit 1
fi


cwd=`pwd`
cd $datadir

dbname=Alaska_ant_dispersion.db
odir=../ForHermannInvert2

if [ -d $odir ]; then
    echo "Output dir already exists - deleting it now"
    rm -rf $odir
fi

mkdir -p $odir

echo $dbname > params_herm.in
echo $odir >> params_herm.in


/data/dna/rmartin/Ambient_noise/CODES/src/To_Hermann/bin/dispersion_db_to_hermann.exe params_herm.in


#Compress the full directory ready for copying
#echo "Compressing grid point directory"
#tar -zcvf ForHermanInvert.tar.gz $odir
#echo "Done!"

cd $cwd
