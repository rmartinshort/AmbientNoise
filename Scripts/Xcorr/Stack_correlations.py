#!/usr/bin/env python 
#RMS 2018
#Stack the correlation .SAC files produced by RP's C code
#This produces files in the format required by the ambient noise tomography workflow 

import pandas as pd
import obspy as op
import glob 
import os
import sys


def str_merge(var):

	nname = '%s_%s_%s' %(var[0],var[1],var[2])
	return nname

def main():

	#Year and day directories to loop over
	years = glob.glob('20*')
	days = range(1,32)

    #Name of the directory to which stacked cross correlations are output
	ofdir = 'STACKS2'

	#The file station.list must be present
	if not os.path.exists('station.list'):
		print ('station.list must be in this directory')
		sys.exit(1)

	#Generate list of station components to stack
	comp_names = pd.read_csv('station.list',names=['net','station','comp'],sep=' ')
	comp_names['fpart'] = comp_names[['net','station','comp']].apply(str_merge,axis=1)
	fnames = comp_names['fpart'].values

	#Stacking loop - constructs file names associated with all paths, then finds these files and
	#stacks them across the entire time domain

	for part1 in fnames:

		first_part = 'COR_'+part1

		for part2 in fnames:

			if part1 != part2:

				second_part = '.'+part2+'.SAC'

				#We want to generate one datastream per cross correlation file, then stack them

				datastream = False
				tracecount = 0

				target_name = first_part+second_part
				ofilename = 'STACKS2/%s' %target_name

				#Search though the time directories. Select common files and stack them

				for year in years:
					for day in days:
						tdir = '%s/%02d/%s' %(year,day,target_name)
						if os.path.exists(tdir):

							try:
								st = op.read(tdir,format='SAC')

								if datastream == False:
									datastream = st[0]
								else:
									datastream.data += st[0].data

								tracecount += 1

							except:
								continue

				#The data has been stacked, so divide by the number of contributing traces

				if datastream != False:
					datastream.data = datastream.data/tracecount
					datastream.write(ofilename,format='SAC')


		print "Done with %s" %part1


if __name__ == '__main__':

	main()