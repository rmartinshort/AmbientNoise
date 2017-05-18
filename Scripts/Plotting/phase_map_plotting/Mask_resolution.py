#!/usr/bin/env python 

#Use GMT to generate masks that cut out those parts of a phase velocity map that have resolutions below some level
#RMS March 2017

import glob
import sys
import pandas as pd


def main():

	resval = 100 #maximum resolution valued allowed

	#get all the resolution files
	resfiles = glob.glob('data*.rea')

	for infile in resfiles:

		print 'Dealing with file %s' %infile

		maskname = infile.split('.')[0]+'_mask.txt'
		df = pd.read_csv(infile,sep="\s+|\t+|\s+\t+|\t+\s+",names=['lon','lat','resolution_x','resolution_y','tmp1','tmp2','tmp3'])
		df['resolution'] = (df.resolution_x + df.resolution_y)/2.0 #make mean resolution column
		maskdata = df[df.resolution <= resval]
		outfile = open(maskname,'w')

		for index, row in maskdata.iterrows():
			outfile.write('%g %g\n' %(float(row['lon']),float(row['lat'])))

		outfile.close()

if __name__ == '__main__':

	main()
