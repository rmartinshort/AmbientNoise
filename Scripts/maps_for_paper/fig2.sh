#!/bin/bash
#RMS Dec 2017 
#Figure 2 for ambient noise paper
#Map of station coverage in Alaska 

gmt gmtset BASEMAP_TYPE_FANCY
gmt gmtset FONT_ANNOT_PRIMARY 20

#rm *.ps

#Volcano locations 
volcanoes=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Alaska_volcanoes.dat
#Tectonic plate bpundaries
birdplates=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/bird_plates.xy
#Yakutat terrane
yakslab=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Yakutat_subducted_extent.dat
yakslab=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/YAK_extent.lonlat
#Fault locations
#faults=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/Ambient_noise_paper/fig1/Ward_faults.dat

TA=TA_stations.dat
AK=AK_stations.dat
AT=AT_stations.dat
AV=AV_stations.dat
YV=YV_stations.dat
XR=XR_stations.dat
XE=XE_stations.dat

Other=other_stations.txt

ps=fig2.ps

J=B-151.5/63/54/72/12i #map projection is Albers Conic Equal Area (see GMT docs)
R=-171/-132/54/72 #Whole of Alaska and Adjacent Canada 

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -A500/1/1 -BWSne -B3.0f3.0 -Di -P -Lf-142/71/71/400.0+l"Distance [km]" -K -Gwhite -Slightblue -Wthinnest --PS_MEDIA=Custom_14ix14i > $ps

#plot the plate margins 
gmt psxy $birdplates -Sf2.5/15p+t+l -J$J -Rd$R -Wthickest,purple -O -V -K >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K -Bg90/g180 >> $ps
#Plot the stations
gmt psxy $Other -J$J -R$R -St0.5 -Gred -Wthinnest -O -K -V >> $ps
gmt psxy $TA -J$J -R$R -St0.5 -Gblue -Wthinnest -O -K -V >> $ps
gmt psxy $AK -J$J -R$R -St0.5 -Gcyan -Wthinnest -O -K -V >> $ps
gmt psxy $AV -J$J -R$R -St0.5 -Gbrown -Wthinnest -O -K -V >> $ps
gmt psxy $YV -J$J -R$R -St0.5 -Ggreen -Wthinnest -O -K -V >> $ps
gmt psxy $XR -J$J -R$R -St0.5 -Gblack -Wthinnest -O -K -V >> $ps
gmt psxy $AT -J$J -R$R -St0.5 -Gdarkgreen -Wthinnest -O -K -V >> $ps
gmt psxy $XE -J$J -R$R -St0.5 -Ggray -Wthinnest -O -K -V >> $ps

#Make the stations legend
gmt pslegend -R$R -J$J -D-180/72.5/3i/0.3i -C0.1i/0.1i -L1.5 -O << EOF >> $ps
G -0.1i
H 15 Legend
D 0.2i 1p
N 1
V 0 1p
S 0.1i t0.4 0.5 blue thinnest,black 0.4i TA 
S 0.1i t0.4 0.5 cyan thinnest,black 0.4i AK
S 0.1i t0.4 0.5 brown thinnest,black 0.4i AV
S 0.1i t0.4 0.5 green thinnest,black 0.4i YV
S 0.1i t0.4 0.5 black thinnest,black 0.4i XR
S 0.1i t0.4 0.5 darkgreen thinnest,black 0.4i AT
S 0.1i t0.4 0.5 gray thinnest,black 0.4i XE
S 0.1i t0.4 0.5 red thinnest,black 0.4i Other

#S 0.1i t0.4 0.5 cyan thinnest,black 0.3i Current AT station
#S 0.1i t0.4 0.5 white thinnest,black 0.3i Current CN station
#S 0.1i t0.4 0.5 orange thinnest,black 0.3i Current AV station
V 0 1p
D 0.2i 1p
N 1
G 0.05i
EOF

#############Inset world#############
#gmt pscoast -Rg -JG-123/63/3.5i -B30g30 -Dc -A500 -Glightbrown -Wthinnest -Slightblue -X9.5i -Y5.8i -O -K >> $ps
#gmt psxy domain.dat -JG-123/63/3.5i -Rg -O -Wthick,red -L >> $ps
#####################################

#Post-processing stuff 
gmt ps2raster $ps -P -Tf

open fig2.pdf
