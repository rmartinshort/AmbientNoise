#!/bin/bash
#RMS 2017
#Adaptation of DOIT.deep from CPS tutorial
#Copies sobs.d and start.mod into each point directory and then runs an inversion
#using the the CPS surface wave code surf96

#At some point we may want to start from a 3D model, in which case we'll need to make a new
#start.mod at each surface point

#Requires the Computer Programs in Seismology code to be installed

cwd=`pwd`
datadir=/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert

if [ ! -d $datadir ]; then
	echo 'The given directory $datadir does not exist'
	exit 1
fi

cd $datadir

for dir in disp*; do

	echo $dir
	cp $cwd/sobs.d $datadir/$dir
	cp $cwd/start.mod $datadir/$dir

	cd $datadir/$dir

	#Run the inversion using surf96
	####
	#	clean up
	####
	surf96 39
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
	# We want to allow the region about some depth to change a lot, while the region below is more constrained. Since
	# we don't know where the Moho is, we need to be careful about which parts of our model are allowed to vary. Ideally, we'd want
	# to use a decent approximation to the true velocity structure at each point and have this is our input 1D model
	#	The  50 km boundary is index 25 wt = 0.9
	#	The 100 km boundary is index 35 wt = 0.1
	#####
	surf96 31 45 0.0

	#####
	#	constrain layers 35 - 44
	#####
		surf96 31 25 0.9
		surf96 31 26 0.8
		surf96 31 27 0.7
		surf96 31 28 0.6
		surf96 31 29 0.5
		surf96 31 30 0.4
		surf96 31 31 0.3
		surf96 31 32 0.2
		surf96 31 33 0.1
		surf96 31 34 0.1
		surf96 31 35 0.1

		for j in 36 37 38 39 40 41 42 43 44 45 46 47 48 49 \
		50 51 52 53 54 55 56 57 58 59 \
		60 61 62 63 64 65 66 67 68 69 \
		70 71 72 73 74 75 76 77 78 79 \
		80 81 82 83 84 85 86 87 88 89
		do
			surf96 31 $j 0.1
		done

		for j in 65 66 67 68 69 \
	        70 71 72 73 74 75 76 77 78 79 \
	        80 81 82 83 84 85 86 87 88 89
	        do
	                surf96 31 $j 0.0
	        done
	#####
	#	also smooth the upper crust a bit
	#####
		surf96 31  1 0.8
		surf96 31  2 0.8
		surf96 31  3 0.8
		surf96 31  4 0.8
		surf96 31  5 0.8
		surf96 31  6 0.8
		surf96 31  7 0.8
		surf96 31  8 0.8
		surf96 31  9 0.8
	#####
	#	start the first inversion with a slightly higher damping
	#	do avoid an overshoot in the first model estimate
	#####
	surf96 32 10
	surf96 37 2 1 2 6
	#####
	#	do 28 more inversions
	#####
	surf96 32 2  #was 0.5
	time surf96 37 28 1 2 6

	#####
	#	get the current model
	#####
	surf96 1 2 28 end.mod

	#####
	#	plot up the dispersion
	#####
	srfphv96
	#####
	#	plot up the resolution kernel
	#####
	srfphr96

	#surf96 45 > 45.out #show velocity weights
	#surf96 47 > 47.out #show Q weights

	#plot the model evolution towards final value
	shwmod96 -ZMAX 100 -K -1 -LEG start.mod tmpmod96.??? end.mod

	#convert to ps and plot results
	plotnps -K < SRFPHR96.PLT > reskernel.ps
	plotnps -K < SRFPHV96.PLT > dispersion.ps
	plotnps -K < SHWMOD96.PLT > med_evo.ps

done

cd $cwd
