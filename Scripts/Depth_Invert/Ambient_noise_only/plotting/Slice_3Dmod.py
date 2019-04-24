#!/usr/bin/env python 
#RMS 2017 
#Prepare an interpolated 3D model dataset for viewing with the 3DVis 
#software and with GMT scripts

import pandas as pd
import numpy as np
import os
import sys


def main():

    df = pd.read_csv('alaska.3d.mod.smooth',sep=' ',names=['longitude','latitude','depth','Vs'])

    Rearth = 6371
   # prepforvis(df,Rearth)
    depthsliceGMT(df,sliceinc=1)


def prepforvis(df,Rearth):
    '''Writes a file that can be read by the 3D Visualizer'''

    loninc = 0.5
    latinc = 0.5

    deps = df.depth.values
    lons = df.longitude.values
    lats = df.latitude.values

    depmin = min(deps)
    depmax = max(deps)
    depinc = deps[1]-deps[0]

    lonmin = min(lons)
    lonmax = max(lons)

    latmin = min(lats)
    latmax = max(lats)

    latno = np.arange(latmin,latmax,latinc)
    lonno = np.arange(lonmin,lonmax,loninc)
    depno = np.arange(depmin,depmax,depinc)

    llat = len(latno)+1
    llon = len(lonno)+1
    llde = len(depno)+1

    if (llat)*(llon)*(llde) != len(deps):

        print llat*llon*llde 
        print len(deps)

        print 'Node number calculation went wrong! This should not happen'
        sys.exit(1)

    else:

        print '# nodes in lat: %g' %llat
        print '# nodes in lon: %g' %llon
        print '# nodes in dep: %g' %llde


    df.depth = Rearth - df.depth
    df.to_csv('exout_3DVis.dat',header=0,sep=' ',index=False)
    #Convert back to depth 
    df.depth = Rearth - df.depth

def depthsliceGMT(df,sliceinc=5):

    '''Produce depth slices though a model for plotting in GMT'''


    dvals = df.depth.values[:181]

    for depth in dvals:

        print depth
        
        if (depth%sliceinc == 0):
            slicename = 'dslice_%g.dat' %depth
            grdname = 'dslice_%g.grd' %depth
            
            print 'Slicing at depth %g' %depth
            dslice = df[df.depth == depth]

            #remove depth and radius cols
            dslice = dslice.drop(['depth'],1)

            dslice.to_csv(slicename,index=False,header=0,sep=' ')
            print 'Generating .grd %s' %grdname
            os.system('gmt surface %s -G%s -I0.1 -R189/237/56.0/73.0' %(slicename,grdname))

if __name__ == '__main__':

    main()




