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
welch=Welch_tremor.dat 
tremor=tremor_locs.txt
splitting=ALL_STACKS.dat


J=B-147/56.0/52/73/10i #map projection is Albers Conic Equal Area (see GMT docs)
R=198/226/56/70 #Whole of Alaska and Adjacent Canada

if [ -z "$1" ]; then
	echo "No arguments given"
	echo ""
	exit 1
fi

GRD=$1
profiles=True

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
$GMT grdsample $GRD -I0.1 -R198/224/58/68.5 -Gtmp.grd

GRD=tmp.grd

#Give the model a name and plot the details
MODNAME="Alaska Vs-depth"
tmpname="phase"

dep=$(echo $grdstartpart | awk -F "_" '{print $2}')

echo $dep

#make the color scheme
#median value
med=$(gmt grdinfo -L1 $GRD | sed -n '11p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.4" | bc)
maxvel=$(echo "$med+0.4" | bc)
echo $minvel
echo $maxvel

#Generate the appropriate color scheme
$colscheme $minvel $maxvel
cpt=vels.cpt

#plot the coastlines
$GMT pscoast -R$R -J$J -Wthick -A1000 -Di -P -K -Glightgray -Slightblue -Y2i --PS_MEDIA=Custom_12.0ix13.0i > $ps 

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -K -O -Q >> $ps

#plot the coastlines (over the top of the image)
$GMT pscoast -R$R -J$J -Wthick -A1000 -Di -P -K -O  >> $ps

#plot topography
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -S1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps


minquakedepth=$(($dep - 10))
maxquakedepth=$(($dep + 10))

#Download events of interest
#$GroupTomo/WRAPPER_SCRIPTS/Get_Earthquakes.py $minquakedepth $maxquakedepth
#$GMT psxy All_quakes_Alaska.dat -J$J -Rd$R -Sc0.1 -Wthinnest -Gblack -O -K >> $ps

#Plot the extent of the subducted yakutat slab
#$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Plot the volcanoes
$GMT psxy $volcanoes -J$J -Rd$R -St0.6 -Gorange -O -K -Wthinnest,black >> $ps

#Plot outline of the Welch tremor region (relevant for 50-80km depths)
#$GMT psxy $tremor -J$J -Rd$R -Sc0.01 -Gred -Wthinnest,red -O -K >> $ps

#if [ $profiles == 'True' ]; then

   #Center of the YAK region
#   pointsfile=tmppoints1.xyp
#   $GMT project -C-155/65.2 -E-143/59.2 -G20 -Q > $pointsfile
#   $GMT psxy $pointsfile -J$J -Rd$R -Wthick -O -K >> $ps

   #Volcanoes region
#   pointsfile=tmppoints1.xyp
#   $GMT project -C-158/64.0 -E-147/58.8 -G20 -Q > $pointsfile
#   $GMT psxy $pointsfile -J$J -Rd$R -Wthick -O -K >> $ps

   #Crossing the WVB
#   pointsfile=tmppoints1.xyp
#   $GMT project -C-142.6/64.0 -E-146.0/59.0 -G20 -Q > $pointsfile
#   $GMT psxy $pointsfile -J$J -Rd$R -Wthick -O -K >> $ps

#fi

#Plot Clo shear wave splits
#awk '{print $1,$2,$3,$4/1.3}' $splitting > dat.splits.all
#$GMT psxy dat.splits.all -SV0.1 -Gblack -Wthick,black -J$J -Rd$R -O -V -K >> $ps

#Make the scale
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba0.1f0.1g0.1/:"Vs [km/s]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${dep} km
END

#Post-processing stuff
$GMT ps2raster $ps -P -Tf
rm *.ps
rm *.eps
evince $GRD1.pdf
