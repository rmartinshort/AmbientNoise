#!/bin/bash
#RMS Jan 2018
#For Ambient noise paper fig4 - plot the location of a station of interest on an alaska map


gmt gmtset BASEMAP_TYPE_FANCY
gmt gmtset FONT_ANNOT_PRIMARY 20


#Volcano locations 
volcanoes=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Alaska_volcanoes.dat
#Tectonic plate bpundaries
birdplates=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/bird_plates.xy
#station location
stloc=station_location.dat

J=B-151.5/63/54/72/8i #map projection is Albers Conic Equal Area (see GMT docs)
R=-171/-132/54/72 #Whole of Alaska and Adjacent Canada 

ps=station_map.ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -A500 -BWSne -B6.0f6.0g6.0 -Di -A500/1/1 -P -Lf-142/71/71/400.0+l"Distance [km]" -K -Glightgray -Slightblue -Wthinnest --PS_MEDIA=Custom_14ix14i > $ps

#Plot on the plate boundaries
gmt psxy $birdplates -J$J -Rd$R -Wthick,purple -Sf2.5/15p+t+l -O -V -K -Bg90/g180 >> $ps

gmt psxy $stloc -J$J -R$R -St0.7 -Gred -Wthinnest -O -V >> $ps

#Post-processing stuff 
gmt ps2raster $ps -P -Tf

open station_map.pdf
