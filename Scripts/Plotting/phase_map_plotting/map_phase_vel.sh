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
	echo "usage: ./map.sh grdfile minvel maxvel"
	exit 1
fi

GRD=$1
minval=$2
maxval=$3
ps=$GRD.ps

#subducted plate contours
alaskaslab=/Users/rmartinshort/Documents/Berkeley/Subduction_dynamics/Slab1.0_data/allslabs/alu_slab1.0_clip.grd
#Volcano locations
volcanoes=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Alaska_volcanoes.dat
#Fault locations
faults=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/Alaska_faults.dat
#Yakutat terrane
yakslab=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Yakutat_subducted_extent.dat
#topography
topo=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/Alaska_gebco.nc

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $2}' | awk -F "." '{print $1}')

if [ $tmpnname == "rays" ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $2}' | awk -F "." '{print $2}')
	phasename=$phasename.RES
else
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | cut -c 5-)
fi

echo $phasename

#make the color scheme
med=$(gmt grdinfo -L1 $GRD | sed -n '11p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.2" | bc)
maxvel=$(echo "$med+0.2" | bc)
echo $minvel
echo $maxvel
 
#Generate the appropriate color scheme
./Generate_colscheme.py $minvel $maxvel
cpt=vels.cpt

#resample to higher resolution
$GMT grdsample $GRD -I0.1 -Gtmp.grd

#plot the image
$GMT grdimage tmp.grd -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -Y2i -P -K --PAPER_MEDIA=Custom_14.0ix13.0i > $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#plot topography 
$GMT grdcontour $topo -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Contour the alaska slab from the slab 1.0 model 
#$GMT grdcontour $alaskaslab -J$J -R$R -Wthickest,green -A100+gwhite+an -C100 -O -K >> $ps

#Plot on the faults, from http://gps.alaska.edu/programming/lab11/alaska_faults.gmt
$GMT psxy $faults -m -J$J -R$R -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot the extent of the subducted yakutat slab 
$GMT psxy $yakslab -J$J -R$R -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
$GMT psxy $volcanoes -J$J -R$R -St0.6 -Gred -Wthinnest,black -O -K -V >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -B0.1f0.1g0.1/:"Phase velocity [km/s]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END

#Post-processing stuff 
$GMT ps2raster $ps -P -Tf
rm *.ps
rm *.eps
open $GRD.pdf
