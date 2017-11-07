#!/bin/bash
#RMS 2017
#Adaptation of DOIT.deep from CPS tutorial
#Copies jobs.d and start.mod into each point directory and then runs an inversion
#using the the CPS surface wave code joint96

#At some point we may want to start from a 3D model, in which case we'll need to make a new
#start.mod at each surface point

#Requires the Computer Programs in Seismology code to be installed

cwd=`pwd`

if [ -z "$1" ]; then

 echo "no command line argument supplied: using default (check)"
 datadir=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_ALL_phonly_1_100
else

 datadir=$1

fi

if [ ! -d $datadir ]; then
	echo 'The given directory $datadir does not exist'
	exit 1

else

	echo $datadir

fi
cd $datadir

stations_fname=Alaska_station_names.txt

if [ ! -f $stations_fname ]; then
   echo "Required file Alaska_station_names.txt not found"
   cd $cwd
   exit 1
fi

while read line ; do
    dir=$(echo $line)

	echo $dir

	cp $cwd/surf_params/jobs.d $dir
	cp $cwd/surf_params/ebh/adapt2/start_ALL.mod $dir/start.mod

	cd $dir

    touch rftn.lst

	#Generate the list of receiver functions file
	ls *.SAC > rftn.lst

	TAIL=`tail -1 rftn.lst`
	KSTNM=`saclhdr -KSTNM ${TAIL}`

	#####
	joint96 39

	#####
	#	set the time window for the RFTN inversion
	#	to -5, 20  The 20 is later than the bounce
	#	The begin time of -5 is OK since we
	#	only use the GAUSS ALPHA=1.0 and 2.5 . If
	#	the 0.5 were  used we may ahve to start earlier
	#	because of the longer width of the pulse ventered at zero lag
	#####
	joint96 33 -5
	joint96 34 20
	#####
	#	lower floor on rftn
	#####
	joint96 42 0.00020
	joint96 40 0.05

	#####
	#	Layer weighting
	#	This is a 45 layer model.
	#	We will attempt to have a smooth upper crust
	#	and attempt to fix the lower crust with a transition
	#	starting at 50 km
	#	Previous inversion indicate a 30 km crustal thickness here
	#
	#	To relate the layer indices to particular boundaries,
	#	I run the program manually and use option 45 at the beginning
	#####
	#	fix the bottom layer which is index 45
	#	The  50 km boundary is index 25 wt = 0.9
	#	The 100 km boundary is index 35 wt = 0.1
	#####

	#####
	#	constrain layers 36 - 60
	#####
	# #
                surf96 31 36 0.9
                surf96 31 37 0.9
                surf96 31 38 0.8
                surf96 31 39 0.8
                surf96 31 40 0.7
                surf96 31 41 0.7
                surf96 31 42 0.6
                surf96 31 43 0.6
                surf96 31 44 0.6
                surf96 31 45 0.6
                surf96 31 46 0.5
                surf96 31 47 0.5
                surf96 31 48 0.5
                surf96 31 49 0.5
                surf96 31 50 0.4
                surf96 31 51 0.4
                surf96 31 52 0.4
                surf96 31 53 0.4
                surf96 31 54 0.3
                surf96 31 55 0.3
                surf96 31 56 0.3 
                surf96 31 57 0.2 
                surf96 31 58 0.2 
                surf96 31 59 0.2
                surf96 31 60 0.1
		# #
		# # #Set weightings of all greater depths to zero
		for j in $(seq 61 82)

			do
			  joint96 31 $j 0.0
      done

                #
		#
		# #	also smooth the crust a bit
		joint96 31  1 0.5
		joint96 31  2 0.5
		joint96 31  3 0.5
		joint96 31  4 0.6
		joint96 31  5 0.6
		joint96 31  6 0.6
		joint96 31  7 0.7
		joint96 31  8 0.7
		joint96 31  9 0.7
        joint96 31 10 0.8
        joint96 31 11 0.8
        joint96 31 12 0.8
        joint96 31 13 0.9
        joint96 31 14 0.9
        joint96 31 15 0.9

        

		#####
		#	set the p factor. The larger the value of p, the more the surface waves influence the problem
		#####
		joint96 43 0.25
		#####
		#	start the first inversion with a slightly higher damping
		#	do avoid an overshoot in the first model estimate
		#####
		joint96 32 10
		joint96 37 2 1 2 6
		#####
		#	do 50 more inversions
		#####
		joint96 32 1.2 #was 0.5

		time joint96 37 30 1 2 6

		#####
		#	get the current model
		#####
		joint96 1 2 28 end.mod
		cp end.mod ${KSTNM}end.mod

		#####
		#	plot up the receiver functions
		#####
		rftnpv96
		srfphv96
		#####
		#	plot up the resolution kernel
		#####
		srfphr96


		#plot the model evolution towards final valu
		shwmod96 -ZMAX 100 -K -1 -LEG start.mod tmpmod96.??? end.mod

		#convert to ps and plot results
		plotnps -K < SRFPHR96.PLT > reskernel.ps
		plotnps -K < SRFPHV96.PLT > dispersion.ps
		plotnps -K < RFTNPV96.PLT > rf_dispersion.ps
		plotnps -K < SHWMOD96.PLT > med_evo.ps
    cd $datadir

done <$stations_fname

cd $cwd
