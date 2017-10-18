#!/usr/bin/env python

#RMS Oct 2017
#Generate a flie of station coordinates to be used to generate folders for the joint inversion
#of surface waves and RFs, using ther CPS codes

import obspy as op
from obspy.clients.fdsn import Client as fdsnClient
import os
import glob
import sys

def main():

    #This is where the file will be located
    datadir = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_ALL_phonly_500_JOINT_oct17/1.0_only"
    cwd = os.getcwd()

    if not os.path.exists(datadir):
        print "Provided dir %s does not exist!" %datadir
        sys.exit(1)

    os.chdir(datadir)

    starttime = "2017-01-01"
    endtime = "2017-08-01"
    latmin = 50
    latmax = 78
    lonmin = -178
    lonmax = -115
    client = fdsnClient("IRIS")

    inv = client.get_stations(starttime=op.UTCDateTime(starttime),endtime=op.UTCDateTime(endtime)\
    ,network='TA',level="channel",loc="*",channel="BHZ",minlatitude=latmin,maxlatitude=latmax,\
    minlongitude=lonmin,maxlongitude=lonmax)

    outfile = open('Alaska_stations_coordinatesTA.txt','w')

    for network in inv:
        for station in network:
            stname = '%s_%s' %(network.code,station.code)
            stlon = station.longitude
            stlat = station.latitude
            outfile.write('%g %g NA %s 1\n' %(stlon,stlat,stname))

    outfile.close()

    os.chdir(cwd)


if __name__ == "__main__":

    main()
