#!/usr/bin/env python
#RMS Nov 2017
#Enter a directory and use the gradient of the final velocity model to pick the moho position. Write the points to a file which then can be plotted
#with GMT

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import glob
import os

def main():

    cwd = os.getcwd()
    datadir = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/Station_grid/Alaska_stations_plus_ghost_500_RFJ_both"
    datadir = "/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/Station_grid/Alaska_stations_plus_ghost_RF_1999_2017_ALL"
    stationsfile = "live_stations.txt"

    if not os.path.isdir(datadir):
        print "Cannot find given directory %s" %datadir
        sys.exit(1)

    else:
        os.chdir(datadir)

    if not os.path.exists(stationsfile):
        print "Cannot find %s" %stationsfile
        sys.exit(1)


    station_db =  pd.read_csv(stationsfile,sep=' ',names=['Lon','Lat','tmp','Code','tmp2'])
    mohodepthfile = open('Moho_depth.dat','w')

    for index, row in station_db.iterrows():

        dirname = row.Code
        stlon = row.Lon + 360.0
        stlat = row.Lat

        #print dirname,stlon,stlat

        if os.path.isdir(dirname):

            os.chdir(dirname)

            print "In dir %s" %dirname

            #Open the final velocity model file
            try:
                velout = pd.read_csv('end.mod',skiprows=12,sep='\s+',header=None,names=['thickness','vp','vs','rho','qp','qs','etap','etas','frefp','frefs'])
                depths = np.cumsum(velout.thickness.values)
                layers = velout.thickness.values
                Vs = velout.vs.values

                #print indstart,indend

                #Adjust the depths vector so that each vel point is the center of a depth layer
                for i in range(len(layers)):
                    depths[i] = float(depths[i]) - float(layers[i])/2.0

                #Criteria is that moho must be associated with these velocities
                deps = depths[(depths>=20)]
                #print mohovels

                indstart, = np.where( depths==deps[0] )
                #indend, = np.where( Vs==vels[-1] )
                                                      
                mohovels = Vs[indstart:]

                #Get the depth at which the velocity gradient is a maximum - this corresponds to the moho
                mohograd = np.gradient(mohovels)
                velgrad = np.gradient(Vs)
                ind = np.argmax(mohograd)
                mohodep = depths[ind+indstart]

                #Make a plot for debugging purposes
                plt.figure()
                plt.plot(depths,velgrad)
                #plt.axvline(x=depths[indstart],ymin=-0.1,ymax=0.1,color='k')
                #plt.axvline(x=depths[indend],ymin=-0.1,ymax=0.1,color='k')
                plt.axvline(x=depths[ind+indstart],ymin=-0.1,ymax=3,color='r',linewidth=4)
                plt.xlabel('Depth[km]')
                plt.ylabel('Gradient in Vs')
                plt.savefig('Vs_gradient.pdf')
                plt.close()

                if (20 <= mohodep <= 60):
                   mohodepthfile.write('%g %g %g %s\n' %(stlon,stlat,mohodep,dirname))
                else:
                   print "Mohodep of %s at station %s" %(mohodep, dirname)

            except:

                print "Something went wrong in dir %s. Probably end.mod not found" %dirname

        os.chdir(datadir)


    mohodepthfile.close()


if __name__ == "__main__":

    main()
