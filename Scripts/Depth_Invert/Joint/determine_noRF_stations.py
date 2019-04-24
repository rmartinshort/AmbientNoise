#!/usr/bin/env python

#RMS Oct 2017
#Make a list of the stations that have no RF information, and their coordinates

import pandas as pd
import glob
import sys
import os


def main():

    stationsdf = pd.read_csv('Alaska_stations.txt',sep=' ',names=['lon','lat','tmp1','stname','tmp2'])
    outfile = open("Alaska_stations_noRF.txt",'w')

    for index, row in stationsdf.iterrows():

        station_name = row.stname
        station_lon = row.lon
        station_lat = row.lat

        if os.path.exists(station_name):
            RFs = glob.glob('%s/*.SAC' %station_name)
        else:
            print "Station %s not present in this dir" %station_name

        if len(RFs) < 1:
            outfile.write('%s %s %s\n' %(station_lon,station_lat,station_name))


    outfile.close()



if __name__ == "__main__":

    main()
