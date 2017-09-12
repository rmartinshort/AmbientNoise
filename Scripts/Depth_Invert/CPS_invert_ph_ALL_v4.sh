 #!/bin/bash
#RMS 2017
#Adaptation of DOIT.deep from CPS tutorial
#Copies sobs.d and start.mod into each point directory and then runs an inversion
#using the the CPS surface wave code surf96

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

for dir in disp*; do

	echo $dir

	cp $cwd/surf_params/sobs.d $dir
	cp $cwd/surf_params/ebh/adapt2/start_ALL.mod $dir/start.mod

	cd $dir

	#Run the inversion using surf96
        #clean up
        ####
        surf96 39
        #####

        #show layers and their weights
        #surf96 45

        #If a layer is not mentioned, its default weight will be 1
        #Linear increase in consraint between layers 37 and 47 - reflects decreasing resolving power
        #of the teleseismic surface waves

        #All layers above this (aside from upper crust) will have the same weighting
        #It would be possible to impose a moho within some layer by forcing a velocity jump there
				surf96 31 48 0.9
				surf96 31 49 0.8
				surf96 31 50 0.7
				surf96 31 51 0.6
				surf96 31 52 0.5
				surf96 31 53 0.4
				surf96 31 54 0.3
				surf96 31 55 0.2
				surf96 31 56 0.1
				# #
				# #
				# # #Set weightings of all greater depths to zero
				for j in $(seq 56 82)

				 			 		 do
				  							surf96 31 $j 0.0
				 		 			 done
				#
				#
				# #	also smooth the upper crust a bit
				surf96 31  1 0.8
				surf96 31  2 0.8
				surf96 31  3 0.8
				surf96 31  4 0.8
				surf96 31  5 0.8
				surf96 31  6 0.8
				surf96 31  7 0.8
				surf96 31  8 0.8
				surf96 31  9 0.8

        #do first iteration with increased damping factor
        surf96 32 10
        #reset number of iterations, run the inversion, calculate dispersion, run inversion
        #update model
        surf96 37 2 1 2 6

        #####
        #do 28 more inversions
        #####
        surf96 32 5  #was 0.5,0.8,0.1
        time surf96 37 28 1 2 6

        #####
        #get the current model
        #####
        surf96 1 2 28 end.mod

        #####
        #plot up the dispersion
        #####
        srfphv96
        #####
        #plot up the resolution kernel
        #####
        srfphr96

        #plot the model evolution towards final value
		shwmod96 -ZMAX 100 -K -1 -LEG start.mod tmpmod96.??? end.mod

        #convert to ps and plot results
        plotnps -K < SRFPHR96.PLT > reskernel.ps
        plotnps -K < SRFPHV96.PLT > dispersion.ps
        plotnps -K < SHWMOD96.PLT > med_evo.ps
	cd $datadir

done

cd $cwd
