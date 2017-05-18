#!/bin/bash 

##RMS 2017
#GMT script to plot phase velocity map for Alaska
GMT=gmt4

gmt gmtset BASEMAP_TYPE_FANCY
gmt set FONT_ANNOT_PRIMARY 20p,Helvetica


J=B-147/56.0/52/73/12i #map projection is Albers Conic Equal Area (see GMT docs)
R=-168/-128/56/71 #Whole of Alaska and Adjacent Canada 


if [ -z "$1" ]; then
	echo "No arguments given"
	echo "usage: ./map.sh data_directory [example_data]"
	exit 1
fi

cwd=`pwd`
dir=$1
cd $dir

ls

for file in data*; do 

echo 'Dealing with' $file

Pfile=$file
ps=$Pfile.ps

MODNAME=$Pfile

#Generate the station-station pairs file
../Extract_paths.py $Pfile
SSpairs=SS_pair_paths.dat
Scoords=S_coords.dat

echo 'Done extracting station pairs: Now plotting'

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -K --PAPER_MEDIA=Custom_14ix12i -Ba8.0f2.0/a4.0f2.0 -Lf-144/57/57/400.0+l"Distance [km]" -Glightgray -Slightblue > $ps

#Plot the station-station pairs 
$GMT psxy $SSpairs -R$R -J$J -m -Wthinnest,blue -O -K >> $ps

#Plot the stations coords 
$GMT psxy $Scoords -R$R -J$J -St0.5 -Gred -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} 
END

#Post-processing stuff 
$GMT ps2raster $ps -P -Tf
rm *.ps
rm *.eps

done

cd $pwd
