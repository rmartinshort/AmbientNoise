#!/usr/bin/env python
#Extract data from a set of station pairs for Ambient noise. Create files for plotting the station coordinates and the station pair lines

import glob
import os
import pandas as pd
import sys

def main():

	indatafile = sys.argv[1]

	if not os.path.isfile(indatafile):
		print 'File %s not found' %indatafile
		sys.exit(1)

	db = pd.read_csv(indatafile,sep=' ',names=['lat1','lon1','lat2','lon2','velocity','tmp1','tmp2','sta1','sta2','distance','azimuth','ndays'])
	db.drop('tmp1',axis=1)
	db.drop('tmp2',axis=1)

	#Write the path file
	outfile1 = open('SS_pair_paths.dat','w') #lines connecting the station pairs
	outfile2 = open('S_coords.dat','w') #the station coordinates themselves

	used_stations = []
	for index, row in db.iterrows():
		la1 = row['lat1']
		la2 = row['lat2']
		lo1 = row['lon1']
		lo2 = row['lon2']

		sta1 = row['sta1']
		sta2 = row['sta2']

		if sta1 not in used_stations:
			outfile2.write('%g %g %s\n' %(lo1,la1,sta1))
			used_stations.append(sta1)
		if sta2 not in used_stations:
			outfile2.write('%g %g %s\n' %(lo2,la2,sta2))
			used_stations.append(sta2)

		outfile1.write('%g %g\n' %(lo1,la1))
		outfile1.write('%g %g\n' %(lo2,la2))
		outfile1.write('>\n')

	outfile1.close()
	outfile2.close()


if __name__ == '__main__':

	main()
