#!/bin/bash
#RMS 2018
#Figure 5 for ambient noise paper
#Series of depth slices though the final model

GMT=gmt

$GMT gmtset BASEMAP_TYPE_FANCY
$GMT gmtset FONT_ANNOT_PRIMARY 20p,Helvetica


#Topography
Topo=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/Alaska_gebco.nc
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
tremor=Wech_tremor.dat
Witt_thesis=Witt_splits.dat

ps=fig5.ps
J=B-151.5/63/54/72/12i #map projection is Albers Conic Equal Area (see GMT docs)
R=-168/-134.5/57/69 #Whole of Alaska and Adjacent Canada 

#Arrange the shear wave splitting measurements from Witt (2017)
awk '{print $1,$2,$3,$4/1.5}' $Witt_thesis > dat.splits.witt
#Make gradient file for lighting effects
gmt grdgradient $Topo -Nt1 -A0/90 -Ggrad_i.nc

#=================================================
#plot 1: 10km
#=================================================

ps=fig5.ps
GRD=dslice_10.grd
dep=10

#Cut the grd file to a region of interest
$GMT grdsample $GRD -I0.1 -R198/224/58/68.5 -Gtmp.grd

#make the color scheme
med=$(gmt grdinfo -L1 $GRD | sed -n '11p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.2" | bc)
maxvel=$(echo "$med+0.2" | bc)
echo $minvel
echo $maxvel

echo $cptscript
$cptscript $minvel $maxvel
cpt=vels.cpt

#plot the coastlines
$GMT pscoast -R$R -J$J -Bwsne -Glightgray -Di -Slightblue -A500/1/1 -K -W0.0 -P --PS_MEDIA=Custom_28.0ix40.0i -Y28i > $ps 

#plot the image
$GMT grdimage tmp.grd -R$R -J$J -C$cpt -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -A500/1/1 -R$R -J$J -BWSne -B3.0f3.0 -Di -P -K -O -W0.0 >> $ps

#Plot transparent topography on top
gmt grdimage $Topo -Igrad_i.nc -C255,255 -t70 -R$R -J$J -K -O >> $ps

#plot topography
#$GMT grdcontour $Topo -J$J -R$R -Wthin -C1000 -S1000 -L100/5000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps


minquakedepth=$(($dep - 10))
maxquakedepth=$(($dep + 10))

#Download events of interest
echo "==========================="
echo "Getting earthquakes"
echo "==========================="
#$GroupTomo/WRAPPER_SCRIPTS/Get_Earthquakes.py $minquakedepth $maxquakedepth 5.0
#$GMT psxy All_quakes_Alaska.dat -J$J -Rd$R -Sc0.15 -Wthinnest -Gwhite -O -K >> $ps

#Plot the extent of the subducted yakutat slab
#$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Plot the volcanoes
$GMT psxy $volcanoes -J$J -Rd$R -St0.6 -Gred -O -K -Wthin,black >> $ps

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
$GMT psscale -D2i/-1i/4.0i/0.2ih -E -C$cpt -t30 -Ba0.1f0.1g0.1/:"Vs [km/s]": -O -K >> $ps

#=================================================
#plot 2: 35km
#=================================================

GRD=dslice_35.grd
dep=35

#Cut the grd file to a region of interest
$GMT grdsample $GRD -I0.1 -R198/224/58/68.5 -Gtmp.grd

#make the color scheme
med=$(gmt grdinfo -L1 $GRD | sed -n '11p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.3" | bc)
maxvel=$(echo "$med+0.3" | bc)
echo $minvel
echo $maxvel

echo $cptscript
$cptscript $minvel $maxvel
cpt=vels.cpt

#plot the coastlines
$GMT pscoast -Rd$R -J$J -A5000 -Di -A500/1/1 -Bwsne -Glightgray -Slightblue -K -W0.0 -P -X14i -O >> $ps 

#plot the image
$GMT grdimage tmp.grd -R$R -J$J -C$cpt -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -A500/1/1 -BWSne -B3.0f3.0 -Di -A5000 -P -K -O -W0.0 >> $ps

#Plot transparent topography on top
gmt grdimage $Topo -Igrad_i.nc -C255,255 -t70 -R$R -J$J -K -O >> $ps

#plot topography
#$GMT grdcontour $Topo -J$J -R$R -Wthin -C1000 -S1000 -O -K -L100/5000 >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps


minquakedepth=$(($dep - 10))
maxquakedepth=$(($dep + 10))

#Download events of interest
echo "==========================="
echo "Getting earthquakes"
echo "==========================="
#$GroupTomo/WRAPPER_SCRIPTS/Get_Earthquakes.py $minquakedepth $maxquakedepth 4.0
#$GMT psxy All_quakes_Alaska.dat -J$J -Rd$R -Sc0.1 -Wthinnest -Gblack -O -K >> $ps

#Plot the extent of the subducted yakutat slab
#$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Plot the volcanoes
$GMT psxy $volcanoes -J$J -Rd$R -St0.6 -Gred -O -K -Wthin,black >> $ps

#Plot outline of the Welch tremor region (relevant for 50-80km depths)
#$GMT psxy $tremor -J$J -Rd$R -Sc0.01 -Gred -Wthinnest,red -O -K >> $ps

#Make the scale
$GMT psscale -D2i/-1i/4.0i/0.2ih -E -C$cpt -t30 -Ba0.1f0.1g0.1/:"Vs [km/s]": -O -K >> $ps

#=================================================
#plot 3: 65km
#=================================================

GRD=dslice_65.grd
dep=65

#Cut the grd file to a region of interest
$GMT grdsample $GRD -I0.1 -R198/224/58/68.5 -Gtmp.grd

#make the color scheme
med=$(gmt grdinfo -L1 $GRD | sed -n '11p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.2" | bc)
maxvel=$(echo "$med+0.1" | bc)
echo $minvel
echo $maxvel

echo $cptscript
$cptscript $minvel $maxvel
cpt=vels.cpt

#plot the coastlines
$GMT pscoast -Rd$R -J$J -Di -A500/1/1 -Bwsne -Glightgray -Slightblue -K -W0.0 -P -X-14i -Y-12i -O >> $ps 

#plot the image
$GMT grdimage tmp.grd -R$R -J$J -C$cpt -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -B3.0f3.0 -Di -A500/1/1 -P -K -O -W0.0 >> $ps

#plot topography
#$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -S1000 -O -K >> $ps

#Plot on the faults
#$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps


minquakedepth=$(($dep - 10))
maxquakedepth=$(($dep + 10))

#Download events of interest
echo "==========================="
echo "Getting earthquakes"
echo "==========================="
$GroupTomo/WRAPPER_SCRIPTS/Get_Earthquakes.py $minquakedepth $maxquakedepth 5.0
$GMT psxy All_quakes_Alaska.dat -J$J -Rd$R -Sc0.1 -Wthinnest -Gblack -O -K >> $ps

#Plot the extent of the subducted yakutat slab
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Plot the volcanoes
$GMT psxy $volcanoes -J$J -Rd$R -St0.6 -Gred -O -K -Wthinnest,black >> $ps

#Plot outline of the Welch tremor region (relevant for 50-80km depths)
$GMT psxy $tremor -J$J -Rd$R -Wthickest,red -O -K >> $ps

#Make the scale
$GMT psscale -D2i/-1i/4.0i/0.2ih -E -C$cpt -Ba0.1f0.1g0.1/:"Vs [km/s]": -O -K >> $ps

#=================================================
#plot 4: 85km
#=================================================

GRD=dslice_85.grd
dep=85

#Cut the grd file to a region of interest
$GMT grdsample $GRD -I0.1 -R198/224/58/68.5 -Gtmp.grd

#make the color scheme
med=$(gmt grdinfo -L1 $GRD | sed -n '11p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.2" | bc)
maxvel=$(echo "$med+0.2" | bc)
echo $minvel
echo $maxvel

echo $cptscript
$cptscript $minvel $maxvel
cpt=vels.cpt

#plot the coastlines
$GMT pscoast -Rd$R -J$J -Bwsne -Di -A500/1/1 -Glightgray -Slightblue -K -W0.0 -P -X14i -O >> $ps 

#plot the image
$GMT grdimage tmp.grd -R$R -J$J -C$cpt -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -B3.0f3.0 -Di -A500/1/1 -P -K -O -W0.0 >> $ps

#plot topography
#$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -S1000 -O -K >> $ps

#Plot on the faults
#$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps

#Plot the Witt splits as blue ticks
#$GMT psxy dat.splits.witt -SV0.3+jc -Gred -Wthickest,blue -J$J -Rd$R -O -V -K >> $ps


minquakedepth=$(($dep - 10))
maxquakedepth=$(($dep + 10))

#Download events of interest
echo "==========================="
echo "Getting earthquakes"
echo "==========================="
$GroupTomo/WRAPPER_SCRIPTS/Get_Earthquakes.py $minquakedepth $maxquakedepth 5.0
$GMT psxy All_quakes_Alaska.dat -J$J -Rd$R -Sc0.1 -Wthinnest -Gblack -O -K >> $ps

#Plot the extent of the subducted yakutat slab
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Plot the volcanoes
$GMT psxy $volcanoes -J$J -Rd$R -St0.6 -Gred -O -K -Wthinnest,black >> $ps

#Plot outline of the Welch tremor region (relevant for 50-80km depths)
$GMT psxy $tremor -J$J -Rd$R -Wthickest,red -O -K >> $ps

#Make the scale
$GMT psscale -D2i/-1i/4.0i/0.2ih -E -C$cpt -Ba0.1f0.1g0.1/:"Vs [km/s]": -O -K >> $ps

#=================================================
#plot 5: 120km
#=================================================

GRD=dslice_120.grd
dep=120

#Cut the grd file to a region of interest
$GMT grdsample $GRD -I0.1 -R198/224/58/68.5 -Gtmp.grd

#make the color scheme
med=$(gmt grdinfo -L1 $GRD | sed -n '11p' | cut -d ' ' -f 3)
echo $med
minvel=$(echo "$med-0.2" | bc)
maxvel=$(echo "$med+0.2" | bc)
echo $minvel
echo $maxvel

echo $cptscript
$cptscript $minvel $maxvel
cpt=vels.cpt

#plot the coastlines
$GMT pscoast -Rd$R -J$J -Bwsne -Glightgray -A500/1/1 -Di -Slightblue -K -W0.0 -P -Y-12i -X-14i -O >> $ps 

#plot the image
$GMT grdimage tmp.grd -R$R -J$J -C$cpt -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -BWSne -A500/1/1 -B3.0f3.0 -Di -P -K -O -W0.0 >> $ps

#plot topography
#$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -S1000 -O -K >> $ps

#Plot on the faults
#$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps


minquakedepth=$(($dep - 10))
maxquakedepth=$(($dep + 10))

#Download events of interest
echo "==========================="
echo "Getting earthquakes"
echo "==========================="
$GroupTomo/WRAPPER_SCRIPTS/Get_Earthquakes.py $minquakedepth $maxquakedepth 5.0
$GMT psxy All_quakes_Alaska.dat -J$J -Rd$R -Sc0.1 -Wthinnest -Gblack -O -K >> $ps

#Plot the extent of the subducted yakutat slab
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Plot the volcanoes
$GMT psxy $volcanoes -J$J -Rd$R -St0.6 -Gred -O -K -Wthinnest,black >> $ps

#Plot outline of the Welch tremor region (relevant for 50-80km depths)
$GMT psxy $tremor -J$J -Rd$R -Wthickest,red -O -K >> $ps

#Make the scale
$GMT psscale -D2i/-1i/4.0i/0.2ih -E -C$cpt -Ba0.1f0.1g0.1/:"Vs [km/s]": -O -K >> $ps

#=================================================
#plot 5: 120km
#=================================================

GRD=VEL.SLICE.120.try.sim.kernel.S.b.100.30.iasp.grd
dep=120

#Cut the grd file to a region of interest
$GMT grdsample $GRD -I0.1 -R198/224/58/68.5 -Gtmp.grd

#make the color scheme
$cptscript -3.5 3.5
cpt=vels.cpt

#plot the coastlines
$GMT pscoast -Rd$R -J$J -Di -A500/1/1 -Bwsne -Glightgray -Slightblue -K -W0.0 -P -X14i -O >> $ps 

#plot the image
$GMT grdimage tmp.grd -R$R -J$J -C$cpt -K -O -Q >> $ps

#Run pscoast to generate the coastlines
gmt pscoast -Rd$R -J$J -A500/1/1 -BWSne -B3.0f3.0 -Di -P -K -O -W0.0 >> $ps

#plot topography
#$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -S1000 -O -K >> $ps

#Plot on the faults
#$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps


minquakedepth=$(($dep - 10))
maxquakedepth=$(($dep + 10))

#Download events of interest
echo "==========================="
echo "Getting earthquakes"
echo "==========================="
$GroupTomo/WRAPPER_SCRIPTS/Get_Earthquakes.py $minquakedepth $maxquakedepth 5.0
$GMT psxy All_quakes_Alaska.dat -J$J -Rd$R -Sc0.1 -Wthinnest -Gblack -O -K >> $ps

#Plot the extent of the subducted yakutat slab
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Plot the volcanoes
$GMT psxy $volcanoes -J$J -Rd$R -St0.6 -Gred -O -K -Wthinnest,black >> $ps

#Plot outline of the Welch tremor region (relevant for 50-80km depths)
$GMT psxy $tremor -J$J -Rd$R -Wthickest,red -O -K >> $ps

#Make the scale
$GMT psscale -D2i/-1i/4.0i/0.2ih -E -C$cpt -Ba1f1g1/:"dVs/Vs [%]": -O >> $ps

#Post-processing stuff
$GMT ps2raster $ps -P -Tf
open fig5.pdf
