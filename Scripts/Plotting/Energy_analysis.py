#!/usr/bin/env python
#RMS March 2017
#Plot the energy amplitude diagrams prodiced by FTAN and vizualiase the chosen dispersion curve
#These give a sense of how 'high quality' the data is, and how it compares with the example dispersion curve

import matplotlib.pylab as plt
import matplotlib
import pandas as pd
import glob
import os
import numpy as np
from netCDF4 import Dataset
from scipy import interpolate
import argparse

def main():

	parser = argparse.ArgumentParser()
	parser.add_argument('-indir',action='store',dest='indir',help='Full path to a directory containing the XC directory structure')
	results = parser.parse_args()

	if os.path.exists(results.indir):
		indir = results.indir
	else:
		print 'Provided dir does not exist'
		sys.exit(1)

	cwd = os.getcwd()
	os.chdir(indir)

	#Load the distance file
	try:
		df_distance = pd.read_csv('tlist.tmp',sep=' ',names=['Filename','Distance'])
	except:

		print 'Something wrong with loading the XC distance file: will try to recreate (this could take a long time)'
		os.system('./readfiles_inorder.sh')


	XCfiles = glob.glob('*.SAC_s')

	for infile in XCfiles:

		print 'Working on file %s' %infile

		figfilename = infile+'_Energy_plot.png'

		ampmap = infilename+'_2_AMP'
		results = infilename+'_2_DISP.1'
		ampdf = pd.read_csv(ampmap,sep="\s+|\t+|\s+\t+|\t+\s+",names=['Period_number','Time','Energy'])
		resdf = pd.read_csv(results,sep="\s+|\t+|\s+\t+|\t+\s+",\
                  names=['Period_number','CentralPeriod','ObservedPeriod','GroupVel','PhaseVel','Amp','SNR'])

		#Generate a netCDF file from the amplitude map
		maxp = max(ampdf.Period_number)
		minp = min(ampdf.Period_number)
		maxt = max(ampdf.Time)
		mint = min(ampdf.Time)

		os.system('gmt xyz2grd %s -R%s/%s/%s/%s -Gtmp.grd -I1' %(ampmap,minp,maxp,mint,maxt))

		#Load dataset and extract gridded variables
		fh = Dataset('tmp.grd', mode='r')
		periods = fh.variables['x'][:]
		times = fh.variables['y'][:]
		energy = fh.variables['z'][:]

		#Make 2D interpolation function
		f = interpolate.interp2d(periods, times, energy, kind='linear')

		#Make a vector x between the min and max period values
		x = np.linspace(min(periods),max(periods),100)

		#Interplate the period values against the real period values
		fp = interpolate.interp1d(periods,resdf.ObservedPeriod,kind='linear')

		#Map from the period numbers to the real periods
		realperiods = fp(x)

		#Get the station-station distance
		fdist = df_distance[df_distance['Filename']==infilename]
		dist = fdist.Distance.values[0]

		#Generate a vector of times (x axis)
		y = np.linspace(min(times),max(times),100)

		#Use the 2D interpolation function to create a map in x-y space
		zz = f(x,y)

		#Convert the y vector to velocities
		vels = dist/y

		#plot the result
		plt.figure()
		plt.pcolor(realperiods,dist/y,zz,cmap='jet')
		plt.hold('on')
		plt.plot(resdf.ObservedPeriod,resdf.GroupVel,'k--',label='Central_period_Gvel')
		plt.xlim([min(resdf.ObservedPeriod),max(resdf.ObservedPeriod)])
		plt.ylim([min(vels),max(vels)])
		plt.xlabel('Period (s)')
		plt.ylabel('Velocity [km/s]')
		plt.colorbar()
		plt.savefig(figfilename)


if __name__ == '__main__':

	main()


