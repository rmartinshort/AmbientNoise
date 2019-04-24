#/bin/bash 

##RMS 2017
#GMT script to plot phase velocity map for Alaska
GMT=gmt4

gmt gmtset BASEMAP_TYPE_FANCY
gmt set FONT_ANNOT_PRIMARY 20p,Helvetica


J=B-147/56.0/52/73/12i #map projection is Albers Conic Equal Area (see GMT docs)
R=-160/-128/56/71 #Whole of Alaska and Adjacent Canada 

faults=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/Alaska_faults.dat
yakslab=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/tectonics_quakes_map/Yakutat_subducted_extent.dat
Topo=/Users/rmartinshort/Documents/Berkeley/Alaska/maps/station_map/Alaska_gebco.nc

#Make a multi-panel image of Alaska phase velocities
ps=Example_phasemaps.ps


##################
# period = 10 s
##################

GRD=masked_pert_data10s_ph.txt.grd
minvel=-10
maxvel=10

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $1}')

if [ $tmpnname == "rays" ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $2}')
	phasename=$phasename.RES
else
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | cut -c 5-)
fi

#make the color scheme
#minval=3.0
#maxval=4.5
#$GMT makecpt -Cjet -T$minval/$maxval/0.01 -Z -I > absvels.cpt

#Generate the appropriate color scheme
./Generate_colscheme.py $minvel $maxvel
cpt=vels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -Y27.5i -P -K --PAPER_MEDIA=Custom_43ix40i > $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#plot topography 
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps

#Plot the extent of the subducted yakutat slab 
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba2f2g2/:"Phase velocity anomaly [%]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O -K >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END

##################
# period = 13 s
##################

GRD=masked_pert_data13s_ph.txt.grd
minvel=-10
maxvel=10

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $1}')

if [ $tmpnname == "rays" ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $2}')
	phasename=$phasename.RES
else
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | cut -c 5-)
fi

#make the color scheme
#minval=3.0
#maxval=4.5
#$GMT makecpt -Cjet -T$minval/$maxval/0.01 -Z -I > absvels.cpt

#Generate the appropriate color scheme
./Generate_colscheme.py $minvel $maxvel
cpt=vels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -P -O -K -Y-7i -X11i >> $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#plot topography 
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps

#Plot the extent of the subducted yakutat slab 
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba2f2g2/:"Phase velocity anomaly [%]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O -K >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END


##################
# period = 16 s
##################

GRD=masked_pert_data16s_ph.txt.grd
minvel=-10
maxvel=10

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $1}')

if [ $tmpnname == "rays" ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $2}')
	phasename=$phasename.RES
else
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | cut -c 5-)
fi

#make the color scheme
#minval=3.0
#maxval=4.5
#$GMT makecpt -Cjet -T$minval/$maxval/0.01 -Z -I > absvels.cpt

#Generate the appropriate color scheme
./Generate_colscheme.py $minvel $maxvel
cpt=vels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -P -K -O -Y-7i -X11i >> $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#plot topography 
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps

#Plot the extent of the subducted yakutat slab 
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba2f2g2/:"Phase velocity anomaly [%]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O -K >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END


##################
# period = 19 s
##################

GRD=masked_pert_data19s_ph.txt.grd
minvel=-7
maxvel=7

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $1}')

if [ $tmpnname == "rays" ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $2}')
	phasename=$phasename.RES
else
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | cut -c 5-)
fi

#make the color scheme
#minval=3.0
#maxval=4.5
#$GMT makecpt -Cjet -T$minval/$maxval/0.01 -Z -I > absvels.cpt

#Generate the appropriate color scheme
./Generate_colscheme.py $minvel $maxvel
cpt=vels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -P -K -O -Y-19.5i -X-31i >> $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#plot topography 
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps

#Plot the extent of the subducted yakutat slab 
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -K >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba2f2g2/:"Phase velocity anomaly [%]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O -K >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END


##################
# period = 22 s
##################

GRD=masked_pert_data22s_ph.txt.grd
minvel=-7
maxvel=7

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $1}')

if [ $tmpnname == "rays" ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $2}')
	phasename=$phasename.RES
else
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | cut -c 5-)
fi

#make the color scheme
#minval=3.0
#maxval=4.5
#$GMT makecpt -Cjet -T$minval/$maxval/0.01 -Z -I > absvels.cpt

#Generate the appropriate color scheme
./Generate_colscheme.py $minvel $maxvel
cpt=vels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -P -K -O -Y-7i -X11i >> $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#plot topography 
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -K  >> $ps

#Plot the extent of the subducted yakutat slab 
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba2f2g2/:"Phase velocity anomaly [%]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O -K >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END

##################
# period = 25 s
##################

GRD=masked_pert_data25s_ph.txt.grd
minvel=-7
maxvel=7

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $1}')

if [ $tmpnname == "rays" ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $2}')
	phasename=$phasename.RES
else
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | cut -c 5-)
fi

#make the color scheme
#minval=3.0
#maxval=4.5
#$GMT makecpt -Cjet -T$minval/$maxval/0.01 -Z -I > absvels.cpt

#Generate the appropriate color scheme
./Generate_colscheme.py $minvel $maxvel
cpt=vels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -P -K -O -Y-7i -X11i >> $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#plot topography 
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -V -K  >> $ps

#Plot the extent of the subducted yakutat slab 
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba2f2g2/:"Phase velocity anomaly [%]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O -K >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END

##################
# period = 28 s
##################

GRD=masked_pert_data28s_ph.txt.grd
minvel=-5
maxvel=5

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $1}')

if [ $tmpnname == "rays" ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $2}')
	phasename=$phasename.RES
else
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | cut -c 5-)
fi

#make the color scheme
#minval=3.0
#maxval=4.5
#$GMT makecpt -Cjet -T$minval/$maxval/0.01 -Z -I > absvels.cpt

#Generate the appropriate color scheme
./Generate_colscheme.py $minvel $maxvel
cpt=vels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -P -K -O -Y-19.5i -X-31i >> $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#plot topography 
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -V -K  >> $ps

#Plot the extent of the subducted yakutat slab 
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba2f2g2/:"Phase velocity anomaly [%]": -O -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O -K >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END

##################
# period = 31 s
##################

GRD=masked_pert_data31s_ph.txt.grd
minvel=-5
maxvel=5

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $1}')

if [ $tmpnname == "rays" ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $2}')
	phasename=$phasename.RES
else
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | cut -c 5-)
fi

#make the color scheme
#minval=3.0
#maxval=4.5
#$GMT makecpt -Cjet -T$minval/$maxval/0.01 -Z -I > absvels.cpt

#Generate the appropriate color scheme
./Generate_colscheme.py $minvel $maxvel
cpt=vels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -P -K -O -Y-7i -X11i >> $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba2f2g2/:"Phase velocity anomaly [%]": -O -K >> $ps

#plot topography 
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -V -K  >> $ps

#Plot the extent of the subducted yakutat slab 
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O -K >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END

##################
# period = 33 s
##################

GRD=masked_pert_data32s_ph.txt.grd
minvel=-5
maxvel=5

#Give the model a name and plot the details
MODNAME="Alaska phase vels"

tmpnname=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $1}')

if [ $tmpnname == "rays" ]; then 
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | awk -F "." '{print $2}')
	phasename=$phasename.RES
else
	phasename=$(echo $GRD | awk -F "_" '{print $3}' | cut -c 5-)
fi

#make the color scheme
#minval=3.0
#maxval=4.5
#$GMT makecpt -Cjet -T$minval/$maxval/0.01 -Z -I > absvels.cpt

#Generate the appropriate color scheme
./Generate_colscheme.py $minvel $maxvel
cpt=vels.cpt

#plot the image
$GMT grdimage $GRD -R$R -J$J -Ba8.0f2.0/a4.0f2.0 -C$cpt -P -K -O -Y-7i -X11i >> $ps

#plot the coastlines
$GMT pscoast -R$R -J$J -Ia -Wthinnest -A2 -Dh -P -O -K >> $ps

#Make the scale 
$GMT psscale -D2.3i/-0.8i/4.0i/0.2ih -E -C$cpt -Ba2f2g2/:"Phase velocity anomaly [%]": -O -K >> $ps

#plot topography 
$GMT grdcontour $Topo -J$J -R$R -Wthinnest -C1000 -O -K >> $ps

#Plot on the faults
$GMT psxy $faults -m -J$J -Rd$R -Wthick,purple -O -V -K  >> $ps

#Plot the extent of the subducted yakutat slab 
$GMT psxy $yakslab -J$J -Rd$R -Wthickest -O -V -K >> $ps

$GMT pstext << END -X3i -Y7i -R0/10.5/0/11 -Jx1 -O >> $ps
0.75 7.5 22 0 1 LM ${MODNAME} ${phasename}
END

#------------------------------------

#Post-processing stuff 
$GMT ps2raster $ps -P -Tf
rm *.ps
open Example_phasemaps.pdf
