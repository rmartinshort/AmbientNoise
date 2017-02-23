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
	echo ""
	exit 1
fi

GRD=$1
minval=$2
maxval=$3
ps=$GRD.ps

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $2}' | awk -F "." '{print $1}')

if [ $tmpnname == 'rays' ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $2}' | awk -F "." '{print $2}')
else
	phasename=$(echo $GRD | awk -F "_" '{print $2}' | cut -c 5-)
	phasename=$phasename.RES
fi

echo $phasename

#make the color scheme
$GMT makecpt -Cpolar -T$minval/$maxval/0.01 -Z > absvels.cpt
cpt=absvels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -Y3i -P -K --PAPER_MEDIA=Custom_15ix15i > $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba0.1f0.1g0.1/:"Phase velocity [km/s]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END

#Post-processing stuff 
$GMT ps2raster $ps -P -Tf
rm *.ps
rm *.eps