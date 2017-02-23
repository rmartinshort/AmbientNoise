#!/bin/bash

#RMS 2017
#This should be inside the dispersion curves directory


dbname=Alaska_ant_dispersion.db
odir=ForHermannInvert

mkdir -p $odir

echo $dbname > params_herm.in
echo $odir >> params_herm.in

/data/dna/rmartin/Ambient_noise/Alaska/bin/Dispersion_dp_2_hermann.exe params_herm.in
