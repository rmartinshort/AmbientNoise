#!/bin/bash 
#RMS 2018
#Basic map of all events since 1970 in Alaska. Events will be colored by their depth.

#This was created to address the reviewer concern that the 'gap' between the eastern edge of the Alaskan WBZ and the eastern
#edge of the slab might not be resolvable. The reviewer didn't really know what they were talking about. However this map might 
#stll be of interest and use to some people.

gmt gmtset BASEMAP_TYPE_FANCY
gmt gmtset FONT_ANNOT_PRIMARY 16

#Subducted plate contours
alaskaslab=/Users/rmartinshort/Documents/Berkeley/Subduction_dynamics/Slab1.0_data/allslabs/alu_slab1.0_clip.grd
#Topography
GRD=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/Alaska_gebco.nc
#Volcano locations 
volcanoes=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Alaska_volcanoes.dat
#Tectonic plate bpundaries
birdplates=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/bird_plates.xy
#Earthquakes
events=Events_since_1970.dat
#Wech tremor
tremor=tremor_locs.txt
#Yakutat terrane
yakslab=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/YAK_extent.lonlat
#Fault locations
faults=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/Ambient_noise_paper/fig1/Ward_faults.dat
#Volcano locations 
volcanoes=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Alaska_volcanoes.dat

J=B-149.5/61.5/57/76/12i #map projection is Albers Conic Equal Area (see GMT docs)
R=-160/-139/57/66 #Whole of Alaska and Adjacent Canada 

#Extract the earthquake data to plot
awk -F' ' '{print $1,$2,$3/1000}' $events > tmp_events

ps=earthquake_map.ps

#Make gradient file for lighting effects
gmt grdgradient $GRD -Nt1 -A0/270 -Ggrad_i.nc

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -B2.0f2.0 -Dh -A100/1/1 -Slightblue -Glightgray -P -K -Wthinnest --PS_MEDIA=Custom_16ix14i > $ps

#plot the transparent topography 
gmt grdimage $GRD -Igrad_i.nc -C255,255 -t50 -R$R -J$J -K -O >> $ps 

#Run pscoast to generate the coastlines (overlay)
gmt pscoast -Rd$R -J$J -BWSne -B2.0f2.0 -Dh -A100/1/1 -P -Lf-144.5/64.5/64.5/200.0+l"Distance [km]" -K -Wthinnest -O >> $ps

#Contour the alaska slab from the slab 1.0 model 
gmt grdcontour $alaskaslab -J$J -R$R -Wthickest,blue -C20 -O -K >> $ps

#Plot all events
gmt psxy tmp_events -J$J -Rg$R -Sc0.2 -Wthinnest -t80 -Cneis.cpt -O -K >> $ps

#Plot the Wech tremor region
gmt psxy $tremor -J$J -Rd$R -Sc0.01 -Gred -Wthin,red -O -K >> $ps

#plot the plate margins 
gmt psxy $birdplates -Sf2.5/15p+t+l -J$J -Rd$R -Wthickest,purple -O -V -K >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot on the faults, from Ward paper
#gmt psxy $faults -J$J -R$R -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$J -R$R -St0.4 -Gred -Wthinnest -O -K -V >> $ps

#Make the legend
gmt pslegend -R$R -J$J -D-155.5/65.2/3i/0.3i -C0.1i/0.1i -L1.5 -O << EOF >> $ps
G -0.1i
H 15 Legend
D 0.2i 1p
N 1
V 0 1p
S 0.1i c0.3 0.5 orange thinnest,black 0.4i 20-50km
S 0.1i c0.3 0.5 yellow thinnest,black 0.4i 50-100km
S 0.1i c0.3 0.5 green thinnest,black 0.4i 100-150km
S 0.1i c0.3 0.5 blue thinnest,black 0.4i 150-200km
V 0 1p
D 0.2i 1p
N 1
G 0.05i
EOF


#Post-processing stuff - make pdf
gmt ps2raster $ps -P -Tf

open earthquake_map.pdf


