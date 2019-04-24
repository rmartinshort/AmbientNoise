#!/bin/bash 
#RMS Dec 2017
#Figure 1 for ambient noise tomo paper
#Topography figure and terrane labels 

#Need to put on WVF region label

gmt gmtset BASEMAP_TYPE_FANCY
gmt gmtset FONT_ANNOT_PRIMARY 20

#Topography
GRD=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/Alaska_gebco.nc
#Volcano locations 
volcanoes=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Alaska_volcanoes.dat
#Tectonic plate bpundaries
birdplates=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/bird_plates.xy
#Yakutat terrane
#yakslab=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Yakutat_subducted_extent.dat
yakslab=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/YAK_extent.lonlat
#Fault locations
faults=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/Ambient_noise_paper/fig1/Ward_faults.dat
#Subducted plate contours
alaskaslab=/Users/rmartinshort/Documents/Berkeley/Subduction_dynamics/Slab1.0_data/allslabs/alu_slab1.0_clip.grd
CPT=/Users/rmartinshort/Documents/Berkeley/Useful_GMT_data/colormaps/mby.cpt

ps=fig1.ps

J=B-151.5/63/54/72/12i #map projection is Albers Conic Equal Area (see GMT docs)
R=-171/-132/54/72 #Whole of Alaska and Adjacent Canada 

#Make gradient file for lighting effects
gmt grdgradient $GRD -Nt1 -A0/90 -Ggrad_i.nc

#Make the image, using the mby.cpt color scheme
gmt grdimage $GRD -Igrad_i.nc -C$CPT -R$R -J$J -K --PS_MEDIA=Custom_12ix14i > $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -B3.0f3.0 -Dh -A500/1/1 -P -K -O -W0.0 >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K -Bg90/g180 >> $ps

#Contour the alaska slab from the slab 1.0 model 
gmt grdcontour $alaskaslab -J$J -R$R -Wthickest,green -C20 -O -K >> $ps

#Plot on the faults, from Ward paper
gmt psxy $faults -J$J -R$R -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#plot the plate margins 
gmt psxy $birdplates -Sf2.5/15p+t+l -J$J -Rd$R -Wthickest,purple -O -V -K >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$J -R$R -St0.4 -Gred -Wthinnest -O -K -V >> $ps

#Plot plate motion vectors
#gmt pstext -R$R -J$J -Gwhite -O -N -K -C0.2/1 -TO -Wthickest,black >> $ps << END
#-146 57 16 0.0 0 TL APM (20 mm/year)
#END

#absolute plate motions
#gmt psxy <<END -: -SV0.2i+e+g -Gblack -Wthickest,black -J$J -R$R -O -V -K >> $ps
#57 -147 329.87 3.94
#54 205 322.73 4.74
#58 219 332.94 3.63
#56.2 -144 90 1.6
#END

#Plot the names of the various terranes
gmt pstext -R$R -J$J -Gwhite -O -N -K >> $ps << END
-147 62.5 16 0.0 0 TL WCT
-147 64.5 16 0.0 0 TL YCT
-147 61 16 0.0 0 TL SMCT
-153 68.5 16 0.0 0 TL AAT
-156 66.0 16 0.0 0 TL ODT
END

#############Inset world#############
gmt pscoast -Rg -JG-123/63/3.5i -B30g30 -Dc -A2000 -Glightbrown -Wthinnest -Slightblue -X8.5i -Y5.0i -O -K >> $ps
gmt psxy domain.dat -JG-123/63/3.5i -Rg -O -Wthicker,red -L >> $ps
#####################################

#Make the scale 
#gmt psscale -D2.3i/-0.4i/4.0i/0.2ih -E -C$CPT -Ba2000f2000g2000/:"Surface height [m]": -O >> $ps

#Post-processing stuff - make pdf
gmt ps2raster $ps -P -Tf

open fig1.pdf

