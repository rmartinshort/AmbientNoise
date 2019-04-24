#!/bin/bash

#plotting of the stations used in this joint (or surface wave only) inversion

J=B-147/56.0/52/73/12i #map projection is Albers Conic Equal Area (see GMT docs)
R=-168/-128/56/71 #Whole of Alaska and Adjacent Canada
Topo=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting/plottingdata/Alaska_lr.nc
faults=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting/plottingdata/wardfaults.dat
yakslab=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting/plottingdata/Yakutat_subducted_extent.dat
volcanoes=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting/plottingdata/Alaska_volcanoes.dat

non_RF_stations=Alaska_stations_noRF.txt

if [ -z "$1" ]; then
    echo "No arguments given"
    echo "Supply path to the stations file you want to plot"
    exit 1
fi

stations=$1
GMT=gmt
ps=Moho_depth.ps

awk '{print $1 " " $2 " " $4}' $stations > station_names.txt
$GMT surface Moho_depth.dat -R200/224/58/69 -I0.1 -T0.6 -Gtmp.grd
$GMT grdsample tmp.grd -R200/224/58/68 -GMoho_dep.grd

GRD=Moho_dep.grd

$GMT makecpt -Cjet -T20/58/1 -Z > mohodep.cpt
cpt=mohodep.cpt

#plot the moho depth map
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -P -K -Q --PS_MEDIA=Custom_14.0ix13.0i > $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -Wthinnest -A1000 -Di -P -K -O -Slightblue >> $ps

#plot topography
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -S2000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthin,purple -O -K  >> $ps

#Plot the extent of the subducted yakutat slab
$GMT psxy $yakslab -J$J -Rd$R -Wthick -O -K >> $ps

#Plot the volcanoes
$GMT psxy $volcanoes -J$J -Rd$R -St0.4 -Gred -O -K -Wthinnest,black >> $ps

#Plot the stations
$GMT psxy $stations -J$J -Rd$R -Si0.1 -Gblack -O -K -Wthinnest,black >> $ps
$GMT pstext station_names.txt -J$J -Rd$R -F+f6p+j5+a45 -O -K  >> $ps

#Plot the stations that don't have RFs
#$GMT psxy $non_RF_stations -J$J -Rd$R -Si0.2 -Ggreen -O -K -Wthinnest,black >> $ps

#Make the scale
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -Y0.5i -E -C$cpt -Ba5f5g5/:"Moho depth [km]": -O >> $ps

#Post-processing stuff
$GMT ps2raster $ps -P -Tf
rm *.ps
rm *.eps
evince Moho_depth.pdf
