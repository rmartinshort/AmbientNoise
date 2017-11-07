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
    stationsfile = "Alaska_stations.txt"

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

        print dirname,stlon,stlat

        if os.path.isdir(dirname):

            os.chdir(dirname)

            print "In dir %s" %dirname

            #Open the final velocity model file
            try:
                velout = pd.read_csv('end.mod',skiprows=12,sep='\s+',header=None,names=['thickness','vp','vs','rho','qp','qs','etap','etas','frefp','frefs'])
                depths = np.cumsum(velout.thickness.values)
                layers = velout.thickness.values
                Vs = velout.vs.values

                #Adjust the depths vector so that each vel point is the center of a depth layer
                for i in range(len(layers)):
                    depths[i] = float(depths[i]) - float(layers[i])/2.0

                #Get the depth at which the velocity gradient is a maximum - this corresponds to the moho
                velgrad = np.gradient(Vs)
                ind = np.argmax(velgrad)
                mohodep = depths[ind]

                #Make a plot for debugging purposes
                #plt.figure()
                #plt.plot(depths,velgrad)
                #plt.xlabel('Depth[km]')
                #plt.ylabel('Gradient in Vs')
                #plt.savefig('Vs_gradient.pdf')

                if (10 <= mohodep <= 60):
                    mohodepthfile.write('%g %g %g %s\n' %(stlon,stlat,mohodep,dirname))

            except:

                print "Something went wrong in dir %s. Probably end.mod not found" %dirname

        os.chdir(datadir)


    mohodepthfile.close()


if __name__ == "__main__":

    main()
