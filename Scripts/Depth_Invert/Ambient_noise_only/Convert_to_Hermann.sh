#!/bin/bash

#RMS 2017
#This should be inside the dispersion curves directory

datadir=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/phase_data/
datadir=/data/dna/rmartin/Ambient_noise/Alaska/ALL_DATA/COR/STACK/Dispersion_curves_10_3/ambient_surface_obs/

if [ ! -d $datadir ]; then
    echo "Directory $datadir does not exist!"
    exit 1
fi


cwd=`pwd`
cd $datadir

dbname=Alaska_ALL.db
odir=../../hermtest

if [ -d $odir ]; then
    echo "Output dir already exists - deleting it now"
    rm -rf $odir
fi

mkdir -p $odir

echo $dbname > params_herm.in
echo $odir >> params_herm.in


/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/src/To_Hermann/bin/dispersion_db_to_hermann.exe params_herm.in

echo "Done!"

cd $cwd
