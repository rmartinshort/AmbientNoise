#!/usr/bin/env python 
#RMS 2017 
#Prepare an interpolated 3D model dataset for viewing with the 3DVis 
#software and with GMT scripts

import pandas as pd
import numpy as np
import os
import sys


def main():

    df = pd.read_csv('alaskastations.3d.smooth.mod',sep=' ',names=['longitude','latitude','depth','Vs'])

    Rearth = 6371
   # prepforvis(df,Rearth)
    depthsliceGMT(df,sliceinc=5)


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


    dvals = df.depth.values[:150]

    for depth in dvals:

        print depth
        
        if (depth%sliceinc == 0):
            slicename = 'dslice_%g.dat' %depth
            slicename_pert = 'dslice_%g_pert.dat' %depth
            grdname = 'dslice_%g.grd' %depth
            grdname_pert = 'dslice_%g_pert.grd' %depth
            
            print 'Slicing at depth %g' %depth
            dslice = df[df.depth == depth]

            dslice_mean = np.mean(dslice.Vs)
            dslice_pert_arr = 100*(np.array(dslice.Vs)-dslice_mean)/dslice_mean

            #remove depth and radius cols


            #Mask the absolute velocity slice
            dslice = mask_data(dslice)
            dslice = dslice.drop(['depth'],1)
            dslice_pert = dslice.copy()
            dslice = dslice.drop(['Vs'],1)

            #Add the perturbation vector and mask the dataframe
            dslice_pert['pert'] = dslice_pert_arr
            dslice_pert = dslice_pert.drop(['Vs'],1)
            dslice_pert = mask_data_pert(dslice_pert)
            dslice_pert = dslice_pert.drop(['pert'],1)

            dslice.to_csv(slicename,index=False,header=0,sep=' ')
            dslice_pert.to_csv(slicename_pert,index=False,header=0,sep=' ')

            print 'Generating .grd %s' %grdname

            os.system('gmt xyz2grd %s -G%s -I0.2 -R191/237/56.0/75.0' %(slicename,grdname))
            os.system('gmt xyz2grd %s -G%s -I0.2 -R191/237/56.0/75.0' %(slicename_pert,grdname_pert))

def mask_line(x):

   '''Equation of the line that divides the masked region from the non-masked region'''

   c = -4.2
   m = -0.5
   y = m*x + c
   return y


def mask_data_pert(depslice):

   newvels = []


   for index,row in depslice.iterrows():
      lon = row.longitude - 360
      y = mask_line(lon)
      lat = row.latitude
      #print row.pert
      if lat >= y:
         newvels.append('NaN') 
      elif lat >= 70: #67.95:
         newvels.append('NaN')
      else:
         newvels.append(row.pert)

   depslice['Masked_vals'] = newvels

   return depslice

def mask_data(depslice):

   newvels = []

   for index,row in depslice.iterrows():
      lon = row.longitude - 360
      y = mask_line(lon)
      lat = row.latitude
      #print row.pert
      if lat >= y:
         newvels.append('NaN') 
      elif lat >= 70: #67.95:
         newvels.append('NaN')
      else:
         newvels.append(row.Vs)

   depslice['Masked_vals'] = newvels

   return depslice

 
if __name__ == "__main__":

    main()




