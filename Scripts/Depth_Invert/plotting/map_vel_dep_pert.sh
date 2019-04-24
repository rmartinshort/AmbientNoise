#!/bin/bash

##RMS 2017
#GMT script to plot phase velocity map for Alaska
GMT=gmt

$GMT gmtset BASEMAP_TYPE_FANCY
$GMT gmtset FONT_ANNOT_PRIMARY 20p,Helvetica

colscheme=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting/Generate_colscheme.py
Topo=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting/plottingdata/Alaska_lr.nc
faults=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting/plottingdata/wardfaults.dat
yakslab=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting/plottingdata/Yakutat_subducted_extent.dat
volcanoes=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting/plottingdata/Alaska_volcanoes.dat

J=B-147/56.0/52/73/10i #map projection is Albers Conic Equal Area (see GMT docs)
R=198/226/56/70 #Whole of Alaska and Adjacent Canada

if [ -z "$1" ]; then
	echo "No arguments given"
	echo ""
	exit 1
fi

GRD=$1

grdendpart=$(echo $GRD | awk -F "." '{print $2}')

if [ $grdendpart == "xyz" ]; then
	echo $GRD
	grdstartpart=$(echo $GRD | awk -F "." '{print $1}')
	phasename=$(echo $endpart | awk -F "_" '{print $4}')
	$GMT xyz2grd -I0.5 -: -R$R $GRD -Gtmp.grd
	$GMT grdsample -I0.1 -R$R tmp.grd -G$grdstartpart.grd
	GRD=$grdstartpart.grd
else 
    
    grdstartpart=$(echo $GRD | awk -F "." '{print $1}')
    echo '.grd file input'
    echo $GRD
fi

GRD1=$GRD
ps=$GRD1.ps

#Cut the grd file to a region of interest
$GMT grdsample $GRD -I0.1 -R200/224/58/68 -Gtmp.grd

GRD=tmp.grd

#Give the model a name and plot the details
MODNAME="Alaska Vs-depth"
tmpname="phase"

dep=$(echo $grdstartpart | awk -F "_" '{print $2}')

echo $dep

#make the color scheme
#median value
med=$(gmt grdinfo -L1 $GRD | sed -n '10p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-6" | bc)
maxvel=$(echo "$med+6" | bc)
echo $minvel
echo $maxvel

#Generate the appropriate color scheme
$colscheme $minvel $maxvel
cpt=vels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -Y2i -P -K --PS_MEDIA=Custom_12.0ix13.0i > $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthicker -A500 -Di -P -O -K >> $ps

#plot topography
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -S1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps


minquakedepth=$(($dep - 10))
maxquakedepth=$(($dep + 10))

#Download events of interest
$GroupTomo/WRAPPER_SCRIPTS/Get_Earthquakes.py $minquakedepth $maxquakedepth
$GMT psxy All_quakes_Alaska.dat -J$J -Rd$R -Sc0.1 -Wthinnest -Gwhite -O -K >> $ps

#Plot the extent of the subducted yakutat slab
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Plot the volcanoes
$GMT psxy $volcanoes -J$J -Rd$R -St0.6 -Gred -O -K -Wthinnest,black >> $ps

#Make the scale
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba1f1g1/:"dVs/Vs [%]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${dep} km
END

#Post-processing stuff
$GMT ps2raster $ps -P -Tf
rm *.ps
rm *.eps
evince $GRD1.pdf
