#!/bin/bash

#RMS 2017
#This should be inside the dispersion curves directory

datadir=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/phase_data/

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

>>>>>>> a6e84b18331c9ff3b443a0675dcf57ea65f64e99
mkdir -p $odir

echo $dbname > params_herm.in
echo $odir >> params_herm.in


/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/src/To_Hermann/bin/dispersion_db_to_hermann.exe params_herm.in

echo "Done!"

cd $cwd
