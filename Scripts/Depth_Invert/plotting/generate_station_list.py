#!/usr/bin/env python

#RMS Nov 2017
#Generate list of stations in this dir, so we can plot them on a map

import glob
import pandas as pd
import os


def main():


     stationsfile = 'Alaska_stations.txt'
     station_db = pd.read_csv(stationsfile,sep=' ',names=['Lon','Lat','tmp','Code','tmp2'])

     livestationinfo = open('live_stations.txt','w')
     livestationnames = open('live_station_names.txt','w')

     TAstations = open('TA_stations.txt','w')
     Otherstations = open('Other_stations.txt','w')

     station_names = list(station_db.Code.values)

     alldirs = glob.glob('*_*')

     for element in alldirs:

         if os.path.isdir(element):
             if element in station_names:

                 station_info = station_db[station_db.Code == element]
                 livestationinfo.write('%g %g %s %s %s\n' %(station_info.Lon.values[0],station_info.Lat.values[0],\
                 station_info.tmp.values[0],station_info.Code.values[0],station_info.tmp2.values[0]))
                 livestationnames.write('%s\n' %station_info.Code.values[0])

                 network = station_info.Code.values[0].split('_')[0]
                 if network == 'TA':
                      TAstations.write('%g %g %s\n' %(station_info.Lon.values[0],station_info.Lat.values[0],station_info.Code.values[0]))
                 elif network != "ghost":
                      Otherstations.write('%g %g %s\n' %(station_info.Lon.values[0],station_info.Lat.values[0],station_info.Code.values[0]))
                 


     livestationnames.close()
     livestationinfo.close()
     Otherstations.close()
     TAstations.close()

if __name__ == "__main__":

    main()
