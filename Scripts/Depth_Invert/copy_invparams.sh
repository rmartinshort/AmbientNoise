#!/bin/bash


datadir=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_0.1_v4
cwd=`pwd`

cd $datadir
for dir in disp*; do

    echo $dir
    
    cp $cwd/surf_params/sobs.d $dir
    cp $cwd/surf_params/ebh/adapt/start.mod $dir
    
done
