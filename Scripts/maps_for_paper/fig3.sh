#!/bin/bash
#RMS 2017
#Figure 3 of ambient noise paper
#Series of phase velocity maps from ambient noise and teleseismic surface wave tomographt workflows, with WARD 

gmt gmtset BASEMAP_TYPE_FANCY
gmt gmtset FONT_ANNOT_PRIMARY 20

#Topography
GRD=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/Alaska_gebcoLR.nc
#Volcano locations 
volcanoes=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Alaska_volcanoes.dat
#Tectonic plate bpundaries
birdplates=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/bird_plates.xy
#Yakutat terrane
yakslab=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/YAK_extent.lonlat
#Fault locations
faults=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/Ambient_noise_paper/fig1/Ward_faults.dat
#colorscheme
cptscript=/Users/rmartinshort/Documents/Berkeley/Alaska/Ambient_noise/Tomo/phase_vels_Alaska/snr10_dist3_WARD/TA_only/Generate_colscheme.py

ps=fig3_2018.ps
J=B-151.5/63/54/72/12i #map projection is Albers Conic Equal Area (see GMT docs)
R=-168/-134.5/57/69 #Whole of Alaska and Adjacent Canada 

#Generate mask
maskfile=mask_coords.dat
gmt grdmask $maskfile -Gmask.grd -R188.5/237.5/51.5/73.5 -I0.1 -NNaN/1/1


#=================================================
#plot 1: amb 10s
#=================================================

amb1=2018_ant/temp_data10s_ph.txt.grd
ward1=Ward_ANT_10s.dat.grd

#apply the mask
gmt grdsample $amb1 -I0.1 -Gtmp.grd
gmt grdmath tmp.grd mask.grd MUL = masked_grd.grd

#Generate colorscheme for both plots
med=$(gmt grdinfo -L1 $amb1 | sed -n '10p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.25" | bc)
maxvel=$(echo "$med+0.25" | bc)
echo $minvel
echo $maxvel

$cptscript $minvel $maxvel
CPT=vels.cpt

#Run pscoast first to generate background
gmt pscoast -Rd$R -J$J -Bwsne -Glightgray -Slightblue -K -Di -A500/1/1 -W0.0 --PS_MEDIA=Custom_40.0ix28.0i -Y28i > $ps

#Make the image, using the tomo color scheme
gmt grdimage masked_grd.grd -C$CPT -R$R -J$J -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -B3.0f3.0 -Di -P -A500/1/1 -K -O -W0.0 >> $ps

#plot topography 
#gmt grdcontour $GRD -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults, from http://gps.alaska.edu/programming/lab11/alaska_faults.gmt
gmt psxy $faults -J$J -Rd$R -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$J -R$R -St0.5 -Gblue -Wthinnest -O -V -K >> $ps

#plot ward domain
gmt psxy ward_domain.dat -Am -J$J -R$R -O -W8,red -L -K >> $ps

#Plot the names of the various terranes
gmt pstext -R$R -J$J -Gwhite -O -N -K >> $ps << END
-147 62.5 16 0.0 0 TL WCT
-147 64.5 16 0.0 0 TL YCT
-147 61 16 0.0 0 TL SMCT
-153 68.5 16 0.0 0 TL AAT
-156 66.0 16 0.0 0 TL ODT
END

#-----------------------------------------
#plot the tomography of Ward (2015)
#-----------------------------------------

Rward=-157/-138/58.75/65.0 #Whole of Alaska and Adjacent Canada 
Jward=B-147/62.5/52/73/6i

#Make the image, using the tomo color scheme
gmt grdimage $ward1 -C$CPT -R$Rward -J$Jward -K -X-0.5i -Y6.5i -O >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$Rward -J$Jward -Bwsne -B3.0f3.0 -A500/1/1 -Di -P -K -O -Wthinnest >> $ps

#plot topography 
#gmt grdcontour $GRD -J$Jward -R$Rward -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults, from http://gps.alaska.edu/programming/lab11/alaska_faults.gmt
gmt psxy $faults -J$Jward -Rd$Rward -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$Jward -Rd$Rward -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$Jward -R$Rward -St0.4 -Gblue -Wthinnest -O -V -K >> $ps

#Plot the names of the various terranes
gmt pstext -R$Rward -J$Jward -Gwhite -O -N -K >> $ps << END
-147 62.5 16 0.0 0 TL WCT
-147 64.5 16 0.0 0 TL YCT
-147 61 16 0.0 0 TL SMCT
END

#Make the scale 
gmt psscale -D0i/0i/4.0i/0.2ih -X2.5i -Y-7.5i -E -C$CPT -Ba0.1f0.1g0.1/:"Ambient noise phase velocity [km/s]": -O -K >> $ps

#=================================================
#plot 2: amb 25s
#=================================================

amb1=2018_ant/temp_data25s_ph.txt.grd
ward1=Ward_ANT_25s.dat.grd

#apply the mask
gmt grdsample $amb1 -I0.1 -Gtmp.grd
gmt grdmath tmp.grd mask.grd MUL = masked_grd.grd

#Generate colorscheme for both plots
med=$(gmt grdinfo -L1 $amb1 | sed -n '10p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.2" | bc)
maxvel=$(echo "$med+0.2" | bc)
echo $minvel
echo $maxvel

$cptscript $minvel $maxvel
CPT=vels.cpt

#Run pscoast first to generate background
gmt pscoast -Rd$R -J$J -Bwsne -Glightgray -Di -A500/1/1 -Slightblue -K -O -W0.0 -X12i -Y1i >> $ps

#Make the image, using the tomo color scheme
gmt grdimage masked_grd.grd -C$CPT -R$R -J$J -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -B3.0f3.0 -Di -P -A500/1/1 -K -O -W0.0 >> $ps

#plot topography 
#gmt grdcontour $GRD -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults, from http://gps.alaska.edu/programming/lab11/alaska_faults.gmt
gmt psxy $faults -J$J -Rd$R -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$J -R$R -St0.5 -Gblue -Wthinnest -O -V -K >> $ps

#plot ward domain
gmt psxy ward_domain.dat -J$J -R$R -O -W8,red -Am -L -K >> $ps

#Plot the names of the various terranes
gmt pstext -R$R -J$J -Gwhite -O -N -K >> $ps << END
-147 62.5 16 0.0 0 TL WCT
-147 64.5 16 0.0 0 TL YCT
-147 61 16 0.0 0 TL SMCT
-153 68.5 16 0.0 0 TL AAT
-156 66.0 16 0.0 0 TL ODT
END

#-----------------------------------------
#plot the tomography of Ward (2015)
#-----------------------------------------

Rward=-157/-138/58.75/65.0 #Whole of Alaska and Adjacent Canada 
Jward=B-147/62.5/52/73/6i

#Make the image, using the tomo color scheme
gmt grdimage $ward1 -C$CPT -R$Rward -J$Jward -K -X-0.5i -Y6.5i -O >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$Rward -J$Jward -Bwsne -B3.0f3.0 -A500/1/1 -Di -P -K -O -Wthinnest >> $ps

#plot topography 
#gmt grdcontour $GRD -J$Jward -R$Rward -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults, from http://gps.alaska.edu/programming/lab11/alaska_faults.gmt
gmt psxy $faults -J$Jward -Rd$Rward -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$Jward -Rd$Rward -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$Jward -R$Rward -St0.4 -Gblue -Wthinnest -O -V -K >> $ps

#Plot the names of the various terranes
gmt pstext -R$Rward -J$Jward -Gwhite -O -N -K >> $ps << END
-147 62.5 16 0.0 0 TL WCT
-147 64.5 16 0.0 0 TL YCT
-147 61 16 0.0 0 TL SMCT
END

#Make the scale 
gmt psscale -D0i/0i/4.0i/0.2ih -X2.5i -Y-7.5i -E -C$CPT -Ba0.1f0.1g0.1/:"Ambient noise phase velocity [km/s]": -O -K >> $ps


#=================================================
#plot 3: amb 35s
#=================================================

amb1=2018_ant/temp_data35s_ph.txt.grd
ward1=Ward_ANT_35s.dat.grd

#apply the mask
gmt grdsample $amb1 -I0.1 -Gtmp.grd
gmt grdmath tmp.grd mask.grd MUL = masked_grd.grd

#Generate colorscheme for both plots
med=$(gmt grdinfo -L1 $amb1 | sed -n '10p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.2" | bc)
maxvel=$(echo "$med+0.2" | bc)
echo $minvel
echo $maxvel

$cptscript $minvel $maxvel
CPT=vels.cpt

#Run pscoast first to generate background
gmt pscoast -Rd$R -J$J -Bwsne -Glightgray -Di -A500/1/1 -Slightblue -K -O -W0.0 -X-16i -Y-12i >> $ps

#Make the image, using the tomo color scheme
gmt grdimage masked_grd.grd -C$CPT -R$R -J$J -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -B3.0f3.0 -A500/1/1 -Di -P -K -O -W0.0 >> $ps

#plot topography 
#gmt grdcontour $GRD -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults, from http://gps.alaska.edu/programming/lab11/alaska_faults.gmt
gmt psxy $faults -J$J -Rd$R -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$J -R$R -St0.5 -Gblue -Wthinnest -O -V -K >> $ps

#plot ward domain
gmt psxy ward_domain.dat -J$J -R$R -O -W8,red -Am -L -K >> $ps

#Plot the names of the various terranes
gmt pstext -R$R -J$J -Gwhite -O -N -K >> $ps << END
-147 62.5 16 0.0 0 TL WCT
-147 64.5 16 0.0 0 TL YCT
-147 61 16 0.0 0 TL SMCT
-153 68.5 16 0.0 0 TL AAT
-156 66.0 16 0.0 0 TL ODT
END

#-----------------------------------------
#plot the tomography of Ward (2015)
#-----------------------------------------

Rward=-157/-138/58.75/65.0 #Whole of Alaska and Adjacent Canada 
Jward=B-147/62.5/52/73/6i

#Make the image, using the tomo color scheme
gmt grdimage $ward1 -C$CPT -R$Rward -J$Jward -K -X-0.5i -Y6.5i -O >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$Rward -J$Jward -Bwsne -B3.0f3.0 -Di -P -A500/1/1 -K -O -Wthinnest >> $ps

#plot topography 
#gmt grdcontour $GRD -J$Jward -R$Rward -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults, from http://gps.alaska.edu/programming/lab11/alaska_faults.gmt
gmt psxy $faults -J$Jward -Rd$Rward -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$Jward -Rd$Rward -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$Jward -R$Rward -St0.4 -Gblue -Wthinnest -O -V -K >> $ps

#Plot the names of the various terranes
gmt pstext -R$Rward -J$Jward -Gwhite -O -N -K >> $ps << END
-147 62.5 16 0.0 0 TL WCT
-147 64.5 16 0.0 0 TL YCT
-147 61 16 0.0 0 TL SMCT
END

#Make the scale 
gmt psscale -D0i/0i/4.0i/0.2ih -X2.5i -Y-7.5i -E -C$CPT -Ba0.1f0.1g0.1/:"Ambient noise phase velocity [km/s]": -O -K >> $ps

#=================================================
#plot 4: tele 45s
#=================================================

amb1=Tele_data45s_ph.txt_45.1.grd

#Remake the mask.grd file for teleseismic tomography
gmt grdmask $maskfile -Gmask.grd -R198/226/57/70 -I0.1 -NNaN/1/1

#apply the mask
gmt grdsample $amb1 -I0.1 -Gtmp.grd
gmt grdmath tmp.grd mask.grd MUL = masked_grd.grd

#Generate colorscheme for both plots
med=$(gmt grdinfo -L1 $amb1 | sed -n '11p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.2" | bc)
maxvel=$(echo "$med+0.2" | bc)
echo $minvel
echo $maxvel

$cptscript $minvel $maxvel
CPT=vels.cpt

#Run pscoast first to generate background
gmt pscoast -Rd$R -J$J -Bwsne -Glightgray -Slightblue -Di -A500/1/1 -K -O -W0.0 -X12i -Y1i >> $ps

#Make the image, using the tomo color scheme
gmt grdimage masked_grd.grd -C$CPT -R$R -J$J -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -B3.0f3.0 -A500/1/1 -Di -P -K -O -W0.0 >> $ps

#plot topography 
#gmt grdcontour $GRD -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults, from http://gps.alaska.edu/programming/lab11/alaska_faults.gmt
gmt psxy $faults -J$J -Rd$R -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$J -R$R -St0.5 -Gblue -Wthinnest -O -V -K >> $ps

#Plot the names of the various terranes
gmt pstext -R$R -J$J -Gwhite -O -N -K >> $ps << END
-147 62.5 16 0.0 0 TL WCT
-147 64.5 16 0.0 0 TL YCT
-147 61 16 0.0 0 TL SMCT
-153 68.5 16 0.0 0 TL AAT
-156 66.0 16 0.0 0 TL ODT
END

#Make the scale 
gmt psscale -D0i/0i/4.0i/0.2ih -X2i -Y-1i -E -C$CPT -Ba0.1f0.1g0.1/:"Teleseismic earthquake phase velocity [km/s]": -O -K >> $ps


#=================================================
#plot 5: tele 75s
#=================================================

amb1=Tele_data75s_ph.txt_75.1.grd

#apply the mask
gmt grdsample $amb1 -I0.1 -Gtmp.grd
gmt grdmath tmp.grd mask.grd MUL = masked_grd.grd

#Generate colorscheme for both plots
med=$(gmt grdinfo -L1 $amb1 | sed -n '11p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.2" | bc)
maxvel=$(echo "$med+0.2" | bc)
echo $minvel
echo $maxvel

$cptscript $minvel $maxvel
CPT=vels.cpt

#Run pscoast first to generate background
gmt pscoast -Rd$R -J$J -Bwsne -Glightgray -Slightblue -Di -A500/1/1 -K -O -W0.0 -X-16i -Y-12i >> $ps

#Make the image, using the tomo color scheme
gmt grdimage masked_grd.grd -C$CPT -R$R -J$J -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -B3.0f3.0 -Di -P -A500/1/1 -K -O -W0.0 >> $ps

#plot topography 
#gmt grdcontour $GRD -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults, from http://gps.alaska.edu/programming/lab11/alaska_faults.gmt
gmt psxy $faults -J$J -Rd$R -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$J -R$R -St0.5 -Gblue -Wthinnest -O -V -K >> $ps

#Plot the names of the various terranes
gmt pstext -R$R -J$J -Gwhite -O -N -K >> $ps << END
-147 62.5 16 0.0 0 TL WCT
-147 64.5 16 0.0 0 TL YCT
-147 61 16 0.0 0 TL SMCT
-153 68.5 16 0.0 0 TL AAT
-156 66.0 16 0.0 0 TL ODT
END

#Make the scale 
gmt psscale -D0i/0i/4.0i/0.2ih -X2i -Y-1i -E -C$CPT -Ba0.1f0.1g0.1/:"Teleseismic earthquake phase velocity [km/s]": -O -K >> $ps

#=================================================
#plot 6: tele 100s
#=================================================

amb1=Tele_data100s_ph.txt_100.1.grd
gmt grdmask $maskfile -Gmask.grd -R198/226/57/70 -I0.1 -NNaN/1/1

#apply the mask
gmt grdsample $amb1 -I0.1 -Gtmp.grd
gmt grdmath tmp.grd mask.grd MUL = masked_grd.grd

#Generate colorscheme for both plots
med=$(gmt grdinfo -L1 $amb1 | sed -n '11p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.2" | bc)
maxvel=$(echo "$med+0.2" | bc)
echo $minvel
echo $maxvel

$cptscript $minvel $maxvel
CPT=vels.cpt

#Run pscoast first to generate background
gmt pscoast -Rd$R -J$J -Bwsne -Glightgray -Slightblue -Di -A500/1/1 -K -O -W0.0 -X12i -Y1i >> $ps

#Make the image, using the tomo color scheme
gmt grdimage masked_grd.grd -C$CPT -R$R -J$J -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -B3.0f3.0 -Di -P -A500/1/1 -K -O -W0.0 >> $ps

#plot topography 
#gmt grdcontour $GRD -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults, from http://gps.alaska.edu/programming/lab11/alaska_faults.gmt
gmt psxy $faults -J$J -Rd$R -Wthickest,purple -O -V -K -Bg90/g180 >> $ps

#Plot the extent of the subducted yakutat slab 
gmt psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K -Bg90/g180 >> $ps

#Plot all volcanoes
gmt psxy $volcanoes -J$J -R$R -St0.5 -Gblue -Wthinnest -O -V -K >> $ps

#Plot the names of the various terranes
gmt pstext -R$R -J$J -Gwhite -O -N -K >> $ps << END
-147 62.5 16 0.0 0 TL WCT
-147 64.5 16 0.0 0 TL YCT
-147 61 16 0.0 0 TL SMCT
-153 68.5 16 0.0 0 TL AAT
-156 66.0 16 0.0 0 TL ODT
END

#Make the scale 
gmt psscale -D0i/0i/4.0i/0.2ih -X2i -Y-1i -E -C$CPT -Ba0.1f0.1g0.1/:"Teleseismic earthquake phase velocity [km/s]": -O >> $ps


#Post-processing stuff 
gmt ps2raster $ps -P -Tf

open fig3_2018.pdf