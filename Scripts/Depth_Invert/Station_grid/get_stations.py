#!/usr/bin/env python

#RMS Oct 2017
#Generate a flie of station coordinates to be used to generate folders for the joint inversion
#of surface waves and RFs, using ther CPS codes

import obspy as op
from obspy.clients.fdsn import Client as fdsnClient
import os
import glob
import sys
import argparse
import numpy as np

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('-addocean',action='store_true',dest='ghoststations',default=False,help="Use this parameter to construct a grid over the ocean region and extract\
    surface wave velocities there. Requires specification of a topo file")
    results = parser.parse_args()

    #This is where the file will be located
    #datadir = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_ALL_phonly_500_JOINT_oct17/1.0_only"
    #datadir = "/home/rmartinshort/Documents/Berkeley/funclab/RF_TA_2017/RAYP_BAZ_STACK/Joint_RF"
    datadir = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/Station_grid/Alaska_stations_plus_ghost_400_AK_TA"
    datadir = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/Station_grid/Alaska_stations_plus_ghost_400_RFJ_2014_2.5"
    datadir = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/Station_grid/Alaska_stations_plus_ghost_Miller_RF_2.5"
    cwd = os.getcwd()

    if not os.path.exists(datadir):
        print "Provided dir %s does not exist!" %datadir
        sys.exit(1)

    os.chdir(datadir)

    starttime = "2017-04-02"
    endtime = "2017-10-01"
    latmin = 48
    latmax = 78
    lonmin = -177
    lonmax = -113
    client = fdsnClient("IRIS")

    #Get the stations recording between start and endtime, which were installed after the start time
    inv = client.get_stations(startafter=op.UTCDateTime(starttime),starttime=op.UTCDateTime(starttime),endtime=op.UTCDateTime(endtime)\
    ,network='TA',level="channel",loc="*",channel="BHZ",minlatitude=latmin,maxlatitude=latmax,\
    minlongitude=lonmin,maxlongitude=lonmax)

    outfile = open('Alaska_stations_TA_april2017.txt','w')

    for network in inv:
        for station in network:
            stname = '%s_%s' %(network.code,station.code)
            stlon = station.longitude
            stlat = station.latitude
            outfile.write('%g %g NA %s 1\n' %(stlon,stlat,stname))

    outfile.close()

    if results.ghoststations == True:
        generate_ghost_stations()

    os.chdir(cwd)



def generate_ghost_stations():

    '''Constructs a grid with increment inc across the region of interest. Uses grdtrack to determine which points are
    offshore and writes them to a new file. This can then be concatinated with the file produced by the station collection
    utility'''

    topofile = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/plotting/plottingdata/Alaska_gebco.nc"

    #bounding box of the region of interest
    #may need to change this
    lonmin = 200
    lonmax = 224
    latmin = 58
    latmax = 68
    inc = 1

    #open a file to write - will then call grdtrack to sample
    outfile = open('dpoints.dat','w')

    #generate grid
    x = np.arange(lonmin-inc,lonmax+inc,inc)
    y = np.arange(latmin-inc,latmax+inc,inc)

    #loop through coordinates
    for lon in x:
        lon = lon-360
        for lat in y:
            outfile.write('%g %g\n' %(lon,lat))

    outfile.close()

    #do the interpolation
    os.system('gmt grdtrack dpoints.dat -G%s > dps.dat' %topofile)

    #Write points that are offshore to a new file
    infile = open('dps.dat','r')
    outfile = open('Ghost_station_coords.dat','w')
    lines = infile.readlines()

    ghostID=1
    for line in lines:
        vals = line.split()
        lon = float(vals[0])
        lat = float(vals[1])
        h = float(vals[2])

        if h < -10:
            stationname = "ghost_%i" %ghostID
            outfile.write('%g %g NA %s 1\n' %(lon,lat,stationname))
            ghostID+=1

    infile.close()
    outfile.close()
    os.system('rm dps.dat')




if __name__ == "__main__":

    #generate_ghost_stations()

    main()
