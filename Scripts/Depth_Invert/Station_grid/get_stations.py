#!/usr/bin/env python
import obspy as op
from obspy.clients.fdsn import Client as fdsnClient
import os
import glob

def main():

    starttime = "2014-01-01"
    endtime = "2017-08-01"
    latmin = 52
    latmax = 73
    lonmin = -171
    lonmax = -123
    client = fdsnClient("IRIS")

    inv = client.get_stations(starttime=op.UTCDateTime(starttime),endtime=op.UTCDateTime(endtime)\
    ,network='*',level="channel",loc="*",channel="BHZ",minlatitude=latmin,maxlatitude=latmax,\
    minlongitude=lonmin,maxlongitude=lonmax)

    outfile = open('Alaska_stations.txt','w')

    for network in inv:
        for station in network:
            stname = '%s_%s' %(network.code,station.code)
            stlon = station.longitude
            stlat = station.latitude
            outfile.write('%g %g NA %s 1\n' %(stlon,stlat,stname))

    outfile.close()



if __name__ == "__main__":

    main()
