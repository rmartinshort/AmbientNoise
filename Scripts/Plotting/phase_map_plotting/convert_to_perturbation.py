#!/usr/bin/env python 
#RMS 2017

#Convert velocity grd files to percentage perturbation from the mean velocity at that period
import os
import glob
import pandas as pd 
from scipy import interpolate

def main():

	vels = pd.read_csv('Mean_phase_vel_prediction.dat',sep=' ',names=['period','velocity'])

	#generate interpolation function for velocities
	fvels = interpolate.interp1d(vels.period.values,vels.velocity.values)

	grdfiles = glob.glob('*data*.grd')

	PGindicator = 'ph'

	for grdfile in grdfiles:


		if PGindicator in grdfile:

			period = float(grdfile.split('_')[1][4:][:-1])
			print period
			print grdfile

			#Determine the mean phase velocity at that period
			meanvel = fvels(period)
			outfile = 'pert'+grdfile[4:]
			print outfile

			os.system('gmt grdmath %s %g DIV 1 SUB 100 MUL = %s' %(grdfile,meanvel,outfile))


if __name__ == '__main__':

	main() 



