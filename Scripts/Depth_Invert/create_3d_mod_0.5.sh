#!/bin/bash

#RMS 2017
#Script to combine the inversion results (which consider of a 1D velocity profile at each point in the domain)
#into a 3D file with lon,lat,depth,vel

#Note that when the resolution of the model is altered, you will have the change this script - edit the 0.5s

if [ -z "$1" ]; then
   #Path to directory containing the 1D velocity profiles
   datadir=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_0.5_v3_nosmooth
else
   datadir=$1
fi

if [ ! -d $datadir ]; then
   echo "Supplied directory $datadir does not exist"
fi

cwd=`pwd`
cd $datadir

#Remove a 3D model file if it already exists
if [ -f alaska.3d.mod ]; then
  rm alaska.3d.mod
fi

#Run a python script to get the min and max coordinates, and the increment

minmax=$(
python << END
import glob
import numpy as np
files=glob.glob('disp*')
lons = np.zeros(len(files))
lats = np.zeros(len(files))

i = 0
for infile in files:
  iftmp = infile.split('_')
  lon=iftmp[1]
  lat=iftmp[2]
  lons[i]=lon
  lats[i]=lat
  i += 1

maxlon = max(lons)
maxlat = max(lats)
minlon = min(lons)
minlat = min(lats)
print maxlon-0.5,maxlat-0.5,minlon+0.5,minlat+0.5
END
)

maxlon=$(echo $minmax | cut -d ' ' -f 1)
maxlat=$(echo $minmax | cut -d ' ' -f 2)
minlon=$(echo $minmax | cut -d ' ' -f 3)
minlat=$(echo $minmax | cut -d ' ' -f 4)

echo "maxlon: $maxlon"
echo "minlon: $minlon"
echo "maxlat: $maxlat"
echo "minlat: $minlat"

lats=( `seq -f %3.1f $minlat 0.5 $maxlat` )
lons=( `seq -f %3.1f $minlon 0.5 $maxlon` )

#Loop over files and append to database in from
#lon lat depth Svel

for la in "${lats[@]}"; do
  for lo in "${lons[@]}"; do

    inputfile=disp_${lo}_${la}_1D/end.mod

    if [ -f $inputfile ]; then
      awk 'BEGIN{depth=0} {if (NR >= 13) print '$lo', '$la', depth, $3} {depth=depth+$1}' $inputfile >> alaska.3d.mod
    else
      echo "No file $inputfile found"
    fi

  done

  echo "done with latitude $la"
done

#Remove depths beyond the resolution of the model (saves space)
awk '{if ($3 <= 60) print $0}' alaska.3d.mod > alaska.3d.clipped.mod
mv alaska.3d.clipped.mod alaska.3d.mod
