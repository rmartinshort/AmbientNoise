#!/usr/bin/env python

#RMS March 2017
#Run Landes's codes to estimate noise directivity in month-long stacked cross correlations
#Also plot the results


import obspy
import glob
import argparse
import sys
import os

sacfilterscript='/data/dna/rmartin/Ambient_noise/CODES/Scripts/filter_sac.sh'
beamformscript='/data/dna/rmartin/Ambient_noise/CODES/Scripts/BeamFormNoise.sh'


def main():

	parser = argparse.ArgumentParser()
	parser.add_argument('-indir',action='store',dest='indir',help='Full path to directory containing the X corr directory structure')
	results = parser.parse_args()

	rerun=True

	if os.path.exists(results.indir):

	    indir = results.indir

	else:

	    raise NameError('Provided dir does not exist')
	    sys.exit(1)

	cwd = os.getcwd
	os.chdir(indir)

	yeardirs = glob.glob('20*')

	for year in yeardirs:

		os.chdir(year)

		months = glob.glob('*')

		yeardir = os.getcwd()

		for month in list(sorted(months)):

			try:
				os.chdir(month)

				sacfiles = glob.glob('*SAC')

				#Make a list of the unique station coordinates from the cross correlation files

				#os.system('rm station.lst')

				if os.path.isfile('station.lst') and (rerun == False):

					print 'Stations coordinate list for year %s, month %s already exists - proceeding' %(year,month)

				else:

					print 'Generating station coordinate list for year %s, month %s' %(year,month)

					Generate_Station_Coords_List(sacfiles)

				if os.path.isfile('noise_0.1_0.3.txt') and (rerun == False):

					print 'Not yet coded!!'

				else:

					#Filter the files in the two passbands corresponding to the two micoseismic peaks
					filters = [[0.05,0.1,15],[0.1,0.3,10]]

					if not os.path.isfile('filter_sac.sh'): 

						os.system('cp %s .' %sacfilterscript)


					if os.path.isfile('BeamFormNoise.sh'):
						os.system('rm BeamFormNoise.sh')


					os.system('cp %s .' %beamformscript)

					for band in filters:

						low = band[0]
						high = band[1]
						time = band[2]

						print 'Using filter band: %s %s' %(low,high)

						os.system('./filter_sac.sh %s %s' %(low,high))

						#Generate a list of all the XC files
						os.system('ls *.tmp > sacfiles')

						#Run the noise beamforming script

						os.system('./BeamFormNoise.sh %s' %time)

						#move the result to a unique filename

						noise_name = 'noise_%s_%s.txt' %(low,high)

						os.system('mv noise.txt %s' %noise_name)

						os.system('rm *.tmp')

				os.chdir(yeardir)

			except:
				print 'Cannot access dir %s' %month

		os.chdir(indir)




def Generate_Station_Coords_List(sacfiles):

	stations = {}	
	stationslist = []

	for infile in sacfiles:

		#Get the station namas associated with that file
		fparts = infile.split('_')
		station1 = fparts[2]
		station2 = fparts[4]

		if station1 not in stationslist:
			tr = obspy.read(infile,format='SAC')
			s1lat = tr[0].stats.sac.stla
			s1lon = 360+tr[0].stats.sac.stlo
			stations[station1] = [s1lat,s1lon]
			stationslist.append(station1)

		if station2 not in stationslist:
			tr = obspy.read(infile,format='SAC')
			s2lat = tr[0].stats.sac.evla
			s2lon = 360+tr[0].stats.sac.evlo
			stations[station2] = [s2lat,s2lon]
			stationslist.append(station2)


	#unpack and write to file
	stationsfile = open('station.lst','w')
	for station in stations:

		lat = stations[station][0]
		lon = stations[station][1]

		stationsfile.write('%s %s %s\n' %(station,lat,lon))

	stationsfile.close()



if __name__ == '__main__':

	main()







