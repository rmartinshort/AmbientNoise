#!/usr/bin/env python

#Enter a directory and extract the locations of all the stations represented by the SAC files there

import obspy
import glob
import argparse
import sys
import os


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('-indir',action='store',dest='indir',help='Full path to directory containing X correlation sac files')
    results = parser.parse_args()

    if os.path.exists(results.indir):

        indir = results.indir

    else:

        raise NameError('Provided dir does not exist')
        sys.exit(1)


    cwd = os.getcwd()
    os.chdir(indir)

    sacfiles = glob.glob('*SAC')
    stations = {}
    stationslist = []

    for infile in sacfiles:

    	#Get the station namas associated with that file
    	fparts = infile.split('_')
    	station1 = fparts[2]
    	station2 = fparts[4]

    	print station1,station2

    	tr = obspy.read(infile,format='SAC')
    	s1lat = tr[0].stats.sac.stla
    	s1lon = 360+tr[0].stats.sac.stlo

    	s2lat = tr[0].stats.sac.evla
    	s2lon = 360+tr[0].stats.sac.evlo

    	if station1 not in stationslist:
    		stations[station1] = [s1lat,s1lon]
    		stationslist.append(station1)

    	if station2 not in stationslist:
    		stations[station2] = [s2lat,s2lon]
    		stationslist.append(station2)

    
    #unpack and write to file
    stationsfile = open('Station_coords.dat','w')
    for station in stations:

    	lat = stations[station][0]
    	lon = stations[station][1]

    	stationsfile.write('%s %s %s\n' %(station,lat,lon))


    stationsfile.close()


if __name__ == '__main__':

	main()