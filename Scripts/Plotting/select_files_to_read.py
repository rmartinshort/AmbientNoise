#!/usr/bin/env python 

#Sorts SACfiles by distance and creates a sac macro that will read a given number of files in distance order
#Can then use sac or sss recordsection to plot these

import pandas as pd
import os
import numpy as np
import sys

def main():

	cwd = os.getcwd()

	indir = sys.argv[1] #this should be the directory containing the SAC files and the list of files by distance

	if not os.path.isdir(indir):
		print 'Directory %s not found!' %indir
		sys.exit(1)

	df = pd.read_csv('%s/tlist.tmp' %indir,delimiter=' ',header=None,names=['file','distance'])
	df = df.sort(['distance'])

	#We want to extract a representative selection of files, covering the entire distance range.
	nfiles = 50
	x = np.linspace(int(min(df.distance)+1),int(max(df.distance)-1),nfiles)
	distvec = df.distance
	statvec = df.file

	#create a macro file for reading in the stations 
	outfile = open('rstations.m','w')
	outfile.write('read ')

	#find the files associated with each distance in the vector x
	i = 0 
	for val in x:
		pos = (np.abs(distvec-val)).argmin()
		stationread = statvec[pos]

		if i > 0:
			if (i%10==0):
				outfile.write('\nread more %s ' %stationread)
			else:
				outfile.write('%s ' %stationread)

		i += 1

	outfile.close()

    #put the read stations macro file into the old directory 
	os.system('mv rstations.m %s' %indir)


if __name__ == '__main__':

	main()



