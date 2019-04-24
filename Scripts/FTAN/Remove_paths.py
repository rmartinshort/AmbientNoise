#!/usr/bin/env python
#RMS 2017
#Removes cross correlation files between station pairs where one of pair lies outside a user defined box - aim here is to reduce the size of the XC 
#dataset and prevent streaking. Run on stacked correlations in the SYM directory

import sys
import obspy
import os, shutil
from obspy.fdsn import Client
import argparse
import glob
client = Client('IRIS')


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('-indir',action='store',dest='indir',help='Full path to directory containing the X corr directory structure')
    results = parser.parse_args()

    try:
        os.chdir(results.indir)
    except:
        print 'Cannot enter given directory %s' %results.indir

    #Alaska region
    minlon = -165
    maxlon = -128
    minlat = 55.5
    maxlat = 70.0

    start = '2014-01-01'
    end = '2018-05-01'

    #Get the details of all the stations in this region
    inventory = client.get_stations(network=None,station=None,level='station',minlongitude=minlon,maxlongitude=maxlon,\
	    minlatitude=minlat,maxlatitude=maxlat,starttime=obspy.UTCDateTime(start),endtime=obspy.UTCDateTime(end))

    #Loop though inventory and generate list of approved stations

    ids = []
    for net in inventory:

	   for sta in net.stations:

	       id = net.code+'_'+sta.code
	       ids.append(id)

    #Go to SYM directory and make a list of the correlation files there 

    if not os.path.isdir('SYM'):

        print 'Directory SYM does not exist!'
        sys.exit(1)

    cwd = os.getcwd()

    os.chdir('SYM')

    #Form all the possible XC names from the stations within the region

    os.system('mkdir -p region_files')

    for station1 in ids:
        for station2 in ids:

            XCname = 'COR_'+station1+'_LHZ.'+station2+'_LHZ.SAC_s'
            dest = 'region_files/'+XCname
            print dest
            try:
                shutil.move(XCname,dest)
            except:
                continue


    # #Now, check if both the stations in the XC are within the region of interest. If so, move the file to a new location
    # for XCfile in Xcfiles:
    #     fp = XCfile.split('.')
    #     fp1 = fp[0].split('_')
    #     fp2 = fp[1].split('_')
    #     st1 = '_'.join(fp1[1:3])
    #     st2 = '_'.join(fp2[:2])
        
    #     if (st1 in ids) and (st2 in ids):

    #         print 'XC between %s and %s within region!' %(st1,st2)

    #         os.system('mv %s* region_files' %XCfile)






	

if __name__ == "__main__":

    main()
