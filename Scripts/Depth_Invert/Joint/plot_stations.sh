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
ps=Used_stations.ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -Wthinnest -A500 -Di -P -K --PS_MEDIA=Custom_14.0ix13.0i -Slightblue -Glightgray > $ps

#plot topography
#$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -S1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthin,purple -O -K  >> $ps

#Plot the extent of the subducted yakutat slab
$GMT psxy $yakslab -J$J -Rd$R -Wthick -O -K >> $ps

#Plot the volcanoes
$GMT psxy $volcanoes -J$J -Rd$R -St0.4 -Gred -O -K -Wthinnest,black >> $ps

#Plot the stations
$GMT psxy $stations -J$J -Rd$R -Si0.2 -Gblack -O -Wthinnest,black >> $ps

#Plot the stations that don't have RFs
#$GMT psxy $non_RF_stations -J$J -Rd$R -Si0.2 -Ggreen -O -Wthinnest,black >> $ps

#Post-processing stuff
$GMT ps2raster $ps -P -Tf
rm *.ps
rm *.eps
evince Used_stations.pdf   



